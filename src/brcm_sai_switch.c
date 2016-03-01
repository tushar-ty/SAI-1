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
#                             Forward declarations                             #
################################################################################
*/
extern char* readline(const char *prompt);
void _brcm_sai_driver_shell();

/*
################################################################################
#                               Global state                                   #
################################################################################
*/
sai_switch_notification_t host_callbacks;

/*
################################################################################
#                                Local state                                   #
################################################################################
*/
static uint32_t _brcm_sai_switch_cookie = 0x56850;
/*
################################################################################
#                               Event handlers                                 #
################################################################################
*/
void
_brcm_sai_switch_event_cb(int unit, opennsl_switch_event_t event,
                          uint32 arg1, uint32 arg2, uint32 arg3, void *userdata)
{
    BRCM_SAI_LOG_SWITCH(SAI_LOG_CRITICAL,
                        "%d Received switch event %d on unit %d: %x %x %x\n",
                        *(int*)userdata, unit, event, arg1, arg2, arg3);
}

/*
################################################################################
#                              Switch functions                                #
################################################################################
*/

/*
* Routine Description:
*   SDK initialization. After the call the capability attributes should be
*   ready for retrieval via sai_get_switch_attribute().
*
* Arguments:
*   [in] profile_id - Handle for the switch profile.
*   [in] switch_hardware_id - Switch hardware ID to open
*   [in/opt] firmware_path_name - Vendor specific path name of the firmware
*                                     to load
*   [in] switch_notifications - switch notification table
* Return Values:
*   SAI_STATUS_SUCCESS on success
*   Failure status code on error
*/
STATIC sai_status_t
brcm_sai_initialize_switch(_In_ sai_switch_profile_id_t profile_id,
    _In_reads_z_(SAI_MAX_HARDWARE_ID_LEN) char* switch_hardware_id,
    _In_reads_opt_z_(SAI_MAX_FIRMWARE_PATH_NAME_LEN) char* firmware_path_name,
    _In_ sai_switch_notification_t* switch_notifications)
{
    int rv, val = 0;
    const char *k = "", *v = "";
    const char *start = NULL;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_SWITCH);

    if (NULL == switch_hardware_id)
    {
        BRCM_SAI_LOG_SWITCH(SAI_LOG_ERROR, "NULL switch_hardware_id passed\n");
        return SAI_STATUS_INVALID_PARAMETER;
    }
    else
    {
        BRCM_SAI_LOG_SWITCH(SAI_LOG_INFO,
                            "Host profile %d init request for: %s\n",
                            profile_id, switch_hardware_id);
    }
    if (NULL == switch_notifications)
    {
        BRCM_SAI_LOG_SWITCH(SAI_LOG_ERROR,
                            "NULL sai_switch_notification_t* passed\n");
        return SAI_STATUS_INVALID_PARAMETER;
    }
    if (true == _brcm_sai_switch_is_inited())
    {
        BRCM_SAI_LOG_SWITCH(SAI_LOG_ERROR,
                            "Switch already initialized - first call shutdown_switch\n");
        return SAI_STATUS_FAILURE;
    }
    memcpy(&host_callbacks, switch_notifications,
           sizeof(sai_switch_notification_t));
    if (host_services.profile_get_next_value)
    {
        /* reset KVPs */
        host_services.profile_get_next_value(profile_id, &start, NULL);
        /* get KVPs based upon profile_id */
        do
        {
            val = host_services.profile_get_next_value(profile_id, &k, &v);
            BRCM_SAI_LOG_SWITCH(SAI_LOG_DEBUG, "Retreiving KVP [%s:%s]\n", k, v);
        } while (val != -1);
    }
    /* init SDK */
    rv = opennsl_driver_init(NULL);
    if (OPENNSL_E_NONE != rv)
    {
        BRCM_SAI_LOG_SWITCH(SAI_LOG_CRITICAL,
                            "Error %d initializing SDK !!\n", rv);
        return SAI_STATUS_FAILURE;
    }
    rv = _brcm_sai_hostif_clean();
    if (OPENNSL_E_NONE != rv)
    {
        BRCM_SAI_LOG_SWITCH(SAI_LOG_CRITICAL,
                            "Error %d cleaning up hostif !!\n", rv);
        return SAI_STATUS_FAILURE;
    }
    /* Init default lls gport tree */
    rv = _brcm_sai_mmu_gport_init();
    if (OPENNSL_E_NONE != rv)
    {
        BRCM_SAI_LOG_SWITCH(SAI_LOG_CRITICAL,
                            "Error %d initializing lls gport tree !!\n", rv);
        return SAI_STATUS_FAILURE;
    }
    /* Register for switch events */
    rv = opennsl_switch_event_register(0, _brcm_sai_switch_event_cb,
                                       &_brcm_sai_switch_cookie);
    if (OPENNSL_E_NONE != rv)
    {
        BRCM_SAI_LOG_SWITCH(SAI_LOG_CRITICAL,
                            "Error %d registering for switch events !!\n", rv);
        return SAI_STATUS_FAILURE;
    }
    /* Register for link events */
    rv = opennsl_linkscan_register(0, _brcm_sai_link_event_cb);
    if (OPENNSL_E_NONE != rv)
    {
        BRCM_SAI_LOG_SWITCH(SAI_LOG_CRITICAL,
                            "Error %d registering for link events !!\n", rv);
        return SAI_STATUS_FAILURE;
    }
    rv = opennsl_l2_addr_register(0, _brcm_sai_fdb_event_cb, (void*)0x5A1092);
    if (OPENNSL_E_NONE != rv)
    {
        BRCM_SAI_LOG_SWITCH(SAI_LOG_CRITICAL,
                            "Error %d registering for fdb events !!\n", rv);
        return SAI_STATUS_FAILURE;
    }
    rv = _brcm_sai_vrf_max_get(&val);
    if (OPENNSL_E_NONE != rv)
    {
        BRCM_SAI_LOG_SWITCH(SAI_LOG_CRITICAL,
                            "Error %d retreiving max vrf !!\n", rv);
        return SAI_STATUS_FAILURE;
    }
    else
    {
        rv = _brcm_sai_alloc_vrf(val);
        if (0 != rv)
        {
            BRCM_SAI_LOG_SWITCH(SAI_LOG_CRITICAL,
                                "Error %d initializing vrf state !!\n", rv);
            return SAI_STATUS_FAILURE;
        }
    }
    rv = _brcm_sai_alloc_rif();
    if (0 != rv)
    {
        BRCM_SAI_LOG_SWITCH(SAI_LOG_CRITICAL,
                            "Error %d initializing rif state !!\n", rv);
        return SAI_STATUS_FAILURE;
    }
    /* Set L3 Egress Mode */
    rv =  opennsl_switch_control_set(0, opennslSwitchL3EgressMode, 1);
    if (OPENNSL_E_NONE != rv)
    {
        BRCM_SAI_LOG_SWITCH(SAI_LOG_CRITICAL,
                            "Error %d setting egress mode !!\n", rv);
        return SAI_STATUS_FAILURE;
    }

    /* Vlan related initialization */
    if (SAI_STATUS_SUCCESS != _brcm_sai_vlan_init())
    {
      BRCM_SAI_LOG_SWITCH(SAI_LOG_CRITICAL,
                          "Error %d initializing vlan module !!\n", rv);
      return SAI_STATUS_FAILURE;
    }

    if (SAI_STATUS_SUCCESS != _brcm_sai_initialize_switch())
    {
      BRCM_SAI_LOG_SWITCH(SAI_LOG_CRITICAL,
                          "Error %d initializing private variables!!\n", rv);
      return SAI_STATUS_FAILURE;
    }

    _brcm_sai_switch_init_set(true);

    BRCM_SAI_FUNCTION_EXIT(SAI_API_SWITCH);

    return SAI_STATUS_SUCCESS;
}

/*
* Routine Description:
*   Release all resources associated with currently opened switch
*
* Arguments:
*   [in] warm_restart_hint - hint that indicates controlled warm restart.
*                            Since warm restart can be caused by crash
*                            (therefore there are no guarantees for this call),
*                            this hint is really a performance optimization.
*
* Return Values:
*   None
*/
STATIC void
brcm_sai_shutdown_switch(_In_ bool warm_restart_hint)
{
    BRCM_SAI_FUNCTION_ENTER(SAI_API_SWITCH);

    memset(&host_callbacks, 0, sizeof(sai_switch_notification_t));
    _brcm_sai_free_vrf();
    _brcm_sai_free_rif();
    _brcm_sai_clear_port_state();
    _brcm_sai_switch_init_set(false);

    BRCM_SAI_FUNCTION_EXIT(SAI_API_SWITCH);
}

/*
* Routine Description:
*   SDK connect. This API connects library to the initialized SDK.
*   After the call the capability attributes should be ready for retrieval
*   via sai_get_switch_attribute().
*
* Arguments:
*   [in] profile_id - Handle for the switch profile.
*   [in] switch_hardware_id - Switch hardware ID to open
*   [in] switch_notifications - switch notification table
* Return Values:
*   SAI_STATUS_SUCCESS on success
*   Failure status code on error
*/
STATIC sai_status_t
brcm_sai_connect_switch(_In_ sai_switch_profile_id_t profile_id,
    _In_reads_z_(SAI_MAX_HARDWARE_ID_LEN) char* switch_hardware_id,
    _In_ sai_switch_notification_t* switch_notifications)
{
    int val = 0;
    const char *k = "", *v = "";
    const char *start = NULL;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_SWITCH);
    BRCM_SAI_SWITCH_INIT_CHECK;

    if (NULL == switch_hardware_id)
    {
        BRCM_SAI_LOG_SWITCH(SAI_LOG_ERROR, "NULL switch_hardware_id passed\n");
        return SAI_STATUS_INVALID_PARAMETER;
    }
    else
    {
        BRCM_SAI_LOG_SWITCH(SAI_LOG_INFO,
                            "Host profile %d connect request for: %s\n",
                            profile_id, switch_hardware_id);
    }
    if (NULL == switch_notifications)
    {
        BRCM_SAI_LOG_SWITCH(SAI_LOG_ERROR,
                            "NULL sai_switch_notification_t* passed\n");

        return SAI_STATUS_INVALID_PARAMETER;
    }
    memcpy(&host_callbacks, switch_notifications,
           sizeof(sai_switch_notification_t));
    if (host_services.profile_get_next_value)
    {
        /* reset KVPs */
        host_services.profile_get_next_value(profile_id, &start, NULL);
        /* get KVPs based upon profile_id */
        do
        {
            val = host_services.profile_get_next_value(profile_id, &k, &v);
            BRCM_SAI_LOG_SWITCH(SAI_LOG_DEBUG,
                                "Retreiving KVP [%s:%s]\n", k, v);
        } while (val != -1);
    }
    BRCM_SAI_FUNCTION_EXIT(SAI_API_SWITCH);

    return SAI_STATUS_SUCCESS;
}

/*
* Routine Description:
*   Disconnect this SAI library from the SDK.
*
* Arguments:
*   None
* Return Values:
*   None
*/
STATIC void
brcm_sai_disconnect_switch(void)
{
    BRCM_SAI_FUNCTION_ENTER(SAI_API_SWITCH);

    memset(&host_callbacks, 0, sizeof(sai_switch_notification_t));

    BRCM_SAI_FUNCTION_EXIT(SAI_API_SWITCH);
}

/*
* Routine Description:
*    Set switch attribute value
*
* Arguments:
*    [in] attr - switch attribute
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_set_switch_attribute(_In_ const sai_attribute_t *attr)
{
#define _SET_SWITCH "Set switch"
    int val;
    sai_status_t rv = SAI_STATUS_SUCCESS;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_SWITCH);
    BRCM_SAI_SWITCH_INIT_CHECK;

    if (NULL == attr)
    {
        return SAI_STATUS_INVALID_PARAMETER;
    }
    switch(attr->id)
    {
        case SAI_SWITCH_ATTR_MAX_VIRTUAL_ROUTERS:
        case SAI_SWITCH_ATTR_FDB_TABLE_SIZE:
        case SAI_SWITCH_ATTR_ON_LINK_ROUTE_SUPPORTED:
        case SAI_SWITCH_ATTR_OPER_STATUS:
        case SAI_SWITCH_ATTR_MAX_TEMP:
        case SAI_SWITCH_ATTR_SWITCHING_MODE:
            rv = SAI_STATUS_NOT_SUPPORTED;
            break;
        case SAI_SWITCH_ATTR_BCAST_CPU_FLOOD_ENABLE:
        case SAI_SWITCH_ATTR_MCAST_CPU_FLOOD_ENABLE:
            rv = SAI_STATUS_NOT_SUPPORTED;
            break;
        case SAI_SWITCH_ATTR_SRC_MAC_ADDRESS:
            rv = _brcm_sai_switch_system_mac_set(attr->value.mac);
            break;
        case SAI_SWITCH_ATTR_MAX_LEARNED_ADDRESSES:
            rv = SAI_STATUS_NOT_SUPPORTED;
            break;
        case SAI_SWITCH_ATTR_FDB_AGING_TIME:
            rv = opennsl_l2_age_timer_set(0, attr->value.u32);
            BRCM_SAI_ATTR_API_CHK(SAI_API_SWITCH, _SET_SWITCH, rv, attr->id);
            break;
        case SAI_SWITCH_ATTR_FDB_UNICAST_MISS_ACTION:
        case SAI_SWITCH_ATTR_FDB_BROADCAST_MISS_ACTION:
        case SAI_SWITCH_ATTR_FDB_MULTICAST_MISS_ACTION:
            rv = SAI_STATUS_NOT_SUPPORTED;
            break;
        case SAI_SWITCH_ATTR_ECMP_HASH_SEED:
            rv = opennsl_switch_control_set(0, opennslSwitchHashSeed0,
                                            attr->value.u32);
            BRCM_SAI_ATTR_API_CHK(SAI_API_SWITCH, _SET_SWITCH, rv, attr->id);
            break;

        case SAI_SWITCH_ATTR_ECMP_HASH_ALGO:
            if (SAI_HASH_RANDOM == attr->value.u32)
            {
                rv = SAI_STATUS_NOT_SUPPORTED;
            }
            else
            {
                val = (SAI_HASH_CRC == attr->value.u32) ? 8 : 1;
                rv = opennsl_switch_control_set(0, opennslSwitchHashField0Config,
                                                val);
                BRCM_SAI_ATTR_API_CHK(SAI_API_SWITCH, _SET_SWITCH, rv, attr->id);
            }
            break;
        case SAI_SWITCH_ATTR_ECMP_HASH_FIELDS:
        case SAI_SWITCH_ATTR_ECMP_MAX_PATHS:
        case SAI_SWITCH_ATTR_DEFAULT_TRAP_CHANNEL:
        case SAI_SWITCH_ATTR_DEFAULT_TRAP_CHANNEL_FD:
        case SAI_SWITCH_ATTR_DEFAULT_TRAP_GROUP:
        case SAI_SWITCH_ATTR_QOS_DEFAULT_TC:
        case SAI_SWITCH_ATTR_QOS_DOT1P_TO_TC_MAP:
        case SAI_SWITCH_ATTR_QOS_DOT1P_TO_COLOR_MAP:
        case SAI_SWITCH_ATTR_QOS_DSCP_TO_TC_MAP:
        case SAI_SWITCH_ATTR_QOS_DSCP_TO_COLOR_MAP:
        case SAI_SWITCH_ATTR_QOS_TC_TO_QUEUE_MAP:
        case SAI_SWITCH_ATTR_QOS_TC_TO_DOT1P_MAP:
        case SAI_SWITCH_ATTR_QOS_TC_TO_DSCP_MAP:
        case SAI_SWITCH_ATTR_PORT_BREAKOUT:
            rv = SAI_STATUS_NOT_SUPPORTED;
            break;
        case SAI_SWITCH_ATTR_COUNTER_REFRESH_INTERVAL:
            rv = SAI_STATUS_ATTR_NOT_IMPLEMENTED_0;
            break;
        case SAI_SWITCH_ATTR_CPU_PORT:
            rv = SAI_STATUS_NOT_SUPPORTED;
            break;
        default:
            BRCM_SAI_LOG_SWITCH(SAI_LOG_ERROR,
                                "Unknown switch attribute %d passed\n",
                                attr->id);
            rv = SAI_STATUS_INVALID_PARAMETER;;
    }

    BRCM_SAI_FUNCTION_EXIT(SAI_API_SWITCH);
    return rv;
#undef _SET_SWITCH
}

/*
* Routine Description:
*    Get switch attribute value
*
* Arguments:
*    [in] attr_count - number of switch attributes
*    [inout] attr_list - array of switch attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_get_switch_attribute(_In_ uint32_t attr_count,
                              _Inout_ sai_attribute_t *attr_list)
{
  return _brcm_sai_get_switch_attribute(attr_count, attr_list);
}

/*
################################################################################
#                                Internal functions                            #
################################################################################
*/

/*
################################################################################
#                                Functions map                                 #
################################################################################
*/
const sai_switch_api_t switch_apis = {
    brcm_sai_initialize_switch,
    brcm_sai_shutdown_switch,
    brcm_sai_connect_switch,
    brcm_sai_disconnect_switch,
    brcm_sai_set_switch_attribute,
    brcm_sai_get_switch_attribute,
};
