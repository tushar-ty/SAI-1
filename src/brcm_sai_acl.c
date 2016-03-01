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
#                                  ACL functions                               #
################################################################################
*/

/*
* Routine Description:
*   Create an ACL table
*
* Arguments:
 *  [out] acl_table_id - the the acl table id
 *  [in] attr_count - number of attributes
 *  [in] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_create_acl_table(_Out_ sai_object_id_t* acl_table_id,
                          _In_ uint32_t attr_count,
                          _In_ const sai_attribute_t *attr_list)
{
    return _brcm_sai_create_acl_table(acl_table_id,
                                      attr_count,
                                      attr_list);
}

/*
* Routine Description:
*   Delete an ACL table
*
* Arguments:
*   [in] acl_table_id - the acl table id
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_delete_acl_table(_In_ sai_object_id_t acl_table_id)
{
    return _brcm_sai_delete_acl_table(acl_table_id);
}

/*
* Routine Description:
*   Set ACL table attribute
*
* Arguments:
*    [in] acl_table_id - the acl table id
*    [in] attr - attribute
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_set_acl_table_attribute(_In_ sai_object_id_t acl_table_id,
                                 _In_ const sai_attribute_t *attr)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_ACL);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_ACL);
    return rv;
}

/*
* Routine Description:
*   Get ACL table attribute
*
* Arguments:
*    [in] acl_table_id - acl table id
*    [in] attr_count - number of attributes
*    [Out] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_get_acl_table_attribute(_In_ sai_object_id_t acl_table_id,
                                 _In_ uint32_t attr_count,
                                 _Out_ sai_attribute_t *attr_list)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_ACL);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_ACL);
    return rv;
}

/*
* Routine Description:
*   Create an ACL entry
*
* Arguments:
*   [out] acl_entry_id - the acl entry id
*   [in] attr_count - number of attributes
*   [in] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_create_acl_entry(_Out_ sai_object_id_t *acl_entry_id,
                          _In_ uint32_t attr_count,
                          _In_ const sai_attribute_t *attr_list)
{
    return _brcm_sai_create_acl_entry(acl_entry_id,
                                      attr_count,
                                      attr_list);
}

/*
* Routine Description:
*   Delete an ACL entry
*
* Arguments:
 *  [in] acl_entry_id - the acl entry id
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_delete_acl_entry(_In_ sai_object_id_t acl_entry_id)
{
    return _brcm_sai_delete_acl_entry(acl_entry_id);
}

/*
* Routine Description:
*   Set ACL entry attribute
*
* Arguments:
*    [in] acl_entry_id - the acl entry id
*    [in] attr - attribute
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_set_acl_entry_attribute(_In_ sai_object_id_t acl_entry_id,
                                 _In_ const sai_attribute_t *attr)
{
    return _brcm_sai_set_acl_entry_attribute(acl_entry_id, attr);
}

/*
* Routine Description:
*   Get ACL entry attribute
*
* Arguments:
*    [in] acl_entry_id - acl entry id
*    [in] attr_count - number of attributes
*    [Out] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_get_acl_entry_attribute(_In_ sai_object_id_t acl_entry_id,
                                 _In_ uint32_t attr_count,
                                 _Out_ sai_attribute_t *attr_list)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_ACL);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_ACL);
    return rv;
}

/*
* Routine Description:
*   Create an ACL counter
*
* Arguments:
*   [out] acl_counter_id - the acl counter id
*   [in] attr_count - number of attributes
*   [in] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_create_acl_counter(_Out_ sai_object_id_t *acl_counter_id,
                            _In_ uint32_t attr_count,
                            _In_ const sai_attribute_t *attr_list)
{
    return _brcm_sai_create_acl_counter(acl_counter_id,
                                        attr_count,
                                        attr_list);
}

/*
* Routine Description:
*   Delete an ACL counter
*
* Arguments:
 *  [in] acl_counter_id - the acl counter id
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_delete_acl_counter(_In_ sai_object_id_t acl_counter_id)
{
    return _brcm_sai_delete_acl_counter(acl_counter_id);
}

/*
* Routine Description:
*   Set ACL counter attribute
*
* Arguments:
*    [in] acl_counter_id - the acl counter id
*    [in] attr - attribute
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_set_acl_counter_attribute(_In_ sai_object_id_t acl_counter_id,
                                   _In_ const sai_attribute_t *attr)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_ACL);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_ACL);
    return rv;
}

/*
* Routine Description:
*   Get ACL counter attribute
*
* Arguments:
*    [in] acl_counter_id - acl counter id
*    [in] attr_count - number of attributes
*    [Out] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_get_acl_counter_attribute(_In_ sai_object_id_t acl_counter_id,
                                   _In_ uint32_t attr_count,
                                   _Out_ sai_attribute_t *attr_list)
{
    return _brcm_sai_get_acl_counter_attribute(acl_counter_id,
                                               attr_count,
                                               attr_list);
}

/*
################################################################################
#                                Functions map                                 #
################################################################################
*/
const sai_acl_api_t acl_apis = {
    brcm_sai_create_acl_table,
    brcm_sai_delete_acl_table,
    brcm_sai_set_acl_table_attribute,
    brcm_sai_get_acl_table_attribute,
    brcm_sai_create_acl_entry,
    brcm_sai_delete_acl_entry,
    brcm_sai_set_acl_entry_attribute,
    brcm_sai_get_acl_entry_attribute,
    brcm_sai_create_acl_counter,
    brcm_sai_delete_acl_counter,
    brcm_sai_set_acl_counter_attribute,
    brcm_sai_get_acl_counter_attribute
};
