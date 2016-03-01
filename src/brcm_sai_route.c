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
#                             Forward declarations                             #
################################################################################
*/
STATIC sai_status_t
_brcm_sai_update_route(const sai_unicast_route_entry_t* unicast_route_entry,
                       sai_uint32_t attr_count,
                       const sai_attribute_t *attr_list);

/*
################################################################################
#                                  Route functions                             #
################################################################################
*/

/*
* Routine Description:
*    Create Route
*
* Arguments:
*    [in] unicast_route_entry - route entry
*    [in] attr_count - number of attributes
*    [in] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/

sai_status_t
brcm_sai_create_route(_In_ const sai_unicast_route_entry_t* unicast_route_entry,
                      _In_ sai_uint32_t attr_count,
                      _In_ const sai_attribute_t *attr_list)
{
    int i;
    sai_status_t rv;
    opennsl_if_t l3_if_id = -1;
    opennsl_l3_route_t l3_rt;
    bool trap = false,  drop = false, copy_to_cpu = false;
    sai_uint32_t vr_id;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_ROUTE);
    BRCM_SAI_SWITCH_INIT_CHECK;
    BRCM_SAI_OBJ_CREATE_PARAM_CHK(unicast_route_entry);

    opennsl_l3_route_t_init(&l3_rt);
    for (i=0; i<attr_count; i++)
    {
        switch (attr_list[i].id)
        {
            case SAI_ROUTE_ATTR_NEXT_HOP_ID:
                l3_if_id = BRCM_SAI_GET_OBJ_VAL(opennsl_if_t,
                                                BRCM_SAI_ATTR_LIST_OBJ(i));
                if (SAI_OBJECT_TYPE_NEXT_HOP_GROUP ==
                    BRCM_SAI_GET_OBJ_TYPE(BRCM_SAI_ATTR_LIST_OBJ(i)))
                {
                    l3_rt.l3a_flags |= OPENNSL_L3_MULTIPATH;
                }
                break;
            case SAI_ROUTE_ATTR_PACKET_ACTION:
                if (SAI_PACKET_ACTION_LOG == attr_list[i].value.u32)
                {
                    copy_to_cpu = true;
                }
                else if (SAI_PACKET_ACTION_TRAP == attr_list[i].value.u32)
                {
                    trap = true;
                    l3_rt.l3a_flags |= OPENNSL_L3_DEFIP_CPU;
                }
                else if (SAI_PACKET_ACTION_DROP == attr_list[i].value.u32)
                {
                    drop = true;
                    l3_rt.l3a_flags |= OPENNSL_L3_DST_DISCARD;
                }
                else
                {
                    BRCM_SAI_LOG_ROUTE(SAI_LOG_ERROR, "Bad attribute passed\n");
                    return SAI_STATUS_INVALID_PARAMETER;
                }
                break;
            default:
                BRCM_SAI_LOG_ROUTE(SAI_LOG_ERROR, "Unimplemented attribute passed\n");
                break;
        }
    }
    if (-1 == l3_if_id && FALSE == trap && FALSE == drop)
    {
        BRCM_SAI_LOG_ROUTE(SAI_LOG_ERROR, "Missing routing info.\n");
                           return SAI_STATUS_INVALID_PARAMETER;
    }
    if (SAI_IP_ADDR_FAMILY_IPV4 == unicast_route_entry->destination.addr_family)
    {
        l3_rt.l3a_ip_mask = ntohl(unicast_route_entry->destination.mask.ip4);
        l3_rt.l3a_subnet = ntohl(unicast_route_entry->destination.addr.ip4 &
                                 unicast_route_entry->destination.mask.ip4);
    }
    else if (SAI_IP_ADDR_FAMILY_IPV6 ==
             unicast_route_entry->destination.addr_family)
    {
        memcpy(l3_rt.l3a_ip6_net, unicast_route_entry->destination.addr.ip6,
               sizeof(l3_rt.l3a_ip6_net));
        memcpy(l3_rt.l3a_ip6_mask, unicast_route_entry->destination.mask.ip6,
               sizeof(l3_rt.l3a_ip6_mask));
    }
    else
    {
        BRCM_SAI_LOG_ROUTE(SAI_LOG_ERROR, "Bad address family passed\n");
        return SAI_STATUS_INVALID_PARAMETER;
    }
    vr_id = BRCM_SAI_GET_OBJ_VAL(sai_uint32_t, unicast_route_entry->vr_id);
    if (false == _brcm_sai_vr_id_valid(vr_id))
    {
        BRCM_SAI_LOG_ROUTE(SAI_LOG_ERROR,
                           "Invalid VR id passed during route create %d\n",
                           vr_id);
        return SAI_STATUS_INVALID_PARAMETER;
    }

    l3_rt.l3a_vrf = vr_id;
    if (trap)
    {
        l3_rt.l3a_intf = _brcm_sai_vrf_if_get(l3_rt.l3a_vrf);
    }
    else if (drop)
    {
        l3_rt.l3a_intf = _brcm_sai_vrf_drop_if_get(l3_rt.l3a_vrf);
    }
    else
    {
        l3_rt.l3a_intf = l3_if_id;

        if (TRUE == copy_to_cpu)
        {
          opennsl_l3_egress_t l3_egr;
          uint32 flags = OPENNSL_L3_REPLACE | OPENNSL_L3_WITH_ID;

          rv = opennsl_l3_egress_get(0, l3_if_id, &l3_egr);
          BRCM_SAI_API_CHK(SAI_API_ROUTE, "L3 egress get", rv);

          l3_egr.flags |= OPENNSL_L3_COPY_TO_CPU;
          rv = opennsl_l3_egress_create(0, flags, &l3_egr, &l3_if_id);
          BRCM_SAI_API_CHK(SAI_API_ROUTE, "L3 egress create w/ replace", rv);
        }
    }
    BRCM_SAI_LOG_ROUTE(SAI_LOG_DEBUG,
                       "Add route vrf: %d, egr %s id: %d mask 0x%x, subnet 0x%x\n",
                       l3_rt.l3a_vrf,
                       !(l3_rt.l3a_flags & OPENNSL_L3_MULTIPATH) ? "nh" : "nhg",
                       l3_rt.l3a_intf,
                       l3_rt.l3a_ip_mask,
                       l3_rt.l3a_subnet );
    rv = opennsl_l3_route_add(0, &l3_rt);
    BRCM_SAI_API_CHK(SAI_API_ROUTE, "L3 route add", rv);

    BRCM_SAI_FUNCTION_EXIT(SAI_API_ROUTE);

    return rv;
}

/*
* Routine Description:
*    Remove Route
*
* Arguments:
*    [in] unicast_route_entry - route entry
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
sai_status_t
brcm_sai_remove_route(_In_ const sai_unicast_route_entry_t* unicast_route_entry)
{
    sai_status_t rv;
    opennsl_l3_route_t l3_rt;
    sai_uint32_t vr_id;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_ROUTE);
    BRCM_SAI_SWITCH_INIT_CHECK;

    if (NULL == unicast_route_entry)
    {
        BRCM_SAI_LOG_ROUTE(SAI_LOG_ERROR, "NULL route passed\n");
        return SAI_STATUS_INVALID_PARAMETER;
    }

    opennsl_l3_route_t_init(&l3_rt);
    vr_id = BRCM_SAI_GET_OBJ_VAL(sai_uint32_t, unicast_route_entry->vr_id);
    l3_rt.l3a_vrf = vr_id;
    if (SAI_IP_ADDR_FAMILY_IPV4 == unicast_route_entry->destination.addr_family)
    {
        l3_rt.l3a_ip_mask = ntohl(unicast_route_entry->destination.mask.ip4);
        l3_rt.l3a_subnet = ntohl(unicast_route_entry->destination.addr.ip4 &
                                 unicast_route_entry->destination.mask.ip4);
    }
    else if (SAI_IP_ADDR_FAMILY_IPV6 ==
             unicast_route_entry->destination.addr_family)
    {
        memcpy(l3_rt.l3a_ip6_net, unicast_route_entry->destination.addr.ip6,
               sizeof(l3_rt.l3a_ip6_net));
        memcpy(l3_rt.l3a_ip6_mask, unicast_route_entry->destination.mask.ip6,
               sizeof(l3_rt.l3a_ip6_mask));
    }
    rv = opennsl_l3_route_delete(0, &l3_rt);
    BRCM_SAI_API_CHK(SAI_API_ROUTE, "L3 route delete", rv);

    BRCM_SAI_FUNCTION_EXIT(SAI_API_ROUTE);

    return rv;
}

/*
* Routine Description:
*    Set route attribute value
*
* Arguments:
*    [in] unicast_route_entry - route entry
*    [in] attr - attribute
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
sai_status_t
brcm_sai_set_route_attribute(_In_ const sai_unicast_route_entry_t*
                                unicast_route_entry,
                             _In_ const sai_attribute_t *attr)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_ROUTE);
    BRCM_SAI_SWITCH_INIT_CHECK;

    if ((NULL == unicast_route_entry) || (NULL == attr))
    {
        BRCM_SAI_LOG_ROUTE(SAI_LOG_ERROR, "NULL params passed\n");
        return SAI_STATUS_INVALID_PARAMETER;
    }

    rv = _brcm_sai_update_route(unicast_route_entry, 1, attr);

    BRCM_SAI_FUNCTION_EXIT(SAI_API_ROUTE);

    return rv;
}

/*
* Routine Description:
*    Get route attribute value
*
* Arguments:
*    [in] unicast_route_entry - route entry
*    [in] attr_count - number of attributes
*    [inout] attr_list - array of attributes
*
* Return Values:
*    SAI_STATUS_SUCCESS on success
*    Failure status code on error
*/
sai_status_t
brcm_sai_get_route_attribute(_In_ const sai_unicast_route_entry_t*
                                unicast_route_entry,
                             _In_ sai_uint32_t attr_count,
                             _Inout_ sai_attribute_t *attr_list)
{
    sai_status_t rv = SAI_STATUS_NOT_IMPLEMENTED;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_ROUTE);
    BRCM_SAI_SWITCH_INIT_CHECK;

    BRCM_SAI_FUNCTION_EXIT(SAI_API_ROUTE);

    return rv;
}

/*
################################################################################
#                               Internal functions                             #
################################################################################
*/
sai_status_t
_brcm_sai_update_route(const sai_unicast_route_entry_t* unicast_route_entry,
                       sai_uint32_t attr_count,
                       const sai_attribute_t *attr_list)
{
    int i;
    sai_status_t rv;
    opennsl_if_t l3_if_id = -1;
    opennsl_l3_route_t l3_rt;
    bool trap = false, drop = false;
    sai_uint32_t vr_id;

    BRCM_SAI_FUNCTION_ENTER(SAI_API_ROUTE);
    BRCM_SAI_SWITCH_INIT_CHECK;

    if ((NULL == unicast_route_entry) || (NULL == attr_list) || (0 == attr_count))
    {
        BRCM_SAI_LOG_ROUTE(SAI_LOG_ERROR, "NULL params passed\n");
        return SAI_STATUS_INVALID_PARAMETER;
    }
    opennsl_l3_route_t_init(&l3_rt);
    for (i=0; i<attr_count; i++)
    {
        switch (attr_list[i].id)
        {
            case SAI_ROUTE_ATTR_NEXT_HOP_ID:
                l3_if_id = attr_list[i].value.u32;
                if (SAI_OBJECT_TYPE_NEXT_HOP_GROUP ==
                    BRCM_SAI_GET_OBJ_TYPE(BRCM_SAI_ATTR_LIST_OBJ(i)))
                {
                    l3_rt.l3a_flags |= OPENNSL_L3_MULTIPATH;
                }
                break;
            case SAI_ROUTE_ATTR_PACKET_ACTION:
                if (SAI_PACKET_ACTION_LOG == attr_list[i].value.u32)
                {
                    l3_rt.l3a_flags |= OPENNSL_L3_COPY_TO_CPU;
                }
                else if (SAI_PACKET_ACTION_TRAP == attr_list[i].value.u32)
                {
                    trap = true;
                    l3_rt.l3a_flags |= OPENNSL_L3_DEFIP_CPU;
                }
                else if (SAI_PACKET_ACTION_DROP == attr_list[i].value.u32)
                {
                    drop = true;
                    l3_rt.l3a_flags |= OPENNSL_L3_DST_DISCARD;
                }
                else
                {
                    BRCM_SAI_LOG_ROUTE(SAI_LOG_ERROR, "Bad attribute passed\n");
                    return SAI_STATUS_INVALID_PARAMETER;
                }
            default:
                BRCM_SAI_LOG_ROUTE(SAI_LOG_INFO, "Un-supported attribute %d passed\n",
                                   attr_list[i].id);
                break;
        }
    }
    if (-1 == l3_if_id && FALSE == trap && FALSE == drop)
    {
        BRCM_SAI_LOG_ROUTE(SAI_LOG_ERROR, "Missing routing info.\n");
                           return SAI_STATUS_INVALID_PARAMETER;
    }
    if (SAI_IP_ADDR_FAMILY_IPV4 == unicast_route_entry->destination.addr_family)
    {
        l3_rt.l3a_ip_mask = ntohl(unicast_route_entry->destination.mask.ip4);
        l3_rt.l3a_subnet = ntohl(unicast_route_entry->destination.addr.ip4 &
                                 unicast_route_entry->destination.mask.ip4);
    }
    else if (SAI_IP_ADDR_FAMILY_IPV6 ==
             unicast_route_entry->destination.addr_family)
    {
        memcpy(l3_rt.l3a_ip6_net, unicast_route_entry->destination.addr.ip6,
               sizeof(l3_rt.l3a_ip6_net));
        memcpy(l3_rt.l3a_ip6_mask, unicast_route_entry->destination.mask.ip6,
               sizeof(l3_rt.l3a_ip6_mask));
    }
    else
    {
        return SAI_STATUS_INVALID_PARAMETER;
    }
    vr_id = BRCM_SAI_GET_OBJ_VAL(sai_uint32_t, unicast_route_entry->vr_id);
    if (false == _brcm_sai_vr_id_valid(vr_id))
    {
        BRCM_SAI_LOG_ROUTE(SAI_LOG_ERROR,
                           "Invalid VR id passed during route create %d\n",
                           vr_id);
        return SAI_STATUS_INVALID_PARAMETER;
    }

    l3_rt.l3a_vrf = vr_id;
    if (trap)
    {
        l3_rt.l3a_intf = _brcm_sai_vrf_if_get(l3_rt.l3a_vrf);
    }
    else if (drop)
    {
        l3_rt.l3a_intf = _brcm_sai_vrf_drop_if_get(l3_rt.l3a_vrf);
    }
    else
    {
        l3_rt.l3a_intf = l3_if_id;
    }

    l3_rt.l3a_flags |= OPENNSL_L3_REPLACE;

    BRCM_SAI_LOG_ROUTE(SAI_LOG_DEBUG, "Update route vrf: %d, egr %s id: %d\n",
                       l3_rt.l3a_vrf,
                       !(l3_rt.l3a_flags & OPENNSL_L3_MULTIPATH) ? "nh" : "nhg",
                       l3_rt.l3a_intf);
    rv = opennsl_l3_route_add(0, &l3_rt);
    BRCM_SAI_API_CHK(SAI_API_ROUTE, "L3 route add", rv);

    BRCM_SAI_FUNCTION_EXIT(SAI_API_ROUTE);

    return rv;
}

/*
################################################################################
#                                Functions map                                 #
################################################################################
*/
const sai_route_api_t route_apis = {
    brcm_sai_create_route,
    brcm_sai_remove_route,
    brcm_sai_set_route_attribute,
    brcm_sai_get_route_attribute
};
