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
/*
################################################################################
#                             QOS MAPS functions                               #
################################################################################
*/

/**
 * @brief Create Qos Map
 *
 * @param[out] qos_map_id Qos Map Id
 * @param[in] attr_count number of attributes
 * @param[in] attr_list array of attributes
 *
 * @return  SAI_STATUS_SUCCESS on success
 *          Failure status code on error
 */
sai_status_t
brcm_sai_create_qos_map(_Out_ sai_object_id_t* qos_map_id,
                        _In_ uint32_t attr_count,
                        _In_ const sai_attribute_t *attr_list)
{
    return _brcm_sai_create_qos_map(qos_map_id,
                                    attr_count,
                                    attr_list);
}

/**
 * @brief Remove Qos Map
 *
 *  @param[in] qos_map_id Qos Map id to be removed.
 *
 *  @return  SAI_STATUS_SUCCESS on success
 *           Failure status code on error
 */
sai_status_t
brcm_sai_remove_qos_map(_In_  sai_object_id_t qos_map_id)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_QOS_MAPS);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_QOS_MAPS);
    return rv;
}

/**
 * @brief Set attributes for qos map
 *
 * @param[in] qos_map_id Qos Map Id
 * @param[in] attr attribute to set
 *
 * @return  SAI_STATUS_SUCCESS on success
 *          Failure status code on error
 */

sai_status_t
brcm_sai_set_qos_map_attribute(_In_ sai_object_id_t qos_map_id,
                               _In_ const sai_attribute_t *attr)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_QOS_MAPS);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_QOS_MAPS);
    return rv;
}

/**
 * @brief  Get attrbutes of qos map
 *
 * @param[in] qos_map_id  map id
 * @param[in] attr_count  number of attributes
 * @param[inout] attr_list  array of attributes
 *
 * @return SAI_STATUS_SUCCESS on success
 *        Failure status code on error
 */
sai_status_t
brcm_sai_get_qos_map_attribute(_In_ sai_object_id_t   qos_map_id,
                               _In_ uint32_t attr_count,
                               _Inout_ sai_attribute_t *attr_list)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_QOS_MAPS);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_QOS_MAPS);
    return rv;
}

/*
################################################################################
#                                Functions map                                 #
################################################################################
*/
const sai_qos_map_api_t qos_map_apis = {
    brcm_sai_create_qos_map,
    brcm_sai_remove_qos_map,
    brcm_sai_set_qos_map_attribute,
    brcm_sai_get_qos_map_attribute
};
