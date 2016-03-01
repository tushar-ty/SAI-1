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
#                             QOS Queue functions                              #
################################################################################
*/

/**
 * @brief Set attribute to Queue
 * @param[in] queue_id queue id to set the attribute
 * @param[in] attr attribute to set
 *
 * @return  SAI_STATUS_SUCCESS on success
 *           Failure status code on error
 */
sai_status_t
brcm_sai_set_queue_attribute(_In_ sai_object_id_t queue_id,
                             _In_ const sai_attribute_t *attr)
{
    return _brcm_sai_set_queue_attribute(queue_id, attr);
}

/**
 * @brief Get attribute to Queue
 * @param[in] queue_id queue id to set the attribute
 * @param[in] attr_count number of attributes
 * @param[inout] attr_list Array of attributes
 *
 * @return  SAI_STATUS_SUCCESS on success
 *           Failure status code on error
 */
sai_status_t
brcm_sai_get_queue_attribute(_In_ sai_object_id_t queue_id,
                             _In_ uint32_t        attr_count,
                             _Inout_ sai_attribute_t *attr_list)
{
    int i, type;
    sai_status_t rv = SAI_STATUS_SUCCESS;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_QUEUE);
    BRCM_SAI_SWITCH_INIT_CHECK;
    BRCM_SAI_GET_ATTRIB_PARAM_CHK;

    for (i = 0; i<attr_count; i++)
    {
        switch(attr_list[i].id)
        {
            case SAI_QUEUE_ATTR_TYPE:
                type = BRCM_SAI_GET_OBJ_SUB_TYPE(queue_id);
                switch (type)
                {
                    case SAI_QUEUE_TYPE_ALL:
                    case SAI_QUEUE_TYPE_UNICAST:
                    case SAI_QUEUE_TYPE_MULTICAST:
                        attr_list[i].value.u32 = type;
                        break;
                    default: rv = SAI_STATUS_INVALID_PARAMETER;
                        break;
                }
                break;
            case SAI_QUEUE_ATTR_WRED_PROFILE_ID:
            case SAI_QUEUE_ATTR_BUFFER_PROFILE_ID:
            case SAI_QUEUE_ATTR_SCHEDULER_PROFILE_ID:
                rv = SAI_STATUS_ATTR_NOT_IMPLEMENTED_0;
                break;
            default:
                BRCM_SAI_LOG_QUEUE(SAI_LOG_ERROR,
                                   "Unknown qos queue attribute %d passed\n",
                                   attr_list[i].id);
                break;
        }
        if (SAI_STATUS_SUCCESS != rv)
        {
            BRCM_SAI_LOG_QUEUE(SAI_LOG_INFO,
                               "Error processsing queue attributes\n");
            return rv;
        }
    }
    BRCM_SAI_FUNCTION_EXIT(SAI_API_QUEUE);
    return rv;
}

/**
 * @brief   Get queue statistics counters.
 *
 * @param[in] queue_id Queue id
 * @param[in] counter_ids specifies the array of counter ids
 * @param[in] number_of_counters number of counters in the array
 * @param[out] counters array of resulting counter values.
 *
 * @return SAI_STATUS_SUCCESS on success
 *         Failure status code on error
 */
sai_status_t
brcm_sai_get_queue_stats(_In_ sai_object_id_t queue_id,
                         _In_ const sai_queue_stat_counter_t *counter_ids,
                         _In_ uint32_t number_of_counters,
                         _Out_ uint64_t* counters)
{
    return _brcm_sai_get_queue_stats(queue_id,
                                     counter_ids,
                                     number_of_counters,
                                     counters);
}

/*
################################################################################
#                                Functions map                                 #
################################################################################
*/
const sai_queue_api_t qos_apis = {
    brcm_sai_set_queue_attribute,
    brcm_sai_get_queue_attribute,
    brcm_sai_get_queue_stats
};
