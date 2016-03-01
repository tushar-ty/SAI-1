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

#if !defined (_BRM_SAI_CUSTOM_ATTR)
#define _BRM_SAI_CUSTOM_ATTR

/*
################################################################################
#                            Custom switch attributes                          #
################################################################################
*/
#define SAI_SWITCH_ATTR_BRCM_CUSTOM_SWITCH_START     ((sai_attr_id_t)0x10000000)
#define SAI_SWITCH_ATTR_BRCM_SWITCH_SHELL_ENABLE     ((sai_attr_id_t)0x10000001)
#define SAI_SWITCH_ATTR_BRCM_CUSTOM_SWITCH_END       ((sai_attr_id_t)0x1000ffff)

#endif /* _BRM_SAI_CUSTOM_ATTR */
