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
#                            Host interface functions                          #
################################################################################
*/

/*
* Routine Description:
*    Create host interface.
*
* Arguments:
*    [out] hif_id - host interface id
*    [in] attr_count - number of attributes
*    [in] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_create_host_interface(_Out_ sai_object_id_t* hif_id,
                               _In_ uint32_t attr_count,
                               _In_ const sai_attribute_t *attr_list)
{
    return _brcm_sai_create_host_interface(hif_id,
                                           attr_count,
                                           attr_list);
}

/*
* Routine Description:
*    Remove host interface
*
* Arguments:
*    [in] hif_id - host interface id
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_remove_host_interface(_In_ sai_object_id_t hif_id)
{
    return _brcm_sai_remove_host_interface(hif_id);
}

/*
* Routine Description:
*    Set host interface attribute
*
* Arguments:
*    [in] hif_id - host interface id
*    [in] attr - attribute
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_set_host_interface_attribute(_In_ sai_object_id_t hif_id,
                                      _In_ const sai_attribute_t *attr)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_HOST_INTERFACE);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_HOST_INTERFACE);
    return rv;
}

/*
* Routine Description:
*    Get host interface attribute
*
* Arguments:
*    [in] hif_id - host interface id
*    [in] attr_count - number of attributes
*    [inout] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_get_host_interface_attribute(_In_ sai_object_id_t hif_id,
                                      _In_ uint32_t attr_count,
                                      _Inout_ sai_attribute_t *attr_list)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_HOST_INTERFACE);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_HOST_INTERFACE);
    return rv;
}

/*
* Routine Description:
*   Set trap attribute value.
*
* Arguments:
*    [in] hostif_trap_id - host interface trap id
*    [in] attr - attribute
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_set_trap_attribute(_In_ sai_hostif_trap_id_t hostif_trapid,
                            _In_ const sai_attribute_t *attr)
{
    return _brcm_sai_set_trap_attribute(hostif_trapid, attr);
}

/*
* Routine Description:
*   Get trap attribute value.
*
* Arguments:
*    [in] hostif_trap_id - host interface trap id
*    [in] attr_count - number of attributes
*    [in,out] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_get_trap_attribute(_In_ sai_hostif_trap_id_t hostif_trapid,
                            _In_ uint32_t attr_count,
                            _Inout_ sai_attribute_t *attr_list)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_HOST_INTERFACE);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_HOST_INTERFACE);
    return rv;
}

/*
################################################################################
#                            Internal KNET functions                           #
################################################################################
*/

int
_brcm_sai_hintf_netif_clean(int unit, opennsl_knet_netif_t *netif, void *user_data)
{
    sai_status_t rv = 0;

    if (TRUE == (bool)user_data)
    {
        rv = opennsl_knet_netif_destroy(unit, netif->id);
        if (!OPENNSL_SUCCESS(rv))
        {
            BRCM_SAI_LOG_HINTF(SAI_LOG_ERROR, "Error 0x%x destroying netif %d\n",
                               rv, netif->id);
        }
        else
        {
            BRCM_SAI_LOG_HINTF(SAI_LOG_DEBUG, "Deleted netif %d\n",
                               netif->id);
        }
    }
    return rv;
}

int
_brcm_sai_hintf_filter_clean(int unit, opennsl_knet_filter_t *filter,
                             void *user_data)
{
    sai_status_t rv = 0;

    if (TRUE == (bool)user_data)
    {
        rv = opennsl_knet_filter_destroy(unit, filter->id);
        if (!OPENNSL_SUCCESS(rv))
        {
            BRCM_SAI_LOG_HINTF(SAI_LOG_ERROR, "Error 0x%x destroying netif %d\n",
                               rv, filter->id);
        }
        else
        {
            BRCM_SAI_LOG_HINTF(SAI_LOG_DEBUG, "Deleted netif %d\n",
                               filter->id);
        }
    }
    return rv;
}

sai_status_t
_brcm_sai_hostif_clean()
{
    sai_status_t rv;

    rv = opennsl_knet_netif_traverse(0, _brcm_sai_hintf_netif_clean, (void *)TRUE);
    if (!OPENNSL_SUCCESS(rv))
    {
        BRCM_SAI_LOG_HINTF(SAI_LOG_ERROR, "Error 0x%x traversing netifs.\n",rv);
        return BRCM_RV_OPENNSL_TO_SAI(rv);
    }
    rv = opennsl_knet_filter_traverse(0, _brcm_sai_hintf_filter_clean, (void *)TRUE);
    if (!OPENNSL_SUCCESS(rv))
    {
        BRCM_SAI_LOG_HINTF(SAI_LOG_ERROR, "Error 0x%x traversing filters.\n",rv);
        return BRCM_RV_OPENNSL_TO_SAI(rv);
    }
    return SAI_STATUS_SUCCESS;
}

/*
*  Host interface methods table retrieved with sai_api_query()
*/
const sai_hostif_api_t hostif_apis = {
    brcm_sai_create_host_interface,
    brcm_sai_remove_host_interface,
    brcm_sai_set_host_interface_attribute,
    brcm_sai_get_host_interface_attribute,
    NULL,
    NULL,
    NULL,
    NULL,
    brcm_sai_set_trap_attribute,
    brcm_sai_get_trap_attribute,
    NULL,
    NULL,
    NULL,
    NULL,
};
