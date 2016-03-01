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

#if !defined (_BRM_SAI_COMMON)
#define _BRM_SAI_COMMON

/*
################################################################################
#                                 Public includes                              #
################################################################################
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

/*
################################################################################
#                               All internal includes                          #
################################################################################
*/
#include <sal/commdefs.h>
#include <sal/types.h>
#include <sal/driver.h>
#include <opennsl/error.h>
#include <opennsl/switch.h>
#include <opennsl/port.h>
#include <opennsl/stg.h>
#include <opennsl/l2.h>
#include <opennsl/l3.h>
#include <opennsl/link.h>
#include <opennsl/knet.h>
#include <opennsl/cosq.h>
#ifdef PRINT_TO_SYSLOG
#include <syslog.h>
#endif
#include "brcm_sai_custom_attr.h"

#ifndef STATIC
#define STATIC
#endif /* !STATIC */

/*
################################################################################
#                            Common internal defines                           #
################################################################################
*/
#define BRCM_SAI_API_ID_MIN               SAI_API_SWITCH
#define BRCM_SAI_API_ID_MAX               SAI_API_BUFFERS

#define _BRCM_SAI_MASK_8                  0xFF
#define _BRCM_SAI_MASK_16                 0xFFFF
#define _BRCM_SAI_MASK_32                 0xFFFFFFFF
#define _BRCM_SAI_MASK_64                 0xFFFFFFFFFFFFFFFF
#define _BRCM_SAI_MASK_64_UPPER_32        0xFFFFFFFF00000000
#define _BRCM_SAI_MASK_64_LOWER_32        0x00000000FFFFFFFF

#define _BRCM_SAI_VR_MAX_VID              4094
#define _BRCM_SAI_VR_DEFAULT_TTL          64
#define _BRCM_SAI_MAX_PORTS               32
#define _BRCM_SAI_MAX_FILTERS_PER_INTF    32

#define _BRCM_SAI_QUEUE_TYPE_MULTICAST    1
#define _BRCM_SAI_L0_SCHEDULER_TYPE       2
#define _BRCM_SAI_L1_SCHEDULER_TYPE       3
#define _BRCM_SAI_PORT_SCHEDULER_TYPE     4
#define _BRCM_SAI_SCHEDULER_ATTR_INVALID  -1
#define _BRCM_SAI_MAX_SCHEDULER_PROFILES  128
#define _BRCM_SAI_SCHEDULER_WEIGHT_MIN    1
#define _BRCM_SAI_SCHEDULER_WEIGHT_MAX    100

#define _BRCM_SAI_WRED_ATTR_GREEN         0x1
#define _BRCM_SAI_WRED_ATTR_YELLOW        0x2
#define _BRCM_SAI_WRED_ATTR_RED           0x4

#define _BRCM_SAI_STORMCONTROL_MAX_CBS    16777249   /* Only valid in Byte Mode */
#define _BRCM_SAI_STORMCONTROL_MAX_PPS    3355441600u /* In Packet Mode */

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/*
################################################################################
#                                   Common types                               #
################################################################################
*/
typedef uint8_t bitmap_t;

typedef struct _brcm_sai_netif_map_s {
    /* Max 256 ids */
    uint8_t netif_id;
    uint8_t filter_id[_BRCM_SAI_MAX_FILTERS_PER_INTF];
    uint8_t num_filters;
} _brcm_sai_netif_map_t;

typedef struct _brcm_sai_qos_ingress_map_s {
    bool valid;            /* Set to TRUE when used */
    uint32_t count;        /* Number of valid entries in this map */
    sai_qos_map_t map[64];
    int inuse;             /* To track the reference count */
} _brcm_sai_qos_ingress_map_t;

typedef struct _brcm_sai_qos_egress_map_s {
    bool valid;            /* Set to TRUE when used */
    uint32_t count;        /* Number of valid entries in this map */
    sai_qos_map_t map[16];
    int inuse;             /* To track the reference count */
} _brcm_sai_qos_egress_map_t;

typedef struct _brcm_sai_scheduler_object_s {
    sai_object_id_t object_id;
    struct _brcm_sai_scheduler_object_s *next;
} _brcm_sai_scheduler_object_t;

typedef struct _brcm_sai_qos_scheduler_s {
    bool valid;
    sai_scheduling_type_t algorithm;
    uint8_t weight;
    sai_meter_type_t shaper_type;
    uint64_t minimum_bandwidth;
    uint64_t minimum_burst;
    uint64_t maximum_bandwidth;
    uint64_t maximum_burst;
    _brcm_sai_scheduler_object_t *object_list;
    int ref_count;
} _brcm_sai_qos_scheduler_t;

typedef struct _brcm_sai_buf_pool_s {
    bool valid;
    sai_buffer_pool_type_t type;
    sai_uint32_t size;
    sai_buffer_threshold_mode_t mode;
} _brcm_sai_buf_pool_t;

typedef struct _brcm_sai_buf_profile_s {
    bool valid;
    sai_object_id_t pool_id;
    sai_uint32_t size;
    sai_uint32_t shared_val; /* dynamic or static depending upon SAI_BUFFER_POOL_TH_MODE */
    sai_uint32_t xoff_thresh;
    sai_uint32_t xon_thresh;
} _brcm_sai_buf_profile_t;

/*
################################################################################
#                                  Common macros                               #
################################################################################
*/

#ifndef SAI_LOG_OFF

#define BRCM_SAI_CHK_LOG(api, log_level) if (sai_log_check(api, log_level))

#ifdef PRINT_TO_SYSLOG
extern
uint8_t _brcm_sai_to_syslog(uint8_t sai_log);
/* Internal Log define, dont use directly */
#define _BRCM_SAI_INT_LOG(api, sai_log_level, __fmt, __args...)    \
  do {                                                           \
      BRCM_SAI_CHK_LOG(api, sai_log_level)                       \
        syslog(_brcm_sai_to_syslog((sai_log_level)),             \
               "%s:%d " __fmt, __FUNCTION__, __LINE__,##__args); \
  }                                                              \
  while(0)

#else

/* Internal Log define, dont use directly */
#define _BRCM_SAI_INT_LOG(api, log_level, __fmt, __args...)        \
  do {                                                           \
      BRCM_SAI_CHK_LOG(api, log_level)                           \
        printf("%s:%d " __fmt, __FUNCTION__, __LINE__,##__args); \
  }                                                              \
  while(0)

#endif

#define BRCM_SAI_LOG(__fmt, __args...)  \
  _BRCM_SAI_INT_LOG(SAI_API_UNSPECIFIED, SAI_LOG_DEBUG, __fmt,##__args)

#else /* SAI_LOG_OFF */
#define _BRCM_SAI_INT_LOG(...)
#define BRCM_SAI_LOG(...)
#endif

/*                      Object management macros
 * +--------------|---------|---------|-----------------------+
 * |63..........48|47.....40|39.....32|31....................0|
 * +--------------|---------|---------|-----------------------+
 * |      map     | subtype |   type  |           id          |
 * +--------------|---------|---------|-----------------------+
 */
#define BRCM_SAI_CREATE_OBJ(type, value) ((((sai_object_id_t)type) << 32) | value)
#define BRCM_SAI_CREATE_OBJ_SUB(type, subtype, value) ((((sai_object_id_t)subtype) << 40) | \
                                                       (((sai_object_id_t)type) << 32) | value)
#define BRCM_SAI_CREATE_OBJ_SUB_MAP(type, subtype, map, value) ((((sai_object_id_t)map) << 48) | \
                                                                (((sai_object_id_t)subtype) << 40) | \
                                                                (((sai_object_id_t)type) << 32) | value)
#define BRCM_SAI_GET_OBJ_TYPE(var) ((uint8_t)(var >> 32))
#define BRCM_SAI_GET_OBJ_SUB_TYPE(var) ((uint8_t)(var >> 40))
#define BRCM_SAI_GET_OBJ_MAP(var) ((uint16_t)(var >> 48))
#define BRCM_SAI_GET_OBJ_VAL(to_type, var) ((to_type)var)

/* Attribute value retreival macros */
#define BRCM_SAI_ATTR_PTR_OBJ(a) attr->value.oid
#define BRCM_SAI_ATTR_OBJ(a) attr_list.value.oid
#define BRCM_SAI_ATTR_LIST_OBJ(a) attr_list[a].value.oid
#define BRCM_SAI_ATTR_LIST_OBJ_VAL(t, a) BRCM_SAI_GET_OBJ_VAL(t, BRCM_SAI_ATTR_LIST_OBJ(a))
#define BRCM_SAI_ATTR_LIST_OBJ_COUNT(a) attr_list[a].value.objlist.count
#define BRCM_SAI_ATTR_OBJ_LIST(a, o) attr_list[a].value.objlist.list[o]
#define BRCM_SAI_ATTR_ACL_FLD_8(a) (attr_list[a].value.aclfield.data.u8)
#define BRCM_SAI_ATTR_ACL_FLD_MASK_8(a) (attr_list[a].value.aclfield.mask.u8)
#define BRCM_SAI_ATTR_ACL_FLD_16(a) (attr_list[a].value.aclfield.data.u16)
#define BRCM_SAI_ATTR_ACL_FLD_MASK_16(a) (attr_list[a].value.aclfield.mask.u16)
#define BRCM_SAI_ATTR_ACL_FLD_32(a) (attr_list[a].value.aclfield.data.u32)
#define BRCM_SAI_ATTR_ACL_FLD_MASK_32(a) (attr_list[a].value.aclfield.mask.u32)
#define BRCM_SAI_ATTR_ACL_FLD_MAC(a) (attr_list[a].value.aclfield.data.mac)
#define BRCM_SAI_ATTR_ACL_FLD_MASK_MAC(a) (attr_list[a].value.aclfield.mask.mac)
#define BRCM_SAI_ATTR_ACL_FLD_IP4(a) (attr_list[a].value.aclfield.data.ip4)
#define BRCM_SAI_ATTR_ACL_FLD_MASK_IP4(a) (attr_list[a].value.aclfield.mask.ip4)
#define BRCM_SAI_ATTR_ACL_FLD_IP6(a) (attr_list[a].value.aclfield.data.ip6)
#define BRCM_SAI_ATTR_ACL_FLD_MASK_IP6(a) (attr_list[a].value.aclfield.mask.ip6)
#define BRCM_SAI_ATTR_ACL_FLD_OBJ(a) (attr_list[a].value.aclfield.data.oid)
#define BRCM_SAI_ATTR_ACL_FLD_OBJ_COUNT(a) (attr_list[a].value.aclfield.data.objlist.count)
#define BRCM_SAI_ATTR_ACL_FLD_OBJ_LIST(a, o) (attr_list[a].value.aclfield.data.objlist.list[o])
#define BRCM_SAI_ATTR_ACL_ACTION_8(a) ((uint32_t)attr_list[a].value.aclaction.parameter.u8)
#define BRCM_SAI_ATTR_ACL_ACTION_16(a) ((uint32_t)attr_list[a].value.aclaction.parameter.u16)
#define BRCM_SAI_ATTR_ACL_ACTION_32(a) ((uint32_t)attr_list[a].value.aclaction.parameter.u32)
#define BRCM_SAI_ATTR_ACL_ACTION_MAC(a) (attr_list[a].value.aclaction.parameter.mac)
#define BRCM_SAI_ATTR_ACL_ACTION_OBJ(a) (attr_list[a].value.aclaction.parameter.oid)

/* Per module logging macros */
#define BRCM_SAI_LOG_SWITCH(log_level,__fmt,__args...)  \
  _BRCM_SAI_INT_LOG(SAI_API_SWITCH, log_level,__fmt,##__args)

#define BRCM_SAI_LOG_PORT(log_level,__fmt,__args...)  \
  _BRCM_SAI_INT_LOG(SAI_API_PORT, log_level,__fmt,##__args)

#define BRCM_SAI_LOG_FDB(log_level,__fmt,__args...)  \
  _BRCM_SAI_INT_LOG(SAI_API_FDB, log_level,__fmt,##__args)

#define BRCM_SAI_LOG_VLAN(log_level,__fmt,__args...)  \
  _BRCM_SAI_INT_LOG(SAI_API_VLAN, log_level,__fmt,##__args)

#define BRCM_SAI_LOG_VR(log_level,__fmt,__args...)  \
  _BRCM_SAI_INT_LOG(SAI_API_VIRTUAL_ROUTER, log_level,__fmt,##__args)

#define BRCM_SAI_LOG_ROUTE(log_level,__fmt,__args...)  \
  _BRCM_SAI_INT_LOG(SAI_API_ROUTE, log_level,__fmt,##__args)

#define BRCM_SAI_LOG_NH(log_level,__fmt,__args...)  \
  _BRCM_SAI_INT_LOG(SAI_API_NEXT_HOP, log_level,__fmt,##__args)

#define BRCM_SAI_LOG_NHG(log_level,__fmt,__args...)  \
  _BRCM_SAI_INT_LOG(SAI_API_NEXT_HOP_GROUP, log_level,__fmt,##__args)

#define BRCM_SAI_LOG_RINTF(log_level,__fmt,__args...)  \
  _BRCM_SAI_INT_LOG(SAI_API_ROUTER_INTERFACE, log_level,__fmt,##__args)

#define BRCM_SAI_LOG_NBOR(log_level,__fmt,__args...)  \
  _BRCM_SAI_INT_LOG(SAI_API_NEIGHBOR, log_level,__fmt,##__args)

#define BRCM_SAI_LOG_ACL(log_level,__fmt,__args...)  \
  _BRCM_SAI_INT_LOG(SAI_API_ACL, log_level,__fmt,##__args)

#define BRCM_SAI_LOG_HINTF(log_level,__fmt,__args...)  \
  _BRCM_SAI_INT_LOG(SAI_API_HOST_INTERFACE, log_level,__fmt,##__args)

#define BRCM_SAI_LOG_POLICER(log_level,__fmt,__args...)  \
  _BRCM_SAI_INT_LOG(SAI_API_POLICER, log_level,__fmt,##__args)

#define BRCM_SAI_LOG_QMAP(log_level,__fmt,__args...)  \
  _BRCM_SAI_INT_LOG(SAI_API_QOS_MAPS, log_level,__fmt,##__args)

#define BRCM_SAI_LOG_QUEUE(log_level,__fmt,__args...)  \
  _BRCM_SAI_INT_LOG(SAI_API_QUEUE, log_level,__fmt,##__args)

#define BRCM_SAI_LOG_WRED(log_level,__fmt,__args...)  \
  _BRCM_SAI_INT_LOG(SAI_API_WRED, log_level,__fmt,##__args)

#define BRCM_SAI_LOG_SCHEDULER(log_level,__fmt,__args...)  \
  _BRCM_SAI_INT_LOG(SAI_API_SCHEDULER, log_level,__fmt,##__args)

#define BRCM_SAI_LOG_MMU_BUFFER(log_level,__fmt,__args...)  \
  _BRCM_SAI_INT_LOG(SAI_API_BUFFERS, log_level,__fmt,##__args)

/* Error check macros */
#define BRCM_SAI_SWITCH_INIT_CHECK                                             \
  do {                                                                         \
      if (false == _brcm_sai_switch_is_inited())                               \
      {                                                                        \
          BRCM_SAI_LOG_SWITCH(SAI_LOG_ERROR, "Switch not initialized yet.\n"); \
          return SAI_STATUS_UNINITIALIZED;                                     \
      }                                                                        \
  }while(0)

#define BRCM_SAI_OBJ_CREATE_PARAM_CHK(ptr)                                    \
  do {                                                                        \
    if ((NULL == ptr) || (NULL == attr_list) || (0 == attr_count))            \
    {                                                                         \
        return SAI_STATUS_INVALID_PARAMETER;                                  \
    }                                                                         \
  }while(0)

#define BRCM_SAI_GET_ATTRIB_PARAM_CHK                                         \
  do {                                                                        \
    if ((NULL == attr_list) || (0 == attr_count))                             \
    {                                                                         \
        return SAI_STATUS_INVALID_PARAMETER;                                  \
    }                                                                         \
  }while(0)

#define BRCM_SAI_FUNCTION_ENTER(api)  \
  _BRCM_SAI_INT_LOG(api, SAI_LOG_INFO, "SAI Enter %s\n", __FUNCTION__)
#define BRCM_SAI_FUNCTION_EXIT(api)  \
  _BRCM_SAI_INT_LOG(api, SAI_LOG_INFO, "SAI Exit %s\n", __FUNCTION__)

#ifndef SAI_CLOSED_SOURCE

#define BRCM_SAI_API_CHK(api, prepend, rv)                           \
  if (OPENNSL_E_NONE != rv)                                          \
  {                                                                  \
      _BRCM_SAI_INT_LOG(api,                                         \
                       SAI_LOG_ERROR,                                \
                       "%s failed with error %s (0x%x).\n", prepend, \
                       opennsl_errmsg(rv),                           \
                       rv);                                          \
      return BRCM_RV_OPENNSL_TO_SAI(rv);                             \
  }

#define BRCM_SAI_ATTR_API_CHK(api, prepend, rv, id)                   \
  if (OPENNSL_E_NONE != rv)                                           \
  {                                                                   \
      _BRCM_SAI_INT_LOG(api,                                          \
                       SAI_LOG_ERROR,                                 \
                       "%s attrib %d failed with error %s (0x%x).\n", \
                       prepend,                                       \
                       id,                                            \
                       opennsl_errmsg(rv),                            \
                       rv);                                           \
      return BRCM_RV_OPENNSL_TO_SAI(rv);                              \
  }

#define BRCM_SAI_NUM_ATTR_API_CHK(api, prepend, num, rv, id)             \
  if (OPENNSL_E_NONE != rv)                                              \
  {                                                                      \
      _BRCM_SAI_INT_LOG(api,                                             \
                       SAI_LOG_ERROR,                                    \
                       "%s %d attrib %d failed with error %s (0x%x).\n", \
                       prepend, num, id,                                 \
                       opennsl_errmsg(rv),                               \
                       rv);                                              \
      return BRCM_RV_OPENNSL_TO_SAI(rv);                                 \
  }

#endif  /* SAI_CLOSED_SOURCE */

#define CHECK_FREE(__ptr)  \
  do {                     \
      if (NULL != (__ptr)) \
      {                    \
          free((__ptr));   \
      }                    \
  } while (0)

/* Bit manipulation macros */
#define BYTE_SIZE (sizeof(bitmap_t) * 8)

/* Here num_bits is the count of items to create a bitmap of */
#define BRCM_SAI_NUM_BYTES(__num_bits)    ((((__num_bits) - 1) / BYTE_SIZE) + 1)

/* __map is bitmap and __bitp is the bit position */
#define BRCM_SAI_SET_BIT(__map, __bitp)                                             \
    do {                                                                            \
        ((__map)[((__bitp) - 1) / BYTE_SIZE] |= 1 << (((__bitp) - 1) % BYTE_SIZE)); \
    } while(0)

#define BRCM_SAI_CLEAR_BIT(__map, __bitp)                                              \
    do {                                                                               \
        ((__map)[((__bitp) - 1) / BYTE_SIZE] &= ~(1 << (((__bitp) - 1) % BYTE_SIZE))); \
    } while (0)

#define BRCM_SAI_IS_SET(__map, __bitp) \
  ((__map)[(((__bitp) - 1) / BYTE_SIZE)] & ( 1 << (((__bitp) - 1) % BYTE_SIZE)))

#define BRCM_SAI_MAX_UNSET(__map,__pos,__max)                       \
    do {                                                            \
        sai_uint32_t i,j=0;                                         \
        sai_uint32_t __size = BRCM_SAI_NUM_BYTES(__max);            \
        uint8_t mask = 0xFF;                                        \
        for (i=(__size)-1; i>=0; i--)                               \
        {                                                           \
            if ( ((__size) -1) == i)                                \
            {                                                       \
                if ( (__max) < (__size)*BYTE_SIZE)                  \
                {                                                   \
                  mask = mask >> (((__size)*BYTE_SIZE) - (__max));  \
                }                                                   \
            }                                                       \
            if ((__map)[i] != (uint8_t)mask)                        \
            {                                                       \
                break;                                              \
            }                                                       \
        }                                                           \
        if (i >= 0)                                                 \
        {                                                           \
            for (j=BYTE_SIZE-1; j>=0; j--)                          \
            {                                                       \
                (__pos) = (j+1) + (i*BYTE_SIZE);                    \
                if ((__pos) > __max)                                \
                {                                                   \
                    continue;                                       \
                }                                                   \
                if (((__map)[i] & (1 << j)) == 0)                   \
                {                                                   \
                    break;                                          \
                }                                                   \
            }                                                       \
        }                                                           \
    } while(0)

#define BRCM_SAI_MAC_IS_ZERO(_mac_) ((_mac_[0] | _mac_[1] | _mac_[2] | \
                                      _mac_[3] | _mac_[4] | _mac_[5]) == 0)
#define BRCM_SAI_MAC_IS_MCAST(_mac_) (_mac_[0] & 0x1)
#define BRCM_SAI_MAC_IS_BCAST(_mac_) ((_mac_[0] & _mac_[1] & _mac_[2] & \
                                       _mac_[3] & _mac_[4] & _mac_[5]) == 0xFF)

/*  Casting SAI vlan type to OPENNSL vlan type. Using this so that
    later if someone decides to change the size of vlan type in SAI,
    all we need to do is change this here.
 */
#define VLAN_CAST(__vlan_id) (opennsl_vlan_t)((__vlan_id))
#define VLAN_ID_CHECK(__vid) \
    ((_BRCM_SAI_VR_MAX_VID >= (__vid)) && (0 < (__vid)))

/*
################################################################################
#                                Global states                                 #
################################################################################
*/
extern service_method_table_t host_services;
extern sai_switch_notification_t host_callbacks;

extern const sai_switch_api_t switch_apis;
extern const sai_port_api_t port_apis;
extern const sai_fdb_api_t fdb_apis;
extern const sai_vlan_api_t vlan_apis;
extern const sai_virtual_router_api_t router_apis;
extern const sai_route_api_t route_apis;
extern const sai_next_hop_api_t next_hop_apis;
extern const sai_next_hop_group_api_t next_hop_grp_apis;
extern const sai_router_interface_api_t router_intf_apis;
extern const sai_neighbor_api_t neighbor_apis;
extern const sai_acl_api_t acl_apis;
extern const sai_hostif_api_t hostif_apis;
extern const sai_policer_api_t policer_apis;
extern const sai_qos_map_api_t qos_map_apis;
extern const sai_queue_api_t qos_apis;
extern const sai_wred_api_t wred_apis;
extern const sai_scheduler_api_t qos_scheduler_apis;
extern const sai_buffer_api_t buffer_apis;

#ifndef SAI_CLOSED_SOURCE
extern char *_brcm_sai_api_type_strings[];
#else
extern sai_log_level_t _adapter_log_level[];
extern sai_mac_t system_src_mac;
extern _brcm_sai_netif_map_t netif_map_port[];
extern _brcm_sai_netif_map_t netif_map_vlan[];
#endif

/*
################################################################################
#                                Public routines                               #
################################################################################
*/

/* Enum convertors */
extern sai_status_t BRCM_RV_OPENNSL_TO_SAI(int);
extern sai_status_t BRCM_STAT_SAI_TO_OPENNSL(sai_port_stat_counter_t sai_stat,
                                             opennsl_stat_val_t *stat);

/* All other routines */
extern bool _brcm_sai_api_is_inited(void);
extern bool _brcm_sai_switch_is_inited(void);
extern void _brcm_sai_link_event_cb(int unit, opennsl_port_t port,
                                    opennsl_port_info_t *info);
extern void _brcm_sai_fdb_event_cb(int unit, opennsl_l2_addr_t *l2addr,
                                   int operation, void *userdata);
extern sai_status_t _brcm_sai_alloc_vrf(int max);
extern void _brcm_sai_free_vrf(void);
extern bool _brcm_sai_vrf_valid(_In_ sai_uint32_t vr_id);
extern int _brcm_sai_vrf_info(sai_uint32_t vr_id, sai_mac_t *mac);
extern opennsl_if_t _brcm_sai_vrf_if_get(sai_uint32_t vr_id);
extern opennsl_if_t _brcm_sai_vrf_drop_if_get(sai_uint32_t vr_id);
extern bool _brcm_sai_vr_id_valid(sai_uint32_t index);
extern sai_status_t _brcm_sai_vlan_init();
extern sai_status_t _brcm_sai_rif_info_get(sai_uint32_t rif_id,
                                           sai_router_interface_type_t *type,
                                           sai_int32_t *port,
                                           sai_vlan_id_t *vlan,
                                           sai_mac_t *mac);
extern int _brcm_sai_storm_info_get(sai_object_id_t policer_id, bool *pkt_mode,
                                    int *cir, int *cbs);
extern opennsl_gport_t _brcm_sai_switch_port_queue_get(int port, int qid, int type);
extern int _brcm_sai_policer_action_info_connect(sai_object_id_t acl,
                                                 sai_object_id_t policer_id,
                                                 sai_packet_action_t *gpa,
                                                 sai_packet_action_t *ypa,
                                                 sai_packet_action_t *rpa);
extern void _brcm_sai_policer_action_ref_remove(sai_object_id_t acl);
extern sai_status_t _brcm_sai_scheduler_attach_object(_brcm_sai_qos_scheduler_t *scheduler,
                                                      sai_object_id_t object_id);
extern sai_status_t _brcm_sai_hostif_clean();
extern sai_status_t _brcm_sai_port_ingress_buffer_config_set(uint8_t port,
                                                             uint8_t pg,
                                                             uint8_t pool_id,
                                                             int max, int min,
                                                             int resume);
extern sai_status_t _brcm_sai_port_egress_buffer_config_set(uint8_t port,
                                                            uint8_t pool_id,
                                                            int mode, int limit,
                                                            int resume);
extern sai_status_t _brcm_sai_buffer_profile_get(sai_object_id_t buffer_profile_id,
                                                 _brcm_sai_buf_profile_t **buf_profile);
extern sai_status_t _brcm_sai_buffer_pool_get(sai_object_id_t buffer_pool_id,
                                              _brcm_sai_buf_pool_t **buf_pool);
extern sai_status_t _brcm_sai_egress_shared_limit_set(int pool_idx, int pool_size,
                                                      int bp_size);

/*
 * This should be last after all the public declarations
 */
#include "brcm_sai_private_apis.h"

#endif /* _BRM_SAI_COMMON */
