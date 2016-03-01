/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2013-2015
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
#                           Next hop group functions                           #
################################################################################
*/

/*
* Routine Description:
*    Create next hop group
*
* Arguments:
*    [out] next_hop_group_id - next hop group id
*    [in] attr_count - number of attributes
*    [in] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_create_next_hop_group(_Out_ sai_object_id_t* next_hop_group_id,
                               _In_ uint32_t attr_count,
                               _In_ const sai_attribute_t *attr_list)
{
    int i, j, count = 0;
    sai_status_t rv;
    opennsl_l3_egress_ecmp_t ecmp_object;
    opennsl_if_t *if_t = NULL;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_NEXT_HOP_GROUP);
    BRCM_SAI_SWITCH_INIT_CHECK;
    BRCM_SAI_OBJ_CREATE_PARAM_CHK(next_hop_group_id);

    opennsl_l3_egress_ecmp_t_init(&ecmp_object);

    for (i=0; i<attr_count; i++)
    {
        switch (attr_list[i].id)
        {
            case SAI_NEXT_HOP_GROUP_ATTR_TYPE:
                if (SAI_NEXT_HOP_GROUP_ECMP != attr_list[i].value.u32)
                {
                    CHECK_FREE(if_t);
                    return SAI_STATUS_NOT_IMPLEMENTED;
                }
                break;
            case SAI_NEXT_HOP_GROUP_ATTR_NEXT_HOP_LIST:
                count = BRCM_SAI_ATTR_LIST_OBJ_COUNT(i);
                if (0 == count)
                {
                    BRCM_SAI_LOG_NHG(SAI_LOG_ERROR, "Error param %d\n", count);
                    return SAI_STATUS_INVALID_PARAMETER;
                }
                if_t = (opennsl_if_t*)malloc(count * sizeof(opennsl_if_t));
                if (NULL == if_t)
                {
                    BRCM_SAI_LOG_NHG(SAI_LOG_ERROR, "Error with alloc %d\n",
                                     count);
                    return SAI_STATUS_NO_MEMORY;
                }
                for (j=0; j<count; j++)
                {
                    if_t[j] = BRCM_SAI_ATTR_OBJ_LIST(i, j);
                    BRCM_SAI_LOG_NHG(SAI_LOG_DEBUG, "path %d: %d\n", j, if_t[j]);
                }
                break;
            default:
                BRCM_SAI_LOG_NHG(SAI_LOG_INFO,
                                 "Unknown nexthop group attribute %d passed\n",
                                 attr_list[i].id);
                return SAI_STATUS_INVALID_PARAMETER;
        }
    }
    if (0 == count)
    {
        BRCM_SAI_LOG_NHG(SAI_LOG_ERROR, "Nexthop list of zero size.\n");
        return SAI_MANDATORY_ATTRIBUTE_MISSING;
    }
    BRCM_SAI_LOG_NHG(SAI_LOG_DEBUG, "Create nh group with %d paths\n", count);
    rv = opennsl_l3_egress_ecmp_create(0, &ecmp_object, count, if_t);
    CHECK_FREE(if_t);
    BRCM_SAI_API_CHK(SAI_API_NEXT_HOP_GROUP, "ecmp nh group create", rv);

    *next_hop_group_id = BRCM_SAI_CREATE_OBJ(SAI_OBJECT_TYPE_NEXT_HOP_GROUP,
                                             ecmp_object.ecmp_intf);
    BRCM_SAI_FUNCTION_EXIT(SAI_API_NEXT_HOP_GROUP);

    return rv;
}

/*
* Routine Description:
*    Remove next hop group
*
* Arguments:
*    [in] next_hop_group_id - next hop group id
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_remove_next_hop_group(_In_ sai_object_id_t next_hop_group_id)
{
    sai_status_t rv;
    opennsl_l3_egress_ecmp_t ecmp_object;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_NEXT_HOP_GROUP);

    BRCM_SAI_SWITCH_INIT_CHECK;
    opennsl_l3_egress_ecmp_t_init(&ecmp_object);
    ecmp_object.ecmp_intf = BRCM_SAI_GET_OBJ_VAL(opennsl_if_t,
                                                 next_hop_group_id);
    rv = opennsl_l3_egress_ecmp_destroy(0, &ecmp_object);
    BRCM_SAI_API_CHK(SAI_API_NEXT_HOP_GROUP, "ecmp nh group delete", rv);

    BRCM_SAI_FUNCTION_EXIT(SAI_API_NEXT_HOP_GROUP);

    return rv;
}

/*
* Routine Description:
*    Set Next Hop Group attribute
*
* Arguments:
*    [in] next_hop_group_id - next_hop_group_id
*    [in] attr - attribute
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_set_next_hop_group_attribute(_In_ sai_object_id_t next_hop_group_id,
                                      _In_ const sai_attribute_t *attr)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_NEXT_HOP_GROUP);

    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_NEXT_HOP_GROUP);

    return rv;
}

/*
* Routine Description:
*    Get Next Hop Group attribute
*
* Arguments:
*    [in] next_hop_group_id - next_hop_group_id
*    [in] attr_count - number of attributes
*    [inout] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_get_next_hop_group_attribute(_In_ sai_object_id_t next_hop_group_id,
                                      _In_ uint32_t attr_count,
                                      _Inout_ sai_attribute_t *attr_list)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_NEXT_HOP_GROUP);

    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_NEXT_HOP_GROUP);

    return rv;
}

/*
* Routine Description:
*    Add next hop to a group
*
* Arguments:
*    [in] next_hop_group_id - next hop group id
*    [in] next_hop_count - number of next hops
*    [in] nexthops - array of next hops
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_add_next_hop_to_group(_In_ sai_object_id_t next_hop_group_id,
                               _In_ uint32_t next_hop_count,
                               _In_ const sai_object_id_t* nexthops)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_NEXT_HOP_GROUP);

    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_NEXT_HOP_GROUP);

    return rv;
}

/*
* Routine Description:
*    Remove next hop from a group
*
* Arguments:
*    [in] next_hop_group_id - next hop group id
*    [in] next_hop_count - number of next hops
*    [in] nexthops - array of next hops
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_remove_next_hop_from_group(_In_ sai_object_id_t next_hop_group_id,
                                    _In_ uint32_t next_hop_count,
                                    _In_ const sai_object_id_t* nexthops)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_NEXT_HOP_GROUP);

    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_NEXT_HOP_GROUP);

    return rv;
}

/*
*  Next Hop methods table retrieved with sai_api_query()
*/
const sai_next_hop_group_api_t next_hop_grp_apis = {
    brcm_sai_create_next_hop_group,
    brcm_sai_remove_next_hop_group,
    brcm_sai_set_next_hop_group_attribute,
    brcm_sai_get_next_hop_group_attribute,
    brcm_sai_add_next_hop_to_group,
    brcm_sai_remove_next_hop_from_group
};
