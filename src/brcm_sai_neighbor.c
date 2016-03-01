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
*    Create neighbor entry
*
* Arguments:
*    [in] neighbor_entry - neighbor entry
*    [in] attr_count - number of attributes
*    [in] attrs - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_create_neighbor_entry(_In_ const sai_neighbor_entry_t* neighbor_entry,
                               _In_ uint32_t attr_count,
                               _In_ const sai_attribute_t *attr_list)
{
    return _brcm_sai_create_neighbor_entry(neighbor_entry,
                                           attr_count,
                                           attr_list);
}

/*
* Routine Description:
*    Remove neighbor entry
*
* Arguments:
*    [in] neighbor_entry - neighbor entry
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_remove_neighbor_entry(_In_ const sai_neighbor_entry_t* neighbor_entry)
{
    return _brcm_sai_remove_neighbor_entry(neighbor_entry);
}

/*
* Routine Description:
*    Set neighbor attribute value
*
* Arguments:
*    [in] neighbor_entry - neighbor entry
*    [in] attr - attribute
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_set_neighbor_attribute(_In_ const sai_neighbor_entry_t* neighbor_entry,
                                _In_ const sai_attribute_t *attr)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_NEIGHBOR);

    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_NEIGHBOR);

    return rv;
}

/*
* Routine Description:
*    Get neighbor attribute value
*
* Arguments:
*    [in] neighbor_entry - neighbor entry
*    [in] attr_count - number of attributes
*    [inout] attrs - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_get_neighbor_attribute(_In_ const sai_neighbor_entry_t* neighbor_entry,
                                _In_ uint32_t attr_count,
                                _Inout_ sai_attribute_t *attr_list)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_NEIGHBOR);

    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_NEIGHBOR);

    return rv;
}

/*
* Routine Description:
*    Remove all neighbor entries
*
* Arguments:
*    None
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_remove_all_neighbor_entries(void)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_NEIGHBOR);

    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_NEIGHBOR);

    return rv;
}

/*
################################################################################
#                                Functions map                                 #
################################################################################
*/
const sai_neighbor_api_t neighbor_apis = {
    brcm_sai_create_neighbor_entry,
    brcm_sai_remove_neighbor_entry,
    brcm_sai_set_neighbor_attribute,
    brcm_sai_get_neighbor_attribute,
    brcm_sai_remove_all_neighbor_entries
};
