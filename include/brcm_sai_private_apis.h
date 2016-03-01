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
#if !defined (BRCM_SAI_PRIVATE_APIS_H)
#define BRCM_SAI_PRIVATE_APIS_H

extern bool _brcm_sai_switch_is_inited(void) LIB_DLL_EXPORTED;

extern void _brcm_sai_switch_init_set(bool init) LIB_DLL_EXPORTED;

extern bool
sai_log_check(_In_ sai_api_t sai_api_id, _In_ sai_log_level_t log_level) LIB_DLL_EXPORTED;

extern sai_status_t
_sai_log_set(_In_ sai_api_t sai_api_id, _In_ sai_log_level_t log_level) LIB_DLL_EXPORTED;

extern sai_status_t _brcm_sai_32bit_size_check(sai_meter_type_t meter_type,
                                               sai_uint64_t test_var) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_initialize_switch(void) LIB_DLL_EXPORTED;

extern sai_mac_t *
_brcm_sai_switch_system_mac_get(void) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_switch_system_mac_set(_In_ const sai_mac_t src_mac) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_mmu_gport_init(void) LIB_DLL_EXPORTED;

extern void _brcm_sai_clear_port_state(void) LIB_DLL_EXPORTED;

extern opennsl_vlan_t _brcm_sai_get_max_unused_vlan_id(void) LIB_DLL_EXPORTED;

extern bool _brcm_sai_vlan_exists(_In_ sai_vlan_id_t vlan_id) LIB_DLL_EXPORTED;

extern void
_brcm_sai_vlan_bmp_init(_In_ sai_vlan_id_t vlan_id) LIB_DLL_EXPORTED;

extern void
_brcm_sai_vlan_bmp_set(sai_vlan_id_t vlan_id) LIB_DLL_EXPORTED;

extern void
_brcm_sai_vlan_bmp_clear(sai_vlan_id_t vlan_id) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_set_ingress_priority_group_attr(_In_ sai_object_id_t ingress_pg_id,
                                          _In_ const sai_attribute_t *attr) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_create_buffer_pool(_Out_ sai_object_id_t* pool_id,
                             _In_ uint32_t attr_count,
                             _In_ const sai_attribute_t *attr_list) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_create_buffer_profile(_Out_ sai_object_id_t* buffer_profile_id,
                                _In_ uint32_t attr_count,
                                _In_ const sai_attribute_t *attr_list) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_create_acl_table(_Out_ sai_object_id_t* acl_table_id,
                           _In_ uint32_t attr_count,
                           _In_ const sai_attribute_t *attr_list) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_delete_acl_table(_In_ sai_object_id_t acl_table_id) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_set_acl_table_attribute(_In_ sai_object_id_t acl_table_id,
                                  _In_ const sai_attribute_t *attr) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_create_acl_entry(_Out_ sai_object_id_t *acl_entry_id,
                           _In_ uint32_t attr_count,
                           _In_ const sai_attribute_t *attr_list) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_delete_acl_entry(_In_ sai_object_id_t acl_entry_id) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_set_acl_entry_attribute(_In_ sai_object_id_t acl_entry_id,
                                  _In_ const sai_attribute_t *attr) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_create_acl_counter(_Out_ sai_object_id_t *acl_counter_id,
                             _In_ uint32_t attr_count,
                             _In_ const sai_attribute_t *attr_list) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_delete_acl_counter(_In_ sai_object_id_t acl_counter_id) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_get_acl_counter_attribute(_In_ sai_object_id_t acl_counter_id,
                                    _In_ uint32_t attr_count,
                                    _Out_ sai_attribute_t *attr_list) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_create_host_interface(_Out_ sai_object_id_t* hif_id,
                                _In_ uint32_t attr_count,
                                _In_ const sai_attribute_t *attr_list) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_remove_host_interface(_In_ sai_object_id_t hif_id) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_set_trap_attribute(_In_ sai_hostif_trap_id_t hostif_trapid,
                             _In_ const sai_attribute_t *attr) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_create_policer(_Out_ sai_object_id_t *policer_id,
                         _In_ uint32_t attr_count,
                         _In_ const sai_attribute_t *attr_list) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_remove_policer(_In_ sai_object_id_t policer_id) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_set_port_attribute(_In_ sai_object_id_t port_id,
                             _In_ const sai_attribute_t *attr) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_get_port_attribute(_In_ sai_object_id_t port_id,
                             _In_ uint32_t attr_count,
                             _Inout_ sai_attribute_t *attr_list) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_create_qos_map(_Out_ sai_object_id_t* qos_map_id,
                         _In_ uint32_t attr_count,
                         _In_ const sai_attribute_t *attr_list) LIB_DLL_EXPORTED;
extern sai_status_t
_brcm_sai_set_queue_attribute(_In_ sai_object_id_t queue_id,
                              _In_ const sai_attribute_t *attr) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_get_queue_stats(_In_ sai_object_id_t queue_id,
                          _In_ const sai_queue_stat_counter_t *counter_ids,
                          _In_ uint32_t number_of_counters,
                          _Out_ uint64_t* counters) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_create_scheduler_profile(_Out_ sai_object_id_t *scheduler_id,
                                   _In_ uint32_t attr_count,
                                   _In_ const sai_attribute_t *attr_list) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_remove_scheduler_profile(_In_ sai_object_id_t scheduler_id,
                                   sai_set_port_attribute_fn set_port_attribute,
                                   sai_set_queue_attribute_fn set_queue_attribute) LIB_DLL_EXPORTED;

extern _brcm_sai_qos_scheduler_t *_brcm_sai_scheduler_get(int id) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_vrf_max_get(_Out_ int *max) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_virtual_router_flags_get(_Out_ sai_uint32_t *flags) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_create_router_interface(_Out_ sai_object_id_t* rif_id,
                                  _In_ sai_uint32_t attr_count,
                                  _In_ sai_attribute_t *attr_list) LIB_DLL_EXPORTED;

extern void
_brcm_sai_rif_info_set(sai_uint32_t rif_id,
                       sai_router_interface_type_t type,
                       sai_int32_t port, sai_vlan_id_t vlan,
                       sai_mac_t mac) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_rif_info_get(sai_uint32_t rif_id,
                       sai_router_interface_type_t *type,
                       sai_int32_t *port, sai_vlan_id_t *vlan,
                       sai_mac_t *mac) LIB_DLL_EXPORTED;

extern sai_status_t _brcm_sai_alloc_rif(void) LIB_DLL_EXPORTED;

extern void _brcm_sai_free_rif(void) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_create_neighbor_entry(_In_ const sai_neighbor_entry_t* neighbor_entry,
                                _In_ uint32_t attr_count,
                                _In_ const sai_attribute_t *attr_list) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_remove_neighbor_entry(_In_ const sai_neighbor_entry_t* neighbor_entry) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_create_next_hop(_Out_ sai_object_id_t* next_hop_id,
                          _In_ uint32_t attr_count,
                          _In_ const sai_attribute_t *attr_list) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_get_switch_attribute(_In_ uint32_t attr_count,
                               _Inout_ sai_attribute_t *attr_list) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_add_ports_to_vlan(_In_ sai_vlan_id_t vlan_id,
                            _In_ uint32_t port_count,
                            _In_ const sai_vlan_port_t* port_list) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_create_wred(_Out_ sai_object_id_t *wred_id,
                      _In_ uint32_t attr_count,
                      _In_ const sai_attribute_t *attr_list) LIB_DLL_EXPORTED;

extern sai_status_t
_brcm_sai_remove_wred(_In_ sai_object_id_t  wred_id) LIB_DLL_EXPORTED;

#endif  /* BRCM_SAI_PRIVATE_APIS_H */
