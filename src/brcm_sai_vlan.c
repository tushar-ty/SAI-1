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
#                                 Vlan functions                               #
################################################################################
*/

/*
* Routine Description:
*    Create a VLAN
*
* Arguments:
*    [in] vlan_id - VLAN id
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_create_vlan(_In_ sai_vlan_id_t vlan_id)
{
    int unit  = 0, rv = 0;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_VLAN);
    BRCM_SAI_SWITCH_INIT_CHECK;

    if (false == VLAN_ID_CHECK(vlan_id))
    {
        BRCM_SAI_LOG_VLAN(SAI_LOG_ERROR, "Vlan create failed Id %d\n",
                          (int)vlan_id);
        return SAI_STATUS_INVALID_PARAMETER;
    }

    rv =  opennsl_vlan_create(unit, VLAN_CAST(vlan_id));
    if (OPENNSL_E_NONE != rv)
    {
        BRCM_SAI_LOG_VLAN(SAI_LOG_ERROR,
                          "Vlan create failed Id %d with error %s\n",
                          (int)vlan_id, opennsl_errmsg(rv));
        return BRCM_RV_OPENNSL_TO_SAI(rv);
    }

    BRCM_SAI_LOG_VLAN(SAI_LOG_DEBUG, "Create vid: %d\n", vlan_id);

    /* Add vlan to internal list of vlan bitmap */
    _brcm_sai_vlan_bmp_set(vlan_id);

    BRCM_SAI_FUNCTION_EXIT(SAI_API_VLAN);
    return SAI_STATUS_SUCCESS;
}

/*
* Routine Description:
*    Remove a VLAN
*
* Arguments:
*    [in] vlan_id - VLAN id
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_remove_vlan(_In_ sai_vlan_id_t vlan_id)
{
  int rv;
  int unit = 0;

  BRCM_SAI_FUNCTION_ENTER(SAI_API_VLAN);
  BRCM_SAI_SWITCH_INIT_CHECK;

  rv = opennsl_vlan_destroy(unit, VLAN_CAST(vlan_id));
  if (OPENNSL_E_NONE != rv)
  {
      if (OPENNSL_E_BADID == rv)
      {
          BRCM_SAI_LOG_VLAN(SAI_LOG_ERROR, "Cannot remove default vlan %d\n",
                            (int)vlan_id);
      }
      else if (OPENNSL_E_NOT_FOUND == rv)
      {
          BRCM_SAI_LOG_VLAN(SAI_LOG_ERROR, "Cannot remove Vlan %d, NOT FOUND\n",
                            (int)vlan_id);
      }
      else
      {
          BRCM_SAI_LOG_VLAN(SAI_LOG_ERROR, "Error %s removing Vlan %d\n",
                            opennsl_errmsg(rv), (int)vlan_id);
      }
  }

  _brcm_sai_vlan_bmp_clear(vlan_id);
  BRCM_SAI_LOG_VLAN(SAI_LOG_DEBUG, "Remove vid: %d\n", vlan_id);

  BRCM_SAI_FUNCTION_EXIT(SAI_API_VLAN);

  return BRCM_RV_OPENNSL_TO_SAI(rv);
}


/*
* Routine Description:
*    Add Port to VLAN
*
* Arguments:
*    [in] vlan_id - VLAN id
*    [in] port_count - number of ports
*    [in] port_list - pointer to membership structures
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_add_ports_to_vlan(_In_ sai_vlan_id_t vlan_id,
                           _In_ uint32_t port_count,
                           _In_ const sai_vlan_port_t* port_list)
{
    return _brcm_sai_add_ports_to_vlan(vlan_id,
                                       port_count,
                                       port_list);
}

/*
* Routine Description:
*    Remove Port from VLAN
*
* Arguments:
*    [in] vlan_id - VLAN id
*    [in] port_count - number of ports
*    [in] port_list - pointer to membership structures
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_remove_ports_from_vlan(_In_ sai_vlan_id_t vlan_id,
                                _In_ uint32_t port_count,
                                _In_ const sai_vlan_port_t* port_list)
{
    int i = 0, rv, unit = 0;
    opennsl_pbmp_t pbm;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_VLAN);
    BRCM_SAI_SWITCH_INIT_CHECK;

    if ((0 == port_count) || (0 == port_list))
    {
        BRCM_SAI_LOG_VLAN(SAI_LOG_ERROR, "Invalid param %u\n", port_count);
        return SAI_STATUS_INVALID_PARAMETER;
    }

    /* Check if VLAN exists */
    if (false == _brcm_sai_vlan_exists(vlan_id))
    {
        BRCM_SAI_LOG_VLAN(SAI_LOG_ERROR, "Invalid vlan id %u\n", (int)vlan_id);
        return SAI_STATUS_INVALID_PARAMETER;
    }


    /* Create port bitmap from port list in vlan_ports */
    OPENNSL_PBMP_CLEAR(pbm);

    for (i=0; i<port_count;i++)
    {
        int port = BRCM_SAI_GET_OBJ_VAL(int, port_list[i].port_id);
        OPENNSL_PBMP_PORT_ADD(pbm, port);
    }

    rv = opennsl_vlan_port_remove(unit, VLAN_CAST(vlan_id), pbm);
    if (OPENNSL_E_NONE != rv)
    {
        BRCM_SAI_LOG_VLAN(SAI_LOG_ERROR,
                          "Error %s removing ports from vlan id %u\n",
                          opennsl_errmsg(rv), (int)vlan_id);
    }

    BRCM_SAI_FUNCTION_EXIT(SAI_API_VLAN);
    return  BRCM_RV_OPENNSL_TO_SAI(rv);
}


/*
* Routine Description:
*    Set VLAN attribute Value
*
* Arguments:
*    [in] vlan_id - VLAN id
*    [in] attr - attribute
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_set_vlan_attribute(_In_ sai_vlan_id_t vlan_id,
                            _In_ const sai_attribute_t *attr)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_VLAN);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_VLAN);

    return rv;
}

/*
* Routine Description:
*    Get VLAN attribute Value
*
* Arguments:
*    [in] vlan_id - VLAN id
*    [in] attr_count - number of attributes
*    [inout] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_get_vlan_attribute(_In_ sai_vlan_id_t vlan_id,
                            _In_ uint32_t attr_count,
                            _Inout_ sai_attribute_t *attr_list)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_VLAN);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_VLAN);

    return rv;
}


/*
* Routine Description:
*    Remove VLAN configuration (remove all VLANs).
*
* Arguments:
*    None
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_remove_all_vlans(void)
{
    int rv, unit = 0;
    opennsl_vlan_t vid;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_VLAN);

    /* Remove all except default vlan */
    rv = opennsl_vlan_destroy_all(unit);

    if (OPENNSL_E_NONE == rv)
    {
        rv = opennsl_vlan_default_get(0, &vid);
        if (SAI_STATUS_SUCCESS != rv)
        {
            BRCM_SAI_LOG_RINTF(SAI_LOG_ERROR, "Error getting default vid\n");
            BRCM_RV_OPENNSL_TO_SAI(rv);
        }
        _brcm_sai_vlan_bmp_init(vid);
    }

    BRCM_SAI_FUNCTION_EXIT(SAI_API_VLAN);
    return  BRCM_RV_OPENNSL_TO_SAI(rv);
}

/*
* Routine Description:
*   Get vlan statistics counters.
*
* Arguments:
*    [in] vlan_id - VLAN id
*    [in] counter_ids - specifies the array of counter ids
*    [in] number_of_counters - number of counters in the array
*    [out] counters - array of resulting counter values.
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_get_vlan_stats(_In_ sai_vlan_id_t vlan_id,
                        _In_ const sai_vlan_stat_counter_t *counter_ids,
                        _In_ uint32_t number_of_counters,
                        _Out_ uint64_t* counters)
{
  return  SAI_STATUS_NOT_IMPLEMENTED;
}

/*
################################################################################
#                                Internal functions                            #
################################################################################
*/
/*
* Routine Description: Internal API to init vlan module. Also adds cpu
*   + xe ports to Vlan 1
*
* Arguments: None
* Return Values: Returns error on failure
*                SAI_STATUS_SUCCESS on success.
*/
sai_status_t
_brcm_sai_vlan_init()
{
    int rv = 0, unit = 0;
    opennsl_vlan_t vid;
    opennsl_port_config_t pcfg;

    /* Init the BRCM SAI vlan bitmap and set default vlan */
    rv = opennsl_vlan_default_get(0, &vid);
    if (SAI_STATUS_SUCCESS != rv)
    {
        BRCM_SAI_LOG_RINTF(SAI_LOG_ERROR, "Error getting default vid\n");
        BRCM_RV_OPENNSL_TO_SAI(rv);
    }
    _brcm_sai_vlan_bmp_init(vid);

    /* After switch init, go ahead and add all ports to default vlan 1*/
    rv = opennsl_port_config_get(unit, &pcfg);
    if (rv != OPENNSL_E_NONE) {
        BRCM_SAI_LOG_VLAN(SAI_LOG_ERROR,
                          "Failed to get port configuration. Error %s\n",
                          opennsl_errmsg(rv));
        return BRCM_RV_OPENNSL_TO_SAI(rv);
    }

    /* 1 is default VLAN during startup */
    rv = opennsl_vlan_port_add(unit, 1, pcfg.e, pcfg.e);
    if (rv != OPENNSL_E_NONE) {
        BRCM_SAI_LOG_VLAN(SAI_LOG_ERROR,
                          "Failed to add ports to default VLAN. Error %s\n",
                          opennsl_errmsg(rv));
        return BRCM_RV_OPENNSL_TO_SAI(rv);
    }

    /* Add CPU port as well */
    rv = opennsl_vlan_port_add(unit, 1, pcfg.cpu, pcfg.cpu);
    if (rv != OPENNSL_E_NONE) {
        BRCM_SAI_LOG_VLAN(SAI_LOG_ERROR,
                          "Failed to add CPU port to default VLAN. Error %s\n",
                          opennsl_errmsg(rv));
        return BRCM_RV_OPENNSL_TO_SAI(rv);
    }

    return SAI_STATUS_SUCCESS;
}

/*
################################################################################
#                                Functions map                                 #
################################################################################
*/
const sai_vlan_api_t vlan_apis = {
    brcm_sai_create_vlan,
    brcm_sai_remove_vlan,
    brcm_sai_set_vlan_attribute,
    brcm_sai_get_vlan_attribute,
    brcm_sai_add_ports_to_vlan,
    brcm_sai_remove_ports_from_vlan,
    brcm_sai_remove_all_vlans,
    brcm_sai_get_vlan_stats
};
