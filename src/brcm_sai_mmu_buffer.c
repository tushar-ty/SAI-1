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
#include <saibuffer.h>
#include <brcm_sai_common.h>

/*
################################################################################
#                              MMU buffer functions                            #
################################################################################
*/

/*
*Routine Description :
*   Set ingress priority group attribute
* Arguments :
*   [in] ingress_pg_id – ingress priority group id
*   [in] attr - attribute
*
* Return Values :
*   SAI_STATUS_SUCCESS on success
*   Failure status code on error
*/
STATIC sai_status_t
brcm_sai_set_ingress_priority_group_attr(_In_ sai_object_id_t ingress_pg_id,
                                         _In_ const sai_attribute_t *attr)
{
    return _brcm_sai_set_ingress_priority_group_attr(ingress_pg_id, attr);
}

/*
* Routine Description:
*    Get ingress priority group attributes
* Arguments:
*   [in] ingress_pg_id – ingress priority group id
*   [in] attr_count - number of attributes
*   [inout] attr_list - array of attributes
*
* Return Values:
*   SAI_STATUS_SUCCESS on success
*   Failure status code on error
*/
STATIC sai_status_t
brcm_sai_get_ingress_priority_group_attr(_In_ sai_object_id_t ingress_pg_id,
                                         _In_ uint32_t attr_count,
                                         _Inout_ sai_attribute_t *attr_list)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_BUFFERS);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_BUFFERS);
    return rv;
}

/*
* Routine Description:
*    Create buffer pool
*
* Arguments:
*   [out] pool_id  -pool id
*   [in] attr_count - number of attributes
*   [in] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_create_buffer_pool(_Out_ sai_object_id_t* pool_id,
                            _In_ uint32_t attr_count,
                            _In_ const sai_attribute_t *attr_list)
{
    return _brcm_sai_create_buffer_pool(pool_id,
                                        attr_count,
                                        attr_list);
}

/*
* Routine Description:
*    Remove buffer pool
*
* Arguments:
*    [in] pool_id  -pool id
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_remove_buffer_pool(_In_ sai_object_id_t pool_id)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_BUFFERS);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_BUFFERS);
    return rv;
}

/*
* Routine Description:
*    Set buffer pool attribute
* Arguments:
*   [in] pool_id – pool id
*   [in] attr - attribute
*
* Return Values:
*   SAI_STATUS_SUCCESS on success
*   Failure status code on error
*/
STATIC sai_status_t
brcm_sai_set_buffer_pool_attr(_In_ sai_object_id_t pool_id,
                              _In_ const sai_attribute_t *attr)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_BUFFERS);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_BUFFERS);
    return rv;
}

/*
* Routine Description:
*    Get buffer pool attributes
* Arguments:
*   [in] pool_id – pool id
*   [in] attr_count - number of attributes
*   [inout] attr_list - array of attributes
*
* Return Values:
*   SAI_STATUS_SUCCESS on success
*   Failure status code on error
*/
STATIC sai_status_t
brcm_sai_get_buffer_pool_attr(_In_ sai_object_id_t pool_id,
                              _In_ uint32_t attr_count,
                              _Inout_ sai_attribute_t *attr_list)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_BUFFERS);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_BUFFERS);
    return rv;
}

/*
* Routine Description:
*    Create buffer profile
*
* Arguments:
*   [Out] buffer_profile_id  - buffer profile id
*   [in] attr_count - number of attributes
*   [in] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/

STATIC sai_status_t
brcm_sai_create_buffer_profile(_Out_ sai_object_id_t* buffer_profile_id,
                               _In_ uint32_t attr_count,
                               _In_ const sai_attribute_t *attr_list)
{
    return _brcm_sai_create_buffer_profile(buffer_profile_id,
                                           attr_count,
                                           attr_list);
}

/*
* Routine Description:
*    Remove buffer profile
*
* Arguments:
*   [in] buffer_profile_id  - buffer profile id
*
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/

STATIC sai_status_t
brcm_sai_remove_buffer_profile(_In_ sai_object_id_t buffer_profile_id)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_BUFFERS);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_BUFFERS);
    return rv;
}

/*
* Routine Description:
*    Set buffer profile attribute
* Arguments:
*   [in] buffer_profile_id  - buffer profile id
*   [in] attr -  buffer profile attribute
*
* Return Values:
* SAI_STATUS_SUCCESS on success
* Failure status code on error
*/
STATIC sai_status_t
brcm_sai_set_buffer_profile_attr(_In_ sai_object_id_t buffer_profile_id,
                                 _In_ const sai_attribute_t *attr)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_BUFFERS);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_BUFFERS);
    return rv;
}

/*
* Routine Description:
*     Get buffer profile attributes
* Arguments:
*   [in] buffer_profile_id  - buffer profile id
*   [in] attr_count - number of attributes
*   [inout] attr_list - array of attributes
*
* Return Values:
*   SAI_STATUS_SUCCESS on success
*   Failure status code on error
*/
STATIC sai_status_t
brcm_sai_get_buffer_profile_attr(_In_ sai_object_id_t buffer_profile_id,
                                 _In_ uint32_t attr_count,
                                 _Inout_ sai_attribute_t *attr_list)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_BUFFERS);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_BUFFERS);
    return rv;
}

/*
################################################################################
#                                  Functions map                               #
################################################################################
*/
const sai_buffer_api_t buffer_apis = {
    brcm_sai_create_buffer_pool,
    brcm_sai_remove_buffer_pool,
    brcm_sai_set_buffer_pool_attr,
    brcm_sai_get_buffer_pool_attr,
    brcm_sai_set_ingress_priority_group_attr,
    brcm_sai_get_ingress_priority_group_attr,
    brcm_sai_create_buffer_profile,
    brcm_sai_remove_buffer_profile,
    brcm_sai_set_buffer_profile_attr,
    brcm_sai_get_buffer_profile_attr,
};
