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
#                                  Scheduler functions                         #
################################################################################
*/

/**
 * @brief  Create Scheduler Profile
 *
 * @param[out] scheduler_id Scheduler id
 * @param[in] attr_count number of attributes
 * @param[in] attr_list array of attributes
 *
 * @return  SAI_STATUS_SUCCESS on success
 *          Failure status code on error
 */
STATIC sai_status_t
brcm_sai_create_scheduler_profile(_Out_ sai_object_id_t *scheduler_id,
                                  _In_ uint32_t attr_count,
                                  _In_ const sai_attribute_t *attr_list)
{
    return _brcm_sai_create_scheduler_profile(scheduler_id,
                                              attr_count,
                                              attr_list);
}

/**
 * @brief  Remove Scheduler profile
 *
 * @param[in] scheduler_id Scheduler id
 *
 * @return  SAI_STATUS_SUCCESS on success
 *          Failure status code on error
 */
STATIC sai_status_t
brcm_sai_remove_scheduler_profile(_In_ sai_object_id_t scheduler_id)
{
  return _brcm_sai_remove_scheduler_profile(scheduler_id,
                                            port_apis.set_port_attribute,
                                            qos_apis.set_queue_attribute);
}

/**
 * @brief  Set Scheduler Attribute
 *
 * @param[in] scheduler_id Scheduler id
 * @param[in] attr attribute to set
 *
 * @return  SAI_STATUS_SUCCESS on success
 *          Failure status code on error
 */
STATIC sai_status_t
brcm_sai_set_scheduler_attribute(_In_ sai_object_id_t scheduler_id,
                                 _In_ const sai_attribute_t *attr)
{
    sai_status_t rv = SAI_STATUS_SUCCESS;
    _brcm_sai_qos_scheduler_t *scheduler = NULL;
    _brcm_sai_scheduler_object_t *current_object;
    sai_object_id_t object_type;
    sai_attribute_t sai_attr;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_SCHEDULER);
    BRCM_SAI_SWITCH_INIT_CHECK;

    scheduler = _brcm_sai_scheduler_get(BRCM_SAI_GET_OBJ_VAL(int, scheduler_id));
    if (NULL == scheduler)
    {
        BRCM_SAI_LOG_SCHEDULER(SAI_LOG_ERROR,
                               "Invalid scheduler id passed.\n");
        return SAI_STATUS_INVALID_PARAMETER;
    }

    switch (attr->id)
    {
        case SAI_SCHEDULER_ATTR_SCHEDULING_ALGORITHM:
                scheduler->algorithm = attr->value.s32;
            break;
        case SAI_SCHEDULER_ATTR_SCHEDULING_WEIGHT:
            if ((attr->value.s32 < _BRCM_SAI_SCHEDULER_WEIGHT_MIN) ||
                (attr->value.s32 > _BRCM_SAI_SCHEDULER_WEIGHT_MAX))
            {
                BRCM_SAI_LOG_SCHEDULER(SAI_LOG_ERROR,
                                       "Scheduler weight %d outside of valid range 1 - 100.\n",
                                       attr->value.s32);
                return SAI_STATUS_INVALID_PARAMETER;
            }
            scheduler->weight = attr->value.s32;
            break;
        case SAI_SCHEDULER_ATTR_MIN_BANDWIDTH_RATE:
            if (SAI_STATUS_FAILURE ==
                    _brcm_sai_32bit_size_check(scheduler->shaper_type,
                                               attr->value.u64))
            {
                BRCM_SAI_LOG_SCHEDULER(SAI_LOG_ERROR,
                                       "Scheduler minimum bandwidth %lu > max allowed.\n",
                                       attr->value.u64);
                return SAI_STATUS_INVALID_PARAMETER;
            }
            scheduler->minimum_bandwidth = (SAI_METER_TYPE_PACKETS == scheduler->shaper_type) ?
                attr->value.u64 : attr->value.u64 * 8 / 1000;
            break;
        case SAI_SCHEDULER_ATTR_MIN_BANDWIDTH_BURST_RATE:
            if (SAI_STATUS_FAILURE ==
                    _brcm_sai_32bit_size_check(scheduler->shaper_type,
                                               attr->value.u64))
            {
                BRCM_SAI_LOG_SCHEDULER(SAI_LOG_ERROR,
                                       "Scheduler minimum burst %lu > max allowed.\n",
                                       attr->value.u64);
                return SAI_STATUS_INVALID_PARAMETER;
            }
            scheduler->minimum_burst = (SAI_METER_TYPE_PACKETS == scheduler->shaper_type) ?
                attr->value.u64 : attr->value.u64 * 8 / 1000;
            break;
        case SAI_SCHEDULER_ATTR_MAX_BANDWIDTH_RATE:
            if (SAI_STATUS_FAILURE ==
                    _brcm_sai_32bit_size_check(scheduler->shaper_type,
                                               attr->value.u64))
            {
                BRCM_SAI_LOG_SCHEDULER(SAI_LOG_ERROR,
                                       "Scheduler maximum bandwidth %lu > max allowed.\n",
                                       attr->value.u64);
                return SAI_STATUS_INVALID_PARAMETER;
            }
            scheduler->maximum_bandwidth = (SAI_METER_TYPE_PACKETS == scheduler->shaper_type) ?
                attr->value.u64 : attr->value.u64 * 8 / 1000;
            break;
        case SAI_SCHEDULER_ATTR_MAX_BANDWIDTH_BURST_RATE:
            if (SAI_STATUS_FAILURE ==
                    _brcm_sai_32bit_size_check(scheduler->shaper_type,
                                               attr->value.u64))
            {
                BRCM_SAI_LOG_SCHEDULER(SAI_LOG_ERROR,
                                       "Scheduler maximum burst %lu > max allowed.\n",
                                       attr->value.u64);
                return SAI_STATUS_INVALID_PARAMETER;
            }
            scheduler->maximum_burst = (SAI_METER_TYPE_PACKETS == scheduler->shaper_type) ?
                attr->value.u64 : attr->value.u64 * 8 / 1000;
            break;
        case SAI_SCHEDULER_ATTR_SHAPER_TYPE:
            scheduler->shaper_type = attr->value.s32;
            break;
        default:
            break;
    }

    current_object = scheduler->object_list;
    sai_attr.value.oid = scheduler_id;
    while (NULL != current_object)
    {
        object_type = BRCM_SAI_GET_OBJ_TYPE(current_object->object_id);
        if (SAI_OBJECT_TYPE_PORT == object_type)
        {
            sai_attr.id = SAI_PORT_ATTR_QOS_SCHEDULER_PROFILE_ID;
            rv = port_apis.set_port_attribute(current_object->object_id, &sai_attr);
            if (SAI_STATUS_SUCCESS != rv)
            {
                BRCM_SAI_LOG_SCHEDULER(SAI_LOG_ERROR,
                                       "Unable to set port attribute %lu for port id %lu, "
                                       "scheduler id %lu\n", attr->value.u64,
                                       current_object->object_id,
                                       scheduler_id);
                return rv;
            }
        }
        else if (SAI_OBJECT_TYPE_QUEUE == object_type)
        {
            sai_attr.id = SAI_QUEUE_ATTR_SCHEDULER_PROFILE_ID;
            rv = qos_apis.set_queue_attribute(current_object->object_id, &sai_attr);
            if (SAI_STATUS_SUCCESS != rv)
            {
                BRCM_SAI_LOG_SCHEDULER(SAI_LOG_ERROR,
                                       "Unable to set queue attribute %lu for queue id %lu, "
                                       "scheduler id %lu\n", attr->value.u64,
                                       current_object->object_id,
                                       scheduler_id);
                return rv;
            }
        }
        else
        {
            return SAI_STATUS_INVALID_OBJECT_TYPE;
        }
        current_object = current_object->next;
    }

    BRCM_SAI_FUNCTION_EXIT(SAI_API_SCHEDULER);
    return rv;
}

/**
 * @brief  Get Scheduler attribute
 *
 * @param[in] scheduler_id - scheduler id
 * @param[in] attr_count - number of attributes
 * @param[inout] attr_list - array of attributes
 *
 * @return SAI_STATUS_SUCCESS on success
 *        Failure status code on error
 */
STATIC sai_status_t
brcm_sai_get_scheduler_attribute(_In_ sai_object_id_t scheduler_id,
                                 _In_ uint32_t attr_count,
                                 _Inout_ sai_attribute_t *attr_list)
{
    int i;
    sai_status_t rv = SAI_STATUS_SUCCESS;
    _brcm_sai_qos_scheduler_t *scheduler = NULL;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_SCHEDULER);
    BRCM_SAI_SWITCH_INIT_CHECK;

    scheduler = _brcm_sai_scheduler_get(BRCM_SAI_GET_OBJ_VAL(int, scheduler_id));
    if (NULL == scheduler)
    {
        BRCM_SAI_LOG_SCHEDULER(SAI_LOG_ERROR,
                               "Invalid scheduler id passed.\n");
        return SAI_STATUS_INVALID_PARAMETER;
    }

    for (i = 0; i < attr_count; i++)
    {
        switch (attr_list[i].id)
        {
            case SAI_SCHEDULER_ATTR_SCHEDULING_ALGORITHM:
                 attr_list[i].value.s32 = scheduler->algorithm;
                break;
            case SAI_SCHEDULER_ATTR_SCHEDULING_WEIGHT:
                attr_list[i].value.s32 = scheduler->weight;
                break;
            case SAI_SCHEDULER_ATTR_MIN_BANDWIDTH_RATE:
                attr_list[i].value.u64 = scheduler->minimum_bandwidth;
                break;
            case SAI_SCHEDULER_ATTR_MIN_BANDWIDTH_BURST_RATE:
                attr_list[i].value.u64 = scheduler->minimum_burst;
                break;
            case SAI_SCHEDULER_ATTR_MAX_BANDWIDTH_RATE:
                attr_list[i].value.u64 = scheduler->maximum_bandwidth;
                break;
            case SAI_SCHEDULER_ATTR_MAX_BANDWIDTH_BURST_RATE:
                attr_list[i].value.u64 = scheduler->maximum_burst;
                break;
            case SAI_SCHEDULER_ATTR_SHAPER_TYPE:
                attr_list[i].value.s32 = scheduler->shaper_type;
                break;
            default:
                break;
        }
    }

    BRCM_SAI_FUNCTION_EXIT(SAI_API_SCHEDULER);
    return rv;
}

/*
################################################################################
#                                Functions map                                 #
################################################################################
*/
const sai_scheduler_api_t qos_scheduler_apis = {
    brcm_sai_create_scheduler_profile,
    brcm_sai_remove_scheduler_profile,
    brcm_sai_set_scheduler_attribute,
    brcm_sai_get_scheduler_attribute
};
