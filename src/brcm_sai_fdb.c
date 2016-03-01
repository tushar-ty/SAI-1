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

#include <string.h>
#include <sai.h>
#include <brcm_sai_common.h>

/*
################################################################################
#                               Event handlers                                 #
################################################################################
*/
void
_brcm_sai_fdb_event_cb(int unit, opennsl_l2_addr_t *l2addr, int operation,
                       void *userdata)
{
    uint32_t attr_count = 0;
    sai_attribute_t attr[2];
    sai_fdb_event_notification_data_t notify;

    BRCM_SAI_LOG_FDB(SAI_LOG_INFO, "FDB event: %d\n", operation);

    if (NULL == host_callbacks.on_fdb_event)
    {
        return;
    }
    if (OPENNSL_L2_CALLBACK_ADD == operation)
    {
        notify.event_type = SAI_FDB_EVENT_LEARNED;
    }
    else if (OPENNSL_L2_CALLBACK_DELETE == operation)
    {
        notify.event_type = SAI_FDB_EVENT_AGED;
    }
    else
    {
        return;
    }
    attr[attr_count].id = SAI_FDB_ENTRY_ATTR_PORT_ID;
    attr[attr_count].value.oid = BRCM_SAI_CREATE_OBJ(SAI_OBJECT_TYPE_PORT, l2addr->port);
    attr_count++;
    memcpy(notify.fdb_entry.mac_address, l2addr->mac, sizeof(sai_mac_t));
    notify.fdb_entry.vlan_id = l2addr->vid;
    notify.attr_count = attr_count;
    notify.attr = attr;
    host_callbacks.on_fdb_event(1, &notify);
}


/*
################################################################################
#                               FDB functions                                  #
################################################################################
*/
/*
* Routine Description:
*    Create FDB entry
*
* Arguments:
*    [in] fdb_entry - fdb entry
*    [in] attr_count - number of attributes
*    [in] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_create_fdb_entry(_In_ const sai_fdb_entry_t* fdb_entry,
                          _In_ uint32_t attr_count,
                          _In_ const sai_attribute_t *attr_list)
{
    int i;
    sai_status_t rv;
    opennsl_l2_addr_t l2addr;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_FDB);
    BRCM_SAI_SWITCH_INIT_CHECK;
    BRCM_SAI_OBJ_CREATE_PARAM_CHK(fdb_entry);

    if (BRCM_SAI_MAC_IS_ZERO(fdb_entry->mac_address))
    {
        BRCM_SAI_LOG_FDB(SAI_LOG_INFO, "Null mac address passed.\n");
        return SAI_STATUS_INVALID_PARAMETER;
    }
    if (BRCM_SAI_MAC_IS_BCAST(fdb_entry->mac_address))
    {
        BRCM_SAI_LOG_FDB(SAI_LOG_INFO, "BCAST mac address not supported.\n");
        return SAI_STATUS_NOT_SUPPORTED;
    }
    if (BRCM_SAI_MAC_IS_MCAST(fdb_entry->mac_address))
    {
        BRCM_SAI_LOG_FDB(SAI_LOG_INFO, "MCAST mac address not supported.\n");
        return SAI_STATUS_NOT_SUPPORTED;
    }
    opennsl_l2_addr_t_init(&l2addr, fdb_entry->mac_address, fdb_entry->vlan_id);
    for (i=0; i<attr_count; i++)
    {
        switch (attr_list[i].id)
        {
            case SAI_FDB_ENTRY_ATTR_TYPE:
                if (SAI_FDB_ENTRY_STATIC == attr_list[i].value.s32)
                {
                    l2addr.flags |= OPENNSL_L2_STATIC;
                }
                break;
            case SAI_FDB_ENTRY_ATTR_PORT_ID:
                l2addr.port = BRCM_SAI_GET_OBJ_VAL(int, BRCM_SAI_ATTR_LIST_OBJ(i));
                break;
            case SAI_FDB_ENTRY_ATTR_PACKET_ACTION:
                break;
            case SAI_FDB_ENTRY_ATTR_CUSTOM_RANGE_BASE:
                l2addr.flags |= OPENNSL_L2_L3LOOKUP;
                break;
            default: 
                BRCM_SAI_LOG_FDB(SAI_LOG_INFO,
                                 "Un-supported attribute %d passed\n",
                                 attr_list[i].id);
                break;
        }
    }
    BRCM_SAI_LOG_FDB(SAI_LOG_DEBUG, "L2 port: %d\n", l2addr.port);
    rv = opennsl_l2_addr_add(0, &l2addr);
    BRCM_SAI_API_CHK(SAI_API_FDB, "Create FDB", rv);

    BRCM_SAI_FUNCTION_EXIT(SAI_API_FDB);

    return BRCM_RV_OPENNSL_TO_SAI(rv);
}

/*
* Routine Description:
*    Remove FDB entry
*
* Arguments:
*    [in] fdb_entry - fdb entry
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_remove_fdb_entry(_In_ const sai_fdb_entry_t* fdb_entry)
{
    sai_status_t rv;
    opennsl_mac_t mac;
    opennsl_vlan_t vid;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_FDB);
    BRCM_SAI_SWITCH_INIT_CHECK;

    if (NULL == fdb_entry)
    {
        return SAI_STATUS_INVALID_PARAMETER;
    }
    memcpy(mac, fdb_entry->mac_address, sizeof(opennsl_mac_t));
    vid = fdb_entry->vlan_id;
    rv = opennsl_l2_addr_delete(0, mac, vid);
    BRCM_SAI_API_CHK(SAI_API_FDB, "Remove FDB", rv);

    BRCM_SAI_FUNCTION_EXIT(SAI_API_FDB);

    return BRCM_RV_OPENNSL_TO_SAI(rv);
}

/*
* Routine Description:
*    Set fdb entry attribute value
*
* Arguments:
*    [in] fdb_entry - fdb entry
*    [in] attr - attribute
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_set_fdb_entry_attribute(_In_ const sai_fdb_entry_t* fdb_entry,
                                 _In_ const sai_attribute_t *attr)
{
    opennsl_l2_addr_t l2addr;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_FDB);
    BRCM_SAI_SWITCH_INIT_CHECK;

    if ((NULL == fdb_entry) || (NULL == attr))
    {
        return SAI_STATUS_INVALID_PARAMETER;
    }
    switch (attr->id)
    {
        case SAI_FDB_ENTRY_ATTR_TYPE:
            if (SAI_FDB_ENTRY_STATIC == attr->value.s32)
            {
                l2addr.flags |= OPENNSL_L2_STATIC;
            }
            /*FIXME*/
            break;
        case SAI_FDB_ENTRY_ATTR_PORT_ID:
            l2addr.port = BRCM_SAI_GET_OBJ_VAL(int, attr->value.oid);
            /*opennsl_l2_replace()*/
            break;
        case SAI_FDB_ENTRY_ATTR_PACKET_ACTION:
            break;
        default: break;
    }

    BRCM_SAI_FUNCTION_EXIT(SAI_API_FDB);

    return SAI_STATUS_SUCCESS;
}

/*
* Routine Description:
*    Get fdb entry attribute value
*
* Arguments:
*    [in] fdb_entry - fdb entry
*    [in] attr_count - number of attributes
*    [inout] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_get_fdb_entry_attribute(_In_ const sai_fdb_entry_t* fdb_entry,
                                 _In_ uint32_t attr_count,
                                 _Inout_ sai_attribute_t *attr_list)
{
    int i;
    sai_status_t rv;
    opennsl_l2_addr_t l2addr;
    opennsl_mac_t mac;
    opennsl_vlan_t vid;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_FDB);
    BRCM_SAI_SWITCH_INIT_CHECK;

    if ((NULL == fdb_entry) || (NULL == attr_list))
    {
        return SAI_STATUS_INVALID_PARAMETER;
    }
    memcpy(mac, fdb_entry->mac_address, sizeof(opennsl_mac_t));
    vid = fdb_entry->vlan_id;
    memset(&l2addr, 0, sizeof(opennsl_l2_addr_t));
    rv = opennsl_l2_addr_get(0, mac, vid, &l2addr);
    BRCM_SAI_API_CHK(SAI_API_FDB, "FDB attrib get", rv);

    for (i=0; i<attr_count; i++)
    {
        switch(attr_list[i].id)
        {
            case SAI_FDB_ENTRY_ATTR_TYPE:
                if (l2addr.flags & OPENNSL_L2_STATIC)
                {
                    attr_list[i].value.s32 = SAI_FDB_ENTRY_STATIC;
                }
                else
                {
                    attr_list[i].value.s32 = SAI_FDB_ENTRY_DYNAMIC;
                }
                break;
            case SAI_FDB_ENTRY_ATTR_PORT_ID:
                BRCM_SAI_ATTR_LIST_OBJ(i) =
                    BRCM_SAI_CREATE_OBJ(SAI_OBJECT_TYPE_PORT, l2addr.port);
                break;
            case SAI_FDB_ENTRY_ATTR_PACKET_ACTION:
                break;
            default: break;
        }
    }

    BRCM_SAI_FUNCTION_EXIT(SAI_API_FDB);

    return BRCM_RV_OPENNSL_TO_SAI(rv);
}

/*
* Routine Description:
*    Remove all FDB entries by attribute set in sai_fdb_flush_attr
*
* Arguments:
*    [in] attr_count - number of attributes
*    [in] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_flush_fdb_entries(_In_ uint32_t attr_count,
                           _In_ const sai_attribute_t *attr_list)
{
    return SAI_STATUS_NOT_IMPLEMENTED;
}

/*
################################################################################
#                                Functions map                                 #
################################################################################
*/
const sai_fdb_api_t fdb_apis = {
    brcm_sai_create_fdb_entry,
    brcm_sai_remove_fdb_entry,
    brcm_sai_set_fdb_entry_attribute,
    brcm_sai_get_fdb_entry_attribute,
    brcm_sai_flush_fdb_entries
};
