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

char *_brcm_sai_api_type_strings[] =  {
    "",
    "SWITCH",
    "PORT",
    "FDB",
    "VLAN",
    "VIRTUAL_ROUTER",
    "ROUTE",
    "NEXT_HOP",
    "NEXT_HOP_GROUP",
    "ROUTER_INTERFACE",
    "NEIGHBOR",
    "ACL",
    "HOST_INTERFACE",
    "MIRROR",
    "SAMPLEPACKET",
    "STP",
    "LAG",
    "POLICER",
    "WRED",
    "QOSMAPS",
    "QUEUES",
    "SCHEDULER",
    "SCH_GROUP",
    "MMU_BUFFERS"
};

sai_status_t
BRCM_RV_OPENNSL_TO_SAI(int rv)
{
    switch(rv)
    {
        case OPENNSL_E_NONE:
            return SAI_STATUS_SUCCESS;
        case OPENNSL_E_PARAM:
            return SAI_STATUS_INVALID_PARAMETER;
        case OPENNSL_E_FAIL:
            return SAI_STATUS_FAILURE;
        case OPENNSL_E_UNAVAIL:
            return SAI_STATUS_NOT_SUPPORTED;
        case OPENNSL_E_NOT_FOUND:
            return SAI_STATUS_ITEM_NOT_FOUND;
        case OPENNSL_E_MEMORY:
            return SAI_STATUS_NO_MEMORY;
        case OPENNSL_E_EXISTS:
            return SAI_STATUS_ITEM_ALREADY_EXISTS;
        case OPENNSL_E_PORT:
            return SAI_STATUS_INVALID_PORT_NUMBER;
        case OPENNSL_E_INIT:
            return SAI_STATUS_UNINITIALIZED;
        case OPENNSL_E_FULL:
            return SAI_STATUS_TABLE_FULL;
        case OPENNSL_E_RESOURCE:
            return SAI_STATUS_INSUFFICIENT_RESOURCES;
        default:
            return SAI_STATUS_FAILURE;
    }
}

sai_status_t
BRCM_STAT_SAI_TO_OPENNSL(sai_port_stat_counter_t sai_stat, opennsl_stat_val_t *stat)
{
    switch(sai_stat)
    {
        case SAI_PORT_STAT_IF_IN_OCTETS:
            *stat = opennsl_spl_snmpIfInOctets;
            break;
        case SAI_PORT_STAT_IF_IN_UCAST_PKTS:
            *stat = opennsl_spl_snmpIfInUcastPkts;
            break;
        case SAI_PORT_STAT_IF_IN_NON_UCAST_PKTS:
            *stat = opennsl_spl_snmpIfInNUcastPkts;
            break;
        case SAI_PORT_STAT_IF_IN_DISCARDS:
            *stat = opennsl_spl_snmpIfInDiscards;
            break;
        case SAI_PORT_STAT_IF_IN_ERRORS:
            *stat = opennsl_spl_snmpIfInErrors;
            break;
        case SAI_PORT_STAT_IF_IN_UNKNOWN_PROTOS:
            *stat = opennsl_spl_snmpIfInUnknownProtos;
            break;
        case SAI_PORT_STAT_IF_IN_BROADCAST_PKTS:
            *stat = opennsl_spl_snmpIfInBroadcastPkts;
            break;
        case SAI_PORT_STAT_IF_IN_MULTICAST_PKTS:
            *stat = opennsl_spl_snmpIfInMulticastPkts;
            break;
        case SAI_PORT_STAT_IF_IN_VLAN_DISCARDS:
            *stat = snmpOpenNSLCustomReceive3;
            break;
        case SAI_PORT_STAT_IF_OUT_OCTETS:
            *stat = opennsl_spl_snmpIfOutOctets;
            break;
        case SAI_PORT_STAT_IF_OUT_UCAST_PKTS:
            *stat = opennsl_spl_snmpIfOutUcastPkts;
            break;
        case SAI_PORT_STAT_IF_OUT_NON_UCAST_PKTS:
            *stat = opennsl_spl_snmpIfOutNUcastPkts;
            break;
        case SAI_PORT_STAT_IF_OUT_DISCARDS:
            *stat = opennsl_spl_snmpIfOutDiscards;
            break;
        case SAI_PORT_STAT_IF_OUT_ERRORS:
            *stat = opennsl_spl_snmpIfOutErrors;
            break;
        case SAI_PORT_STAT_IF_OUT_QLEN:
            *stat = opennsl_spl_snmpIfOutQLen;
            break;
        case SAI_PORT_STAT_IF_OUT_BROADCAST_PKTS:
            *stat = opennsl_spl_snmpIfOutBroadcastPkts;
            break;
        case SAI_PORT_STAT_IF_OUT_MULTICAST_PKTS:
            *stat = opennsl_spl_snmpIfOutMulticastPkts;
            break;
        case SAI_PORT_STAT_ETHER_STATS_DROP_EVENTS:
            *stat = opennsl_spl_snmpEtherStatsDropEvents;
            break;
        case SAI_PORT_STAT_ETHER_STATS_MULTICAST_PKTS:
            *stat = opennsl_spl_snmpEtherStatsMulticastPkts;
            break;
        case SAI_PORT_STAT_ETHER_STATS_BROADCAST_PKTS:
            *stat = opennsl_spl_snmpEtherStatsBroadcastPkts;
            break;
        case SAI_PORT_STAT_ETHER_STATS_UNDERSIZE_PKTS:
            *stat = opennsl_spl_snmpEtherStatsUndersizePkts;
            break;
        case SAI_PORT_STAT_ETHER_STATS_FRAGMENTS:
            *stat = opennsl_spl_snmpEtherStatsFragments;
            break;
        case SAI_PORT_STAT_ETHER_STATS_PKTS_64_OCTETS:
            *stat = opennsl_spl_snmpEtherStatsPkts64Octets;
            break;
        case SAI_PORT_STAT_ETHER_STATS_PKTS_65_TO_127_OCTETS:
            *stat = opennsl_spl_snmpEtherStatsPkts65to127Octets;
            break;
        case SAI_PORT_STAT_ETHER_STATS_PKTS_128_TO_255_OCTETS:
            *stat = opennsl_spl_snmpEtherStatsPkts128to255Octets;
            break;
        case SAI_PORT_STAT_ETHER_STATS_PKTS_256_TO_511_OCTETS:
            *stat = opennsl_spl_snmpEtherStatsPkts256to511Octets;
            break;
        case SAI_PORT_STAT_ETHER_STATS_PKTS_512_TO_1023_OCTETS:
            *stat = opennsl_spl_snmpEtherStatsPkts512to1023Octets;
            break;
        case SAI_PORT_STAT_ETHER_STATS_PKTS_1024_TO_1518_OCTETS:
            *stat = opennsl_spl_snmpEtherStatsPkts1024to1518Octets;
            break;
        case SAI_PORT_STAT_ETHER_STATS_PKTS_1519_TO_2047_OCTETS:
            *stat = snmpOpenNSLReceivedPkts1519to2047Octets;
            break;
        case SAI_PORT_STAT_ETHER_STATS_PKTS_2048_TO_4095_OCTETS:
            *stat = snmpOpenNSLEtherStatsPkts2048to4095Octets;
            break;
        case SAI_PORT_STAT_ETHER_STATS_PKTS_4096_TO_9216_OCTETS:
            *stat = snmpOpenNSLEtherStatsPkts4095to9216Octets;
            break;
        case SAI_PORT_STAT_ETHER_STATS_PKTS_9217_TO_16383_OCTETS:
            *stat = snmpOpenNSLEtherStatsPkts9217to16383Octets;
            break;
        case SAI_PORT_STAT_ETHER_STATS_OVERSIZE_PKTS:
            *stat = opennsl_spl_snmpEtherStatsOversizePkts;
            break;
        case SAI_PORT_STAT_ETHER_RX_OVERSIZE_PKTS:
            *stat = opennsl_spl_snmpEtherRxOversizePkts;
            break;
        case SAI_PORT_STAT_ETHER_TX_OVERSIZE_PKTS:
            *stat = opennsl_spl_snmpEtherTxOversizePkts;
            break;
        case SAI_PORT_STAT_ETHER_STATS_JABBERS:
            *stat = opennsl_spl_snmpEtherStatsJabbers;
            break;
        case SAI_PORT_STAT_ETHER_STATS_OCTETS:
            *stat = opennsl_spl_snmpEtherStatsOctets;
            break;
        case SAI_PORT_STAT_ETHER_STATS_PKTS:
            *stat = opennsl_spl_snmpEtherStatsPkts;
            break;
        case SAI_PORT_STAT_ETHER_STATS_COLLISIONS:
            *stat = opennsl_spl_snmpEtherStatsCollisions;
            break;
        case SAI_PORT_STAT_ETHER_STATS_CRC_ALIGN_ERRORS:
            *stat = opennsl_spl_snmpEtherStatsCRCAlignErrors;
            break;
        case SAI_PORT_STAT_ETHER_STATS_TX_NO_ERRORS:
            *stat = opennsl_spl_snmpEtherStatsTXNoErrors;
            break;
        case SAI_PORT_STAT_ETHER_STATS_RX_NO_ERRORS:
            *stat = opennsl_spl_snmpEtherStatsRXNoErrors;
            break;
        case SAI_PORT_STAT_IP_IN_RECEIVES:
            *stat = snmpOpenNSLCustomReceive4;
            break;
        case SAI_PORT_STAT_IP_IN_OCTETS:
            *stat = opennsl_spl_snmpIfInOctets; /* Not supported */
            break;
        case SAI_PORT_STAT_IP_IN_UCAST_PKTS:
            *stat = opennsl_spl_snmpIpInReceives;
            break;
        case SAI_PORT_STAT_IP_IN_NON_UCAST_PKTS:
            *stat = snmpOpenNSLCustomReceive5;
            break;
        case SAI_PORT_STAT_IP_IN_DISCARDS:
            *stat = opennsl_spl_snmpIpInDiscards;
            break;
        case SAI_PORT_STAT_IP_OUT_OCTETS:
            *stat = opennsl_spl_snmpIfOutOctets; /* Not supported */
            break;
        case SAI_PORT_STAT_IP_OUT_UCAST_PKTS:
            *stat = snmpOpenNSLCustomTransmit6;
            break;
        case SAI_PORT_STAT_IP_OUT_NON_UCAST_PKTS:
            *stat = snmpOpenNSLCustomTransmit7;
            break;
        case SAI_PORT_STAT_IP_OUT_DISCARDS:
            *stat = snmpOpenNSLCustomTransmit8;
            break;
        case SAI_PORT_STAT_IPV6_IN_RECEIVES:
            *stat = opennsl_spl_snmpIpv6IfStatsInReceives;
            break;
        case SAI_PORT_STAT_IPV6_IN_OCTETS:
            *stat = opennsl_spl_snmpIfInOctets; /* Not supported */
            break;
        case SAI_PORT_STAT_IPV6_IN_UCAST_PKTS:
            *stat = snmpOpenNSLCustomReceive6;
            break;
        case SAI_PORT_STAT_IPV6_IN_NON_UCAST_PKTS:
            *stat = opennsl_spl_snmpIpv6IfStatsInMcastPkts;
            break;
        case SAI_PORT_STAT_IPV6_IN_MCAST_PKTS:
            *stat = opennsl_spl_snmpIpv6IfStatsInMcastPkts;
            break;
        case SAI_PORT_STAT_IPV6_IN_DISCARDS:
            *stat = opennsl_spl_snmpIpv6IfStatsInDiscards;
            break;
        case SAI_PORT_STAT_IPV6_OUT_OCTETS:
            *stat = opennsl_spl_snmpIfOutOctets; /* Not supported */
            break;
        case SAI_PORT_STAT_IPV6_OUT_UCAST_PKTS:
            *stat = snmpOpenNSLCustomTransmit9;
            break;
        case SAI_PORT_STAT_IPV6_OUT_NON_UCAST_PKTS:
            *stat = opennsl_spl_snmpIpv6IfStatsOutMcastPkts;
            break;
        case SAI_PORT_STAT_IPV6_OUT_MCAST_PKTS:
            *stat = opennsl_spl_snmpIpv6IfStatsOutMcastPkts;
            break;
        case SAI_PORT_STAT_IPV6_OUT_DISCARDS:
            *stat = opennsl_spl_snmpIpv6IfStatsOutDiscards;
            break;
        case SAI_PORT_STAT_ETHER_IN_PKTS_64_OCTETS:
            *stat = snmpOpenNSLReceivedPkts64Octets;
            break;
        case SAI_PORT_STAT_ETHER_IN_PKTS_65_TO_127_OCTETS:
            *stat = snmpOpenNSLReceivedPkts65to127Octets;
            break;
        case SAI_PORT_STAT_ETHER_IN_PKTS_128_TO_255_OCTETS:
            *stat = snmpOpenNSLReceivedPkts128to255Octets;
            break;
        case SAI_PORT_STAT_ETHER_IN_PKTS_256_TO_511_OCTETS:
            *stat = snmpOpenNSLReceivedPkts256to511Octets;
            break;
        case SAI_PORT_STAT_ETHER_IN_PKTS_512_TO_1023_OCTETS:
            *stat = snmpOpenNSLReceivedPkts512to1023Octets;
            break;
        case SAI_PORT_STAT_ETHER_IN_PKTS_1024_TO_1518_OCTETS:
            *stat = snmpOpenNSLReceivedPkts1024to1518Octets;
            break;
        case SAI_PORT_STAT_ETHER_IN_PKTS_1519_TO_2047_OCTETS:
            *stat = snmpOpenNSLReceivedPkts1519to2047Octets;
            break;
        case SAI_PORT_STAT_ETHER_IN_PKTS_2048_TO_4095_OCTETS:
            *stat = snmpOpenNSLReceivedPkts2048to4095Octets;
            break;
        case SAI_PORT_STAT_ETHER_IN_PKTS_4096_TO_9216_OCTETS:
            *stat = snmpOpenNSLReceivedPkts4095to9216Octets;
            break;
        case SAI_PORT_STAT_ETHER_IN_PKTS_9217_TO_16383_OCTETS:
            *stat = snmpOpenNSLReceivedPkts9217to16383Octets;
            break;
        case SAI_PORT_STAT_ETHER_OUT_PKTS_64_OCTETS:
            *stat = snmpOpenNSLTransmittedPkts64Octets;
            break;
        case SAI_PORT_STAT_ETHER_OUT_PKTS_65_TO_127_OCTETS:
            *stat = snmpOpenNSLTransmittedPkts65to127Octets;
            break;
        case SAI_PORT_STAT_ETHER_OUT_PKTS_128_TO_255_OCTETS:
            *stat = snmpOpenNSLTransmittedPkts128to255Octets;
            break;
        case SAI_PORT_STAT_ETHER_OUT_PKTS_256_TO_511_OCTETS:
            *stat = snmpOpenNSLTransmittedPkts256to511Octets;
            break;
        case SAI_PORT_STAT_ETHER_OUT_PKTS_512_TO_1023_OCTETS:
            *stat = snmpOpenNSLTransmittedPkts512to1023Octets;
            break;
        case SAI_PORT_STAT_ETHER_OUT_PKTS_1024_TO_1518_OCTETS:
            *stat = snmpOpenNSLTransmittedPkts1024to1518Octets;
            break;
        case SAI_PORT_STAT_ETHER_OUT_PKTS_1519_TO_2047_OCTETS:
            *stat = snmpOpenNSLTransmittedPkts1519to2047Octets;
            break;
        case SAI_PORT_STAT_ETHER_OUT_PKTS_2048_TO_4095_OCTETS:
            *stat = snmpOpenNSLTransmittedPkts2048to4095Octets;
            break;
        case SAI_PORT_STAT_ETHER_OUT_PKTS_4096_TO_9216_OCTETS:
            *stat = snmpOpenNSLTransmittedPkts4095to9216Octets;
            break;
        case SAI_PORT_STAT_ETHER_OUT_PKTS_9217_TO_16383_OCTETS:
            *stat = snmpOpenNSLTransmittedPkts9217to16383Octets;
            break;

        default:
            return SAI_STATUS_NOT_SUPPORTED;
    }
    return SAI_STATUS_SUCCESS;
}

#ifdef PRINT_TO_SYSLOG
uint8_t
_brcm_sai_to_syslog(uint8_t _sai_level)
{
    switch(_sai_level)
    {
        case SAI_LOG_DEBUG:
            return LOG_DEBUG;
            break;
        case SAI_LOG_INFO:
            return LOG_INFO;
            break;
        case SAI_LOG_WARN:
            return LOG_WARNING;
            break;
        case SAI_LOG_ERROR:
            return LOG_ERR;
            break;
        case SAI_LOG_CRITICAL:
            return LOG_CRIT;
            break;
        default:
            return LOG_DEBUG;
            break;
    }
    return LOG_DEBUG;
}
#endif
