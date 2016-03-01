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
typedef struct brcm_sai_api_map_s {
    sai_api_t api_id;
    const void *api_table;
} brcm_sai_api_map_t;

/*
################################################################################
#                                     API map                                  #
################################################################################
*/

const brcm_sai_api_map_t sai_api_map[] = {
    { SAI_API_UNSPECIFIED,      NULL },
    { SAI_API_SWITCH,           &switch_apis },
    { SAI_API_PORT,             &port_apis },
    { SAI_API_FDB,              &fdb_apis },
    { SAI_API_VLAN,             &vlan_apis },
    { SAI_API_VIRTUAL_ROUTER,   &router_apis },
    { SAI_API_ROUTE,            &route_apis },
    { SAI_API_NEXT_HOP,         &next_hop_apis },
    { SAI_API_NEXT_HOP_GROUP,   &next_hop_grp_apis },
    { SAI_API_ROUTER_INTERFACE, &router_intf_apis },
    { SAI_API_NEIGHBOR,         &neighbor_apis },
    { SAI_API_ACL,              &acl_apis },
    { SAI_API_HOST_INTERFACE,   &hostif_apis },
    { SAI_API_MIRROR,           NULL },
    { SAI_API_SAMPLEPACKET,     NULL },
    { SAI_API_STP,              NULL },
    { SAI_API_LAG,              NULL },
    { SAI_API_POLICER,          &policer_apis },
    { SAI_API_WRED,             &wred_apis },
    { SAI_API_QOS_MAPS,         &qos_map_apis },
    { SAI_API_QUEUE,            &qos_apis },
    { SAI_API_SCHEDULER,        &qos_scheduler_apis },
    { SAI_API_SCHEDULER_GROUP,  NULL },
    { SAI_API_BUFFERS,          &buffer_apis }
};

/*
################################################################################
#                               Global state                                   #
################################################################################
*/
static bool sai_api_inited = false;
service_method_table_t host_services;

/*
################################################################################
#                              Well known functions                            #
################################################################################
*/

/*
* Routine Description:
*     Adapter module initialization call. This is NOT for SDK initialization.
*
* Arguments:
*     [in] flags - reserved for future use, must be zero
*     [in] services - methods table with services provided by adapter host
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
sai_status_t
sai_api_initialize(_In_ uint64_t flags,
                   _In_ const service_method_table_t* services)
{

    BRCM_SAI_LOG("SAI Enter %s\n", __FUNCTION__);

    if (NULL == services)
    {
        BRCM_SAI_LOG("NULL service_method_table_t* passed to %s\n",
                     __FUNCTION__);
        return SAI_STATUS_INVALID_PARAMETER;
    }
    if ((NULL == services->profile_get_next_value) ||
        (NULL == services->profile_get_value))
    {
        return SAI_STATUS_INVALID_PARAMETER;
    }
    memcpy(&host_services, services, sizeof(service_method_table_t));
    sai_api_inited = true;

    BRCM_SAI_LOG("SAI Exit %s\n", __FUNCTION__);
    return SAI_STATUS_SUCCESS;
}

/*
* Routine Description:
*     Retrieve a pointer to the C-style method table for desired SAI
*     functionality as specified by the given sai_api_id.
*
* Arguments:
*     [in] sai_api_id - SAI api ID
*     [out] api_method_table - Caller allocated method table
*           The table must remain valid until the sai_api_uninitialize() is called
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
sai_status_t
sai_api_query(_In_ sai_api_t sai_api_id,
              _Out_ void** api_method_table)
{
    BRCM_SAI_LOG("SAI Enter %s\n", __FUNCTION__);
    if (!sai_api_inited)
    {
        BRCM_SAI_LOG("API not initialized yet.\n");
        return SAI_STATUS_UNINITIALIZED;
    }
    if (sai_api_id == SAI_API_UNSPECIFIED || sai_api_id > COUNTOF(sai_api_map))
    {
        BRCM_SAI_LOG("Invalid sai_api_t %d passed to %s\n", sai_api_id,
                     __FUNCTION__);
        return SAI_STATUS_INVALID_PARAMETER;
    }
    if (NULL == sai_api_map[sai_api_id].api_table)
    {
        return SAI_STATUS_NOT_IMPLEMENTED;
    }
    *api_method_table = (void*)(sai_api_map[sai_api_id].api_table);

    BRCM_SAI_LOG("SAI Exit %s\n", __FUNCTION__);
    return SAI_STATUS_SUCCESS;
}

/*
* Routine Description:
*   Uninitialization of the adapter module. SAI functionalities, retrieved via
*   sai_api_query() cannot be used after this call.
*
* Arguments:
*   None
*
* Return Values:
*   SAI_STATUS_SUCCESS on success
*   Failure status code on error
*/
sai_status_t
sai_api_uninitialize(void)
{
    BRCM_SAI_LOG("SAI Enter %s\n", __FUNCTION__);

    memset(&host_services, 0, sizeof(service_method_table_t));
    sai_api_inited = false;

    BRCM_SAI_LOG("SAI Exit %s\n", __FUNCTION__);
    return SAI_STATUS_SUCCESS;
}

/*
* Routine Description:
*     Set log level for sai api module. The default log level is SAI_LOG_WARN.
*
* Arguments:
*     [in] sai_api_id - SAI api ID
*     [in] log_level - log level
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
sai_status_t
sai_log_set(_In_ sai_api_t sai_api_id, _In_ sai_log_level_t log_level)
{
    return _sai_log_set(sai_api_id, log_level);
}

/*
* Routine Description:
*     Query sai object type.
*
* Arguments:
*     [in] sai_object_id_t
*
* Return Values:
*    Return SAI_OBJECT_TYPE_NULL when sai_object_id is not valid.
*    Otherwise, return a valid sai object type SAI_OBJECT_TYPE_XXX
*/
sai_object_type_t
sai_object_type_query(_In_ sai_object_id_t sai_object_id)
{
    sai_object_type_t obj;

    BRCM_SAI_LOG("SAI Enter %s\n", __FUNCTION__);
    obj = BRCM_SAI_GET_OBJ_TYPE(sai_object_id);
    if ((SAI_OBJECT_TYPE_NULL >= obj) || (SAI_OBJECT_TYPE_MAX <= obj))
    {
        return SAI_OBJECT_TYPE_NULL;
    }
    BRCM_SAI_LOG("SAI Exit %s\n", __FUNCTION__);
    return obj;
}

/*
################################################################################
#                                Internal functions                            #
################################################################################
*/
bool
_sai_api_is_inited(void)
{
    return sai_api_inited;
}
