/*
* Copyright (c) 2014 Microsoft Open Technologies, Inc.
*
*    Licensed under the Apache License, Version 2.0 (the "License"); you may
*    not use this file except in compliance with the License. You may obtain
*    a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
*
*    THIS CODE IS PROVIDED ON AN  *AS IS* BASIS, WITHOUT WARRANTIES OR
*    CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
*    LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS
*    FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT.
*
*    See the Apache Version 2.0 License for specific language governing
*    permissions and limitations under the License.
*
*    Microsoft would like to thank the following companies for their review and
*    assistance with these files: Intel Corporation, Mellanox Technologies Ltd,
*    Dell Products, L.P., Facebook, Inc, Broadcom Corp.
*
* Module Name:
*
*    sail3tunnelintf.h
*
* Abstract:
*
*    This module defines SAI L3 TUNNEL INTERFACE API.
*
*/

#if !defined (__SAIL3TUNNELINTF_H_)
#define __SAIL3TUNNELINTF_H_

#include <saitypes.h>

/*
 * L3 tunnel egress interface type, i.e., the type of the encapped header
 */
typedef enum _sai_l3_tunnel_egress_interface_type_t
{
    /* Sai l3 tunnel egress interface IPv4 */
    SAI_L3_TUNNEL_EGRESS_INTERFACE_IPV4,

    /* Sai l3 tunnel egress interface IPv6 */
    SAI_L3_TUNNEL_EGRESS_INTERFACE_IPV6,

} sai_l3_tunnel_egress_interface_type_t;

/*
 *  Attribute id for l3 tunnel egress interface
 */
typedef enum _sai_l3_tunnel_egress_interface_attr_t
{
    /* READ-ONLY */

    /* L3 tunnel egress interface router interface. [sai_object_id_t]
     * (MANDATORY_ON_CREATE|CREATE_ONLY) */
    SAI_L3_TUNNEL_EGRESS_INTERFACE_ATTR_ROUTER_INTF,

    /* READ-WRITE */

    /* L3 tunnel egress interface type [sai_l3_tunnel_egress_interface_type_t]
     * (MANDATORY_ON_CREATE|CREATE_AND_SET) */
    SAI_L3_TUNNEL_EGRESS_INTERFACE_ATTR_TYPE,

    /* L3 tunnel egress interface source IP address.
     * This has to be coherent with the SAI_L3_TUNNEL_EGRESS_INTERFACE_ATTR_TYPE.
     * [sai_ip_address_t] (MANDATORY_ON_CREATE|CREATE_AND_SET) */
    SAI_L3_TUNNEL_EGRESS_INTERFACE_ATTR_SIP,

    /* L3 tunnel egress interface destination IP address.
     * This has to be coherent with the SAI_L3_TUNNEL_EGRESS_INTERFACE_ATTR_TYPE.
     * [sai_ip_address_t] (MANDATORY_ON_CREATE|CREATE_AND_SET) */
    SAI_L3_TUNNEL_EGRESS_INTERFACE_ATTR_DIP,

    /* L3 tunnel egress interface ttl [uint8_t] (CREATE_AND_SET) (default to 64) */
    SAI_L3_TUNNEL_EGRESS_INTERFACE_ATTR_TTL,

    /* L3 tunnel egress interface dscp [uint8_t] (CREATE_AND_SET) (default to 0) */
    SAI_L3_TUNNEL_EGRESS_INTERFACE_ATTR_DSCP,

    /* L3 tunnel egress interface IPv6 flow label
     * [uint32_t] (CREATE_AND_SET) (default to 0) */
    SAI_L3_TUNNEL_EGRESS_INTERFACE_ATTR_IPV6_FLOW_LABEL,

    /* -- */

    /* Custom range base value */
    SAI_L3_TUNNEL_EGRESS_INTERFACE_ATTR_CUSTOM_RANGE_BASE  = 0x10000000
    
} sai_l3_tunnel_egress_interface_attr_t;

/*
 * L3 tunnel ingress interface type, i.e., the type of the incoming encapped header
 */
typedef enum _sai_l3_tunnel_ingress_interface_type_t
{
    /* Sai l3 tunnel ingress interface IPv4 */
    SAI_L3_TUNNEL_INGRESS_INTERFACE_IPV4,

    /* Sai l3 tunnel ingress interface IPv6 */
    SAI_L3_TUNNEL_INGRESS_INTERFACE_IPV6,

} sai_l3_tunnel_ingress_interface_type_t;

/*
 *  Attribute id for l3 tunnel ingress interface
 */
typedef enum _sai_l3_tunnel_ingress_interface_attr_t
{
    /* READ-ONLY */

    /* READ-WRITE */
    /* L3 tunnel ingress interface type [sai_l3_tunnel_ingress_interface_type_t]
     * (MANDATORY_ON_CREATE|CREATE_ONLY) */
    SAI_L3_TUNNEL_INGRESS_INTERFACE_ATTR_TYPE,

    /* L3 tunnel ingress interface router interfaces list. [sai_object_list_t]
     * (MANDATORY_ON_CREATE|CREATE_AND_SET) */
    SAI_L3_TUNNEL_INGRESS_INTERFACE_ATTR_ROUTER_INTF_LIST,

    /* L3 tunnel ingress interface source IP address.
     * This has to be coherent with the SAI_L3_TUNNEL_INGRESS_INTERFACE_ATTR_TYPE.
     * [sai_ip_prefix_t] (MANDATORY_ON_CREATE|CREATE_AND_SET) */
    SAI_L3_TUNNEL_INGRESS_INTERFACE_ATTR_SIP_PREFIX,

    /* L3 tunnel ingress interface destination IP address.
     * This has to be coherent with the SAI_L3_TUNNEL_INGRESS_INTERFACE_ATTR_TYPE.
     * [sai_ip_prefix_t] (MANDATORY_ON_CREATE|CREATE_AND_SET) */
    SAI_L3_TUNNEL_INGRESS_INTERFACE_ATTR_DIP_PREFIX,

    /* L3 tunnel ingress interface dscp, whether to copy outer DSCP field to inner header.
     * [bool] (CREATE_AND_SET) (default to FALSE) */
    SAI_L3_TUNNEL_INGRESS_INTERFACE_ATTR_DSCP,

    /* L3 tunnel ingress interface virtual router
     * [sai_object_id_t] (CREATE_AND_SET) (default to SAI_NULL_OBJECT_ID) */
    SAI_L3_TUNNEL_INGRESS_INTERFACE_ATTR_VIRTUAL_ROUTER,

    /* -- */

    /* Custom range base value */
    SAI_L3_TUNNEL_INGRESS_INTERFACE_ATTR_CUSTOM_RANGE_BASE  = 0x10000000
    
} sai_l3_tunnel_ingress_interface_attr_t;

/*
 * Routine Description:
 *    Create l3 tunnel interface
 *
 * Arguments:
 *    [out] l3_tunnel_interface_id - l3 tunnel interface id
 *    [in] attr_count - number of attributes
 *    [in] attr_list - array of attributes
 *
 * Return Values:
 *    SAI_STATUS_SUCCESS on success
 *    Failure status code on error
 */
typedef sai_status_t (*sai_create_l3_tunnel_interface_fn)(
    _Out_ sai_object_id_t* l3_tunnel_interface_id,
    _In_ uint32_t attr_count,
    _In_ const sai_attribute_t *attr_list
    );

/*
 * Routine Description:
 *    Remove l3 tunnel interface
 *
 * Arguments:
 *    [in] l3_tunnel_interface_id - l3 tunnel interface id
 *
 * Return Values:
 *    SAI_STATUS_SUCCESS on success
 *    Failure status code on error
 */
typedef sai_status_t (*sai_remove_l3_tunnel_interface_fn)(
    _In_ sai_object_id_t l3_tunnel_interface_id
    );

/*
 * Routine Description:
 *    Set l3 tunnel interface attribute
 *
 * Arguments:
 *    [in] sai_object_id_t - l3_tunnel_interface_id
 *    [in] attr - attribute
 *
 * Return Values:
 *    SAI_STATUS_SUCCESS on success
 *    Failure status code on error
 */
typedef sai_status_t (*sai_set_l3_tunnel_interface_attribute_fn)(
    _In_ sai_object_id_t l3_tunnel_interface_id,
    _In_ const sai_attribute_t *attr
    );

/*
 * Routine Description:
 *    Get l3 tunnel interface attribute
 *
 * Arguments:
 *    [in] sai_object_id_t - l3_tunnel_interface_id
 *    [in] attr_count - number of attributes
 *    [inout] attr_list - array of attributes
 *
 * Return Values:
 *    SAI_STATUS_SUCCESS on success
 *    Failure status code on error
 */
typedef sai_status_t (*sai_get_l3_tunnel_interface_attribute_fn)(
    _In_ sai_object_id_t l3_tunnel_interface_id,
    _In_ uint32_t attr_count,
    _Inout_ sai_attribute_t *attr_list
    );

/*
 *  L3 tunnel methods table retrieved with sai_api_query()
 */
typedef struct _sai_l3_tunnel_interface_api_t
{
    sai_create_l3_tunnel_interface_fn         create_l3_tunnel_egress_interface;
    sai_remove_l3_tunnel_interface_fn         remove_l3_tunnel_egress_interface;
    sai_set_l3_tunnel_interface_attribute_fn  set_l3_tunnel_egress_interface_attribute;
    sai_get_l3_tunnel_interface_attribute_fn  get_l3_tunnel_egress_interface_attribute;
    sai_create_l3_tunnel_interface_fn         create_l3_tunnel_ingress_interface;
    sai_remove_l3_tunnel_interface_fn         remove_l3_tunnel_ingress_interface;
    sai_set_l3_tunnel_interface_attribute_fn  set_l3_tunnel_ingress_interface_attribute;
    sai_get_l3_tunnel_interface_attribute_fn  get_l3_tunnel_ingress_interface_attribute;

} sai_l3_tunnel_interface_api_t;

#endif // __SAIL3TUNNELINTF_H_
