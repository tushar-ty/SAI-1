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
#                                Next hop functions                            #
################################################################################
*/

/*
* Routine Description:
*    Create next hop
*
* Arguments:
*    [out] next_hop_id - next hop id
*    [in] attr_count - number of attributes
*    [in] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_create_next_hop(_Out_ sai_object_id_t* next_hop_id,
                         _In_ uint32_t attr_count,
                         _In_ const sai_attribute_t *attr_list)
{
    return _brcm_sai_create_next_hop(next_hop_id,
                                     attr_count,
                                     attr_list);
}

/*
* Routine Description:
*    Remove next hop
*
* Arguments:
*    [in] next_hop_id - next hop id
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_remove_next_hop(_In_ sai_object_id_t next_hop_id)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_NEXT_HOP);
    BRCM_SAI_SWITCH_INIT_CHECK;

    rv = opennsl_l3_egress_destroy(0, BRCM_SAI_GET_OBJ_VAL(opennsl_if_t,
                                                           next_hop_id));
    BRCM_SAI_API_CHK(SAI_API_NEXT_HOP, "L3 egress destroy", rv);

    BRCM_SAI_FUNCTION_EXIT(SAI_API_NEXT_HOP);

    return rv;
}

/*
* Routine Description:
*    Set Next Hop attribute
*
* Arguments:
*    [in] sai_object_id_t - next_hop_id
*    [in] attr - attribute
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_set_next_hop_attribute(_In_ sai_object_id_t next_hop_id,
                                _In_ const sai_attribute_t *attr)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_NEXT_HOP);
    BRCM_SAI_SWITCH_INIT_CHECK;


    BRCM_SAI_FUNCTION_EXIT(SAI_API_NEXT_HOP);

    return rv;
}

/*
* Routine Description:
*    Get Next Hop attribute
*
* Arguments:
*    [in] sai_object_id_t - next_hop_id
*    [in] attr_count - number of attributes
*    [inout] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_get_next_hop_attribute(_In_ sai_object_id_t next_hop_id,
                                _In_ uint32_t attr_count,
                                _Inout_ sai_attribute_t *attr_list)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_NEXT_HOP);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_NEXT_HOP);

    return rv;
}

/*
################################################################################
#                                Functions map                                 #
################################################################################
*/
const sai_next_hop_api_t next_hop_apis = {
    brcm_sai_create_next_hop,
    brcm_sai_remove_next_hop,
    brcm_sai_set_next_hop_attribute,
    brcm_sai_get_next_hop_attribute
};

