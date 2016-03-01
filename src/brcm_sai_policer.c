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
#                              Policer functions                               #
################################################################################
*/

/*
* Routine Description:
*   Create Policer
*
* Arguments:
*   [out] policer_id - the policer id
*   [in] attr_count - number of attributes
*   [in] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_create_policer(_Out_ sai_object_id_t *policer_id,
                        _In_ uint32_t attr_count,
                        _In_ const sai_attribute_t *attr_list)
{
    return _brcm_sai_create_policer(policer_id,
                                    attr_count,
                                    attr_list);
}

/*
* Routine Description:
*   Delete policer
*
* Arguments:
 *  [in] policer_id - Policer id
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_remove_policer(_In_ sai_object_id_t policer_id)
{
    return _brcm_sai_remove_policer(policer_id);
}

/*
* Routine Description:
*   Set Policer attribute
*
* Arguments:
*    [in] policer_id - Policer id
*    [in] attr - attribute
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_set_policer_attribute(_In_ sai_object_id_t policer_id,
                               _In_ const sai_attribute_t *attr)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_POLICER);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_POLICER);
    return rv;
}

/*
* Routine Description:
*   Get Policer attribute
*
* Arguments:
*    [in] policer_id - policer id
*    [in] attr_count - number of attributes
*    [Out] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_get_policer_attribute(_In_ sai_object_id_t policer_id,
                               _In_ uint32_t attr_count,
                               _Out_ sai_attribute_t *attr_list)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_POLICER);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_POLICER);
    return rv;
}

/**
 * @brief  Get Policer Statistics
 *
 * @param[in] policer_id - policer id
 * @param[in] counter_ids - array of counter ids
 * @param[in] number_of_counters - number of counters in the array
 * @param[out] counters - array of resulting counter values.
 *
 * @return SAI_STATUS_SUCCESS on success
 *         Failure status code on error
 */
STATIC sai_status_t
brcm_sai_get_policer_stats(_In_ sai_object_id_t policer_id,
                           _In_ const sai_policer_stat_counter_t *counter_ids,
                           _In_ uint32_t number_of_counters,
                           _Out_ uint64_t* counters)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_POLICER);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_POLICER);
    return rv;
}

/*
################################################################################
#                                Functions map                                 #
################################################################################
*/
const sai_policer_api_t policer_apis = {
    brcm_sai_create_policer,
    brcm_sai_remove_policer,
    brcm_sai_set_policer_attribute,
    brcm_sai_get_policer_attribute,
    brcm_sai_get_policer_stats
};
