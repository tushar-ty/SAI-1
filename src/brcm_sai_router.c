/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2013-2016
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 **********************************************************************/

#include <sai.h>
#include <brcm_sai_common.h>

/*
################################################################################
#                                Local state                                   #
################################################################################
*/
typedef struct _brcm_sai_vr_info_s {
    opennsl_vrf_t vr_id;
    opennsl_if_t l3_if_id;
    opennsl_if_t l3_drop_id;
    sai_mac_t vr_mac;
}_brcm_sai_vr_info_t;
static _brcm_sai_vr_info_t *_brcm_sai_vrf_map = NULL;
static sai_uint32_t _brcm_sai_vr_count = 0;
static sai_uint32_t _brcm_sai_vr_max;

/*
################################################################################
#                              Router functions                                #
################################################################################
*/
/*
* Routine Description:
*    Create virtual router
*
* Arguments:
*    [in] vr_id - virtual router id
*    [in] attr_count - number of attributes
*    [in] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_create_virtual_router(_Out_ sai_object_id_t *vr_id,
                               _In_ sai_uint32_t attr_count,
                               _In_ const sai_attribute_t *attr_list)
{
    int i;
    bool vmac = FALSE;
    sai_status_t rv = SAI_STATUS_SUCCESS;
    opennsl_l3_intf_t l3_intf;
    opennsl_l3_egress_t l3_eg;
    opennsl_if_t l3_if_id = -1;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_VIRTUAL_ROUTER);
    BRCM_SAI_SWITCH_INIT_CHECK;

    if (_brcm_sai_vr_count == _brcm_sai_vr_max)
    {
        return SAI_STATUS_INSUFFICIENT_RESOURCES;
    }
    /* Skip checking non-zero attributes for now as the system mac is used below */
    if (NULL == vr_id)
    {
        return SAI_STATUS_INVALID_PARAMETER;
    }
    /* Search for an unused id */
    for (i=1; i<=_brcm_sai_vr_max; i++)
    {
        if (0 == _brcm_sai_vrf_map[i].vr_id)
        {
            break;
        }
    }
    if (i > _brcm_sai_vr_max)
    {
        BRCM_SAI_LOG_VR(SAI_LOG_ERROR, "Unexpected vrf resource issue.\n");
        return SAI_STATUS_FAILURE;
    }
    BRCM_SAI_LOG_VR(SAI_LOG_DEBUG, "Using vr_id: %d\n", i);
    *vr_id = BRCM_SAI_CREATE_OBJ(SAI_OBJECT_TYPE_VIRTUAL_ROUTER, i);
    _brcm_sai_vrf_map[i].vr_id = i;
    _brcm_sai_vr_count++;

    opennsl_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_ttl = _BRCM_SAI_VR_DEFAULT_TTL;
    l3_intf.l3a_vrf = i;
    l3_intf.l3a_vid = 1;
    for (i=0; i<attr_count; i++)
    {
        if (SAI_VIRTUAL_ROUTER_ATTR_SRC_MAC_ADDRESS == attr_list[i].id)
        {
            memcpy(l3_intf.l3a_mac_addr, attr_list[i].value.mac,
                   sizeof(l3_intf.l3a_mac_addr));
            vmac = TRUE;
            break;
        }
    }
    if (FALSE == vmac)
    {
        memcpy(l3_intf.l3a_mac_addr, _brcm_sai_switch_system_mac_get(),
               sizeof(l3_intf.l3a_mac_addr));
    }
    rv = opennsl_l3_intf_create(0, &l3_intf);
    BRCM_SAI_API_CHK(SAI_API_VIRTUAL_ROUTER, "L3 intf create", rv);
    BRCM_SAI_LOG_VR(SAI_LOG_DEBUG, "drop/trap intf created: %d\n",
                    l3_intf.l3a_intf_id);

    opennsl_l3_egress_t_init(&l3_eg);
    l3_eg.intf = l3_intf.l3a_intf_id;
    l3_eg.flags = OPENNSL_L3_DST_DISCARD;
    memcpy(l3_eg.mac_addr, l3_intf.l3a_mac_addr, sizeof(l3_eg.mac_addr));
    rv = opennsl_l3_egress_create(0, 0, &l3_eg, &l3_if_id);
    BRCM_SAI_API_CHK(SAI_API_VIRTUAL_ROUTER, "L3 egress create", rv);
    BRCM_SAI_LOG_VR(SAI_LOG_DEBUG, "drop L3 egress object id: %d\n", l3_if_id);
    _brcm_sai_vrf_map[l3_intf.l3a_vrf].l3_drop_id = l3_if_id;

    opennsl_l3_egress_t_init(&l3_eg);
    l3_eg.intf = l3_intf.l3a_intf_id;
    (void)_brcm_sai_virtual_router_flags_get(&l3_eg.flags);
    memcpy(l3_eg.mac_addr, l3_intf.l3a_mac_addr, sizeof(l3_eg.mac_addr));
    rv = opennsl_l3_egress_create(0, 0, &l3_eg, &l3_if_id);
    BRCM_SAI_API_CHK(SAI_API_VIRTUAL_ROUTER, "L3 egress create", rv);
    BRCM_SAI_LOG_VR(SAI_LOG_DEBUG, "trap L3 egress object id: %d\n", l3_if_id);
    _brcm_sai_vrf_map[l3_intf.l3a_vrf].l3_if_id = l3_if_id;

    memcpy(_brcm_sai_vrf_map[l3_intf.l3a_vrf].vr_mac, l3_intf.l3a_mac_addr,
           sizeof(sai_mac_t));

    BRCM_SAI_FUNCTION_EXIT(SAI_API_VIRTUAL_ROUTER);

    return rv;
}

/*
* Routine Description:
*    Remove virtual router
*
* Arguments:
*    [in] vr_id - virtual router id
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_remove_virtual_router(_In_ sai_object_id_t vr_id)
{
    sai_status_t rv = SAI_STATUS_SUCCESS;
    sai_uint32_t _vr_id = BRCM_SAI_GET_OBJ_VAL(sai_uint32_t, vr_id);

    BRCM_SAI_FUNCTION_ENTER(SAI_API_VIRTUAL_ROUTER);
    BRCM_SAI_SWITCH_INIT_CHECK;

    if (_brcm_sai_vr_max < _vr_id)
    {
        return SAI_STATUS_INVALID_PARAMETER;
    }
    _brcm_sai_vrf_map[_vr_id].vr_id = 0;
    _brcm_sai_vr_count--;
    BRCM_SAI_LOG_VR(SAI_LOG_DEBUG, "freeing vr_id: %d\n", _vr_id);

    BRCM_SAI_FUNCTION_EXIT(SAI_API_VIRTUAL_ROUTER);

    return rv;
}

/*
* Routine Description:
*    Set virtual router attribute Value
*
* Arguments:
*    [in] vr_id - virtual router id
*    [in] attr - attribute
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_set_virtual_router_attribute(_In_ sai_object_id_t vr_id,
                                      _In_ const sai_attribute_t *attr)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_VIRTUAL_ROUTER);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_VIRTUAL_ROUTER);

    return rv;
}

/*
* Routine Description:
*    Get virtual router attribute Value
*
* Arguments:
*    [in] vr_id - virtual router id
*    [in] attr_count - number of attributes
*    [in] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_get_virtual_router_attribute(_In_ sai_object_id_t vr_id,
                                      _In_ sai_uint32_t attr_count,
                                      _Inout_ sai_attribute_t *attr_list)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_VIRTUAL_ROUTER);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_VIRTUAL_ROUTER);

    return rv;
}

/*
################################################################################
#                                Internal functions                            #
################################################################################
*/

/* Routine to allocate vrf state */
sai_status_t
_brcm_sai_alloc_vrf(int max)
{
    if ((NULL == _brcm_sai_vrf_map) && max)
    {
        _brcm_sai_vrf_map = (_brcm_sai_vr_info_t*)malloc((max+1) *
                            sizeof(_brcm_sai_vr_info_t));
        if (NULL == _brcm_sai_vrf_map)
        {
            BRCM_SAI_LOG_VR(SAI_LOG_CRITICAL,
                            "Error allocating memory for vr state.\n");
            return SAI_STATUS_NO_MEMORY;
        }
        else
        {
            memset(_brcm_sai_vrf_map, 0, (max+1) * sizeof(_brcm_sai_vr_info_t));
            _brcm_sai_vr_max = max;
            BRCM_SAI_LOG_VR(SAI_LOG_INFO, "Max vrfs: %d.\n", max);
        }
    }
    return 0;
}

/* Routine to free vrf state */
void
_brcm_sai_free_vrf(void)
{
    if (NULL != _brcm_sai_vrf_map)
    {
        CHECK_FREE(_brcm_sai_vrf_map);
        _brcm_sai_vrf_map = NULL;
    }
}

/* Routine to verify if a vr_id is active/valid */
bool
_brcm_sai_vrf_valid(sai_uint32_t vr_id)
{
    if (NULL == _brcm_sai_vrf_map)
    {
        return false;
    }
    if (_brcm_sai_vr_max < vr_id)
    {
        return false;
    }
    if (_brcm_sai_vrf_map[vr_id].vr_id)
    {
        return true;
    }
    return false;
}

/* Routine to get vr properties */
int
_brcm_sai_vrf_info(sai_uint32_t vr_id, sai_mac_t *mac)
{
    if (_brcm_sai_vrf_valid(vr_id))
    {
        memcpy(mac, _brcm_sai_vrf_map[vr_id].vr_mac, sizeof(sai_mac_t));
        return 0;
    }
    return -1;
}

bool _brcm_sai_vr_id_valid(sai_uint32_t vr_id)
{
    return (vr_id < _brcm_sai_vr_max);
}


opennsl_if_t
_brcm_sai_vrf_if_get(sai_uint32_t vr_id)
{
    return _brcm_sai_vrf_map[vr_id].l3_if_id;
}


opennsl_if_t
_brcm_sai_vrf_drop_if_get(sai_uint32_t vr_id)
{
    return _brcm_sai_vrf_map[vr_id].l3_drop_id;
}

/*
################################################################################
#                                Functions map                                 #
################################################################################
*/
const sai_virtual_router_api_t router_apis = {
    brcm_sai_create_virtual_router,
    brcm_sai_remove_virtual_router,
    brcm_sai_set_virtual_router_attribute,
    brcm_sai_get_virtual_router_attribute
};
