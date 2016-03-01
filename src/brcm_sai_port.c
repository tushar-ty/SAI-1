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
#                               Event handlers                                 #
################################################################################
*/
void
_brcm_sai_link_event_cb(int unit, opennsl_port_t port,
                        opennsl_port_info_t *info)
{
    sai_port_oper_status_notification_t status;

    if (NULL == host_callbacks.on_port_state_change)
    {
        return;
    }
    status.port_id = BRCM_SAI_CREATE_OBJ(SAI_OBJECT_TYPE_PORT, port);
    if (OPENNSL_PORT_LINK_STATUS_UP == info->linkstatus)
    {
        BRCM_SAI_LOG_PORT(SAI_LOG_INFO, "Port %d link status event: UP\n",
                          port);
        status.port_state = SAI_PORT_OPER_STATUS_UP;
    }
    else
    {
        BRCM_SAI_LOG_PORT(SAI_LOG_INFO, "Port %d link status event: DOWN\n",
                          port);
        status.port_state = SAI_PORT_OPER_STATUS_DOWN;
    }
    host_callbacks.on_port_state_change(1, &status);
}

/*
################################################################################
#                              Port functions                                  #
################################################################################
*/
/*
* Routine Description:
*   Set port attribute value.
*
* Arguments:
*    [in] port_id - port id
*    [in] attr - attribute
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_set_port_attribute(_In_ sai_object_id_t port_id,
                            _In_ const sai_attribute_t *attr)
{
    return _brcm_sai_set_port_attribute(port_id, attr);
}

/*
* Routine Description:
*   Get port attribute value.
*
* Arguments:
*    [in] port_id - port id
*    [in] attr_count - number of attributes
*    [inout] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_get_port_attribute(_In_ sai_object_id_t port_id,
                            _In_ uint32_t attr_count,
                            _Inout_ sai_attribute_t *attr_list)
{
    return _brcm_sai_get_port_attribute(port_id,
                                        attr_count,
                                        attr_list);
}

/*
* Routine Description:
*   Get port statistics counters.
*
* Arguments:
*    [in] port_id - port id
*    [in] counter_ids - specifies the array of counter ids
*    [in] number_of_counters - number of counters in the array
*    [out] counters - array of resulting counter values.
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
STATIC sai_status_t
brcm_sai_get_port_stats(_In_ sai_object_id_t port_id,
                        _In_ const sai_port_stat_counter_t *counter_ids,
                        _In_ uint32_t number_of_counters,
                        _Out_ uint64_t* counters)
{
    int i, j, counter_idx;
    sai_status_t rv;
    opennsl_port_t port;
    opennsl_stat_val_t *stats;
    struct {
        sai_port_stat_counter_t stat;
        int offset;
    } skip_stats[] = {
        { SAI_PORT_STAT_IP_IN_OCTETS, -1 },
        { SAI_PORT_STAT_IP_OUT_OCTETS, -1 },
        { SAI_PORT_STAT_IPV6_IN_OCTETS, -1 },
        { SAI_PORT_STAT_IPV6_OUT_OCTETS, -1 }
    };
    bool request_tx_count = false;
    uint64 tx_count;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_PORT);
    BRCM_SAI_SWITCH_INIT_CHECK;

    stats = (opennsl_stat_val_t*)
            malloc(number_of_counters * sizeof(opennsl_stat_val_t));
    if (NULL == stats)
    {
        BRCM_SAI_LOG_PORT(SAI_LOG_CRITICAL,
                          "Error allocating memory for port stats.\n");
        return SAI_STATUS_NO_MEMORY;
    }
    for(i=0; i<number_of_counters; i++)
    {
        if (SAI_PORT_STAT_ETHER_STATS_PKTS_1519_TO_2047_OCTETS == counter_ids[i])
        {
            /* If the requested counter is the Rx/Tx version of the
             * 1519_to_2047 counter, we are only getting the "In" version
             * in the "multi get", below. We need to request the "Out"
             * version as well.
             */
            request_tx_count = true;
            counter_idx = i;
        }
        rv = BRCM_STAT_SAI_TO_OPENNSL(counter_ids[i], &stats[i]);
        if (SAI_STATUS_SUCCESS != rv)
        {
            BRCM_SAI_LOG_PORT(SAI_LOG_ERROR,
                              "Unknown stat[%d] type: %d.\n", i, counter_ids[i]);
            return rv;
        }
        for(j=0; j<COUNTOF(skip_stats); j++)
        {
            if(counter_ids[i] == skip_stats[j].stat)
            {
                skip_stats[j].offset = i;
                break;
            }
        }
    }
    port = BRCM_SAI_GET_OBJ_VAL(opennsl_port_t, port_id);
    rv = opennsl_stat_multi_get(0, port, number_of_counters, stats,
                                (uint64*)counters);
    BRCM_SAI_API_CHK(SAI_API_PORT, "Multi stats get", rv);
    if (TRUE == request_tx_count)
    {
        rv = opennsl_stat_get(0, port,
                              snmpOpenNSLTransmittedPkts1519to2047Octets, &tx_count);
        BRCM_SAI_API_CHK(SAI_API_PORT, "Stat get", rv);
        counters[counter_idx] += tx_count;
    }
    for(i=0; i<COUNTOF(skip_stats); i++)
    {
        if (skip_stats[i].offset >= 0)
        {
            counters[skip_stats[i].offset] = 0;
        }
    }
    free(stats);
    BRCM_SAI_FUNCTION_EXIT(SAI_API_PORT);
    return rv;
}

/*
################################################################################
#                                Functions map                                 #
################################################################################
*/
const sai_port_api_t port_apis = {
    brcm_sai_set_port_attribute,
    brcm_sai_get_port_attribute,
    brcm_sai_get_port_stats
};
