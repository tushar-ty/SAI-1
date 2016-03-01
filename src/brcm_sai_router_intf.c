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
#                        Router interface functions                            #
################################################################################
*/
/*
* Routine Description:
*    Create router interface.
*
* Arguments:
*    [out] rif_id - router interface id
*    [in] attr_count - number of attributes
*    [in] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_create_router_interface(_Out_ sai_object_id_t* rif_id,
                                 _In_ sai_uint32_t attr_count,
                                 _In_ sai_attribute_t *attr_list)
{
    int i, type = -1;
    sai_status_t rv;
    opennsl_vlan_t vid;
    opennsl_l3_intf_t l3_intf;
    sai_int32_t port = -1;
    sai_vlan_port_t port_list;
    opennsl_l2_station_t l2_stn;
    int station_id = 0;
    bool imac = FALSE;
    opennsl_vrf_t vrf = 0;
    opennsl_mac_t dst_mac_mask = {0xff,0xff, 0xff, 0xff, 0xff, 0xff};

    BRCM_SAI_FUNCTION_ENTER(SAI_API_ROUTER_INTERFACE);
    BRCM_SAI_SWITCH_INIT_CHECK;
    BRCM_SAI_OBJ_CREATE_PARAM_CHK(rif_id);

    for (i=0; i<attr_count; i++)
    {
        if (SAI_ROUTER_INTERFACE_ATTR_TYPE == attr_list[i].id)
        {
            type = attr_list[i].value.u32;
            break;
        }
    }
    if (-1 == type)
    {
        BRCM_SAI_LOG_RINTF(SAI_LOG_ERROR, "No interface type.\n");
        return SAI_MANDATORY_ATTRIBUTE_MISSING;
    }
    opennsl_l3_intf_t_init(&l3_intf);
    for (i=0; i<attr_count; i++)
    {
        switch (attr_list[i].id)
        {
            case SAI_ROUTER_INTERFACE_ATTR_TYPE:
                break;
            case SAI_ROUTER_INTERFACE_ATTR_VIRTUAL_ROUTER_ID:
            {
                vrf = BRCM_SAI_ATTR_LIST_OBJ_VAL(opennsl_vrf_t, i);
                if (false == _brcm_sai_vrf_valid(vrf))
                {
                    BRCM_SAI_LOG_RINTF(SAI_LOG_ERROR, "Invalid vrf value.\n");
                    return SAI_STATUS_INVALID_PARAMETER;
                }
                l3_intf.l3a_vrf = vrf;
                break;
            }
            case SAI_ROUTER_INTERFACE_ATTR_PORT_ID:
                if (SAI_ROUTER_INTERFACE_TYPE_PORT != type)
                {
                    BRCM_SAI_LOG_RINTF(SAI_LOG_ERROR, "Type value mismatch.\n");
                    return SAI_STATUS_INVALID_PARAMETER;
                }
                port = attr_list[i].value.u32;
                /* For port interaces, get vlan */
                l3_intf.l3a_vid = _brcm_sai_get_max_unused_vlan_id();
                if (0 == l3_intf.l3a_vid)
                {
                    BRCM_SAI_LOG_RINTF(SAI_LOG_ERROR, "No free vlans found\n");
                    return SAI_STATUS_FAILURE;
                }
                /* Create vlan */
                rv = vlan_apis.create_vlan(l3_intf.l3a_vid);
                if (SAI_STATUS_SUCCESS != rv)
                {
                    BRCM_SAI_LOG_RINTF(SAI_LOG_ERROR,
                                       "Creating new vlan %d failed\n",
                                       (int)l3_intf.l3a_vid);
                    return rv;
                }
                /* Add port to vlan */
                port_list.port_id = port;
                port_list.tagging_mode = SAI_VLAN_PORT_UNTAGGED;
                rv = vlan_apis.add_ports_to_vlan(l3_intf.l3a_vid, 1, &port_list);
                if (SAI_STATUS_SUCCESS != rv)
                {
                    BRCM_SAI_LOG_RINTF(SAI_LOG_ERROR,
                                       "Error Adding port %d to new vlan %d\n",
                                       port, (int)l3_intf.l3a_vid);
                    return rv;
                }
                BRCM_SAI_LOG_RINTF(SAI_LOG_DEBUG,
                                   "Added port %d to new vlan %d\n",
                                   port, (int)l3_intf.l3a_vid);
                /* Add vlan PVID value for untagged packets */
                rv = opennsl_port_untagged_vlan_set(0, port, l3_intf.l3a_vid);
                if (SAI_STATUS_SUCCESS != rv)
                {
                    BRCM_SAI_LOG_RINTF(SAI_LOG_ERROR,
                                       "Error in port %d setting untagged "
                                       "vlan %d\n",
                                       port, (int)l3_intf.l3a_vid);
                    BRCM_RV_OPENNSL_TO_SAI(rv);
                }
                /* Remove port from default vlan */
                rv = opennsl_vlan_default_get(0, &vid);
                if (SAI_STATUS_SUCCESS != rv)
                {
                    BRCM_SAI_LOG_RINTF(SAI_LOG_ERROR,
                                       "Error getting default vid\n");
                    BRCM_RV_OPENNSL_TO_SAI(rv);
                }
                port_list.port_id = port;
                rv = vlan_apis.remove_ports_from_vlan(vid, 1, &port_list);
                if (SAI_STATUS_SUCCESS != rv)
                {
                    BRCM_SAI_LOG_RINTF(SAI_LOG_ERROR,
                                       "Removing port %d from default "
                                       "vlan %d failed.\n",
                                       port, vid);
                    return rv;
                }
                break;
            case SAI_ROUTER_INTERFACE_ATTR_VLAN_ID:
                if (SAI_ROUTER_INTERFACE_TYPE_VLAN != type)
                {
                    BRCM_SAI_LOG_RINTF(SAI_LOG_ERROR, "Type value mismatch.\n");
                    return SAI_STATUS_INVALID_PARAMETER;
                }
                l3_intf.l3a_vid = attr_list[i].value.u32;
                break;
            case SAI_ROUTER_INTERFACE_ATTR_SRC_MAC_ADDRESS:
                memcpy(l3_intf.l3a_mac_addr, attr_list[i].value.mac,
                       sizeof(l3_intf.l3a_mac_addr));
                imac = TRUE;
                break;
            default:
                BRCM_SAI_LOG_RINTF(SAI_LOG_INFO,
                                   "Unknown attribute %d passed\n",
                                   attr_list[i].id);
                break;
        }
    }
    if (0 == vrf)
    {
        BRCM_SAI_LOG_RINTF(SAI_LOG_ERROR, "No VR ID provided for interface.\n");
        return SAI_MANDATORY_ATTRIBUTE_MISSING;
    }
    if (SAI_ROUTER_INTERFACE_TYPE_PORT == type && -1 == port)
    {
        BRCM_SAI_LOG_RINTF(SAI_LOG_ERROR, "No port id for port interface.\n");
        return SAI_MANDATORY_ATTRIBUTE_MISSING;
    }
    if (FALSE == imac)
    {
        if (_brcm_sai_vrf_info(vrf, (sai_mac_t*)l3_intf.l3a_mac_addr) < 0)
        {
            BRCM_SAI_LOG_RINTF(SAI_LOG_ERROR, "Error retreiving VR mac.\n");
            return SAI_STATUS_ITEM_NOT_FOUND;
        }
    }
    l3_intf.l3a_ttl = _BRCM_SAI_VR_DEFAULT_TTL;
    rv = opennsl_l3_intf_create(0, &l3_intf);
    BRCM_SAI_API_CHK(SAI_API_ROUTER_INTERFACE, "L3 intf create", rv);
    *rif_id = BRCM_SAI_CREATE_OBJ(SAI_OBJECT_TYPE_ROUTER_INTERFACE,
                                  l3_intf.l3a_intf_id);
    vid = SAI_ROUTER_INTERFACE_TYPE_PORT == type ? 0 : l3_intf.l3a_vid;
    opennsl_l2_station_t_init(&l2_stn);
    l2_stn.flags = OPENNSL_L2_STATION_IPV4 | OPENNSL_L2_STATION_IPV6 | OPENNSL_L2_STATION_ARP_RARP;
    memcpy(l2_stn.dst_mac, l3_intf.l3a_mac_addr, sizeof(l2_stn.dst_mac));
    memcpy(l2_stn.dst_mac_mask, dst_mac_mask, sizeof(dst_mac_mask));

    BRCM_SAI_LOG_RINTF(SAI_LOG_DEBUG,
                       "Add my station for vid %d\n", vid);
    rv = opennsl_l2_station_add(0, &station_id, &l2_stn);
    BRCM_SAI_API_CHK(SAI_API_ROUTER_INTERFACE, "Add my stn entry", rv);

    _brcm_sai_rif_info_set(l3_intf.l3a_intf_id, type,
                           SAI_ROUTER_INTERFACE_TYPE_PORT == type ?
                           port : 0, vid, l3_intf.l3a_mac_addr);
    BRCM_SAI_FUNCTION_EXIT(SAI_API_ROUTER_INTERFACE);

    return rv;
}

/*
* Routine Description:
*    Remove router interface
*
* Arguments:
*    [in] rif_id - router interface id
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
sai_status_t
brcm_sai_remove_router_interface(_In_ sai_object_id_t rif_id)
{
    sai_status_t rv;
    opennsl_l3_intf_t l3_intf;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_ROUTER_INTERFACE);
    BRCM_SAI_SWITCH_INIT_CHECK;

    opennsl_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = BRCM_SAI_GET_OBJ_VAL(opennsl_if_t, rif_id);
    rv = opennsl_l3_intf_delete(0, &l3_intf);
    BRCM_SAI_API_CHK(SAI_API_ROUTER_INTERFACE, "L3 intf delete", rv);

    /* FIXME: Remove station-id and restore vlans. */

    BRCM_SAI_FUNCTION_EXIT(SAI_API_ROUTER_INTERFACE);

    return rv;
}

/*
* Routine Description:
*    Set router interface attribute
*
* Arguments:
*    [in] sai_router_interface_id_t - router_interface_id
*    [in] attr - attribute
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_set_router_interface_attribute(_In_ sai_object_id_t rif_id,
                                        _In_ const sai_attribute_t *attr)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_ROUTER_INTERFACE);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_ROUTER_INTERFACE);

    return rv;
}

/*
* Routine Description:
*    Get router interface attribute
*
* Arguments:
*    [in] sai_router_interface_id_t - router_interface_id
*    [in] attr_count - number of attributes
*    [inout] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_get_router_interface_attribute(_In_ sai_object_id_t rif_id,
                                        _In_ sai_uint32_t attr_count,
                                        _Inout_ sai_attribute_t *attr_list)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_ROUTER_INTERFACE);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_ROUTER_INTERFACE);

    return rv;
}

/*
################################################################################
#                                Functions map                                 #
################################################################################
*/
const sai_router_interface_api_t router_intf_apis = {
    brcm_sai_create_router_interface,
    brcm_sai_remove_router_interface,
    brcm_sai_set_router_interface_attribute,
    brcm_sai_get_router_interface_attribute
};
