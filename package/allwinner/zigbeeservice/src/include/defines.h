/**************************************************************************************************
 * Filename:       defines.h
 * Description:    Global definition.
 *
 */
#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <libubox/avl.h>
#include <stddef.h>
#include "ubus.h"

#define IEEE_ADDRLEN 8

#define LIGHT_NAME_LEN	32
#if defined (CLOUD_PLAT_JD)
	#define LIGHT_SERVER_LEN	30
	#define LIGHT_ACCESSKEY_LEN	32
	#define LIGHT_FEEDID_LEN	18
#endif

#define SET_STATE_FLAG_ON	0
#define SET_STATE_FLAG_BRI	1
#define SET_STATE_FLAG_HUE	2
#define SET_STATE_FLAG_SAT	3
#define SET_FLAG_GROUP_NAME	4
#define SET_FLAG_GROUP_MEM	5
#define SET_STATE_FLAG_COLORTEMP	6

#define GROUP_NAME_LEN		16

#define MAX_SUPPORTED_SCENE_NAME_LENGTH 32
#define MAX_SUPPORTED_SCENE_MEMBERS 20

#define MAX_SUPPORTED_RULE_MEMBERS 20

#define SCAN_FLAG_BIT	0
#define SERIAL_FLAG_BIT	1

/* 1 : gateway; 2 : light */
#define GW_DEVICE_TYPE	1
#define GW_PRODUCT_ID	"SZ12-ZLL"
#define GW_UCI_SERVER_CONFIG	"zll_sz"

#ifndef SET_INTERVAL
#define SET_INTERVAL		5
#endif

#define DUMMY_END_POINT	0xEE
#define DUMMY_SCENE_ID	0xEE

/*********************************************************************
 * 							MACROS
 */

/* Replacement for offsetof on different platforms.
 * */
#define sz_offsetof(type, field) offsetof(type, field)

/*	If we know that a given pointer points to a field in a
 * structure, return a pointer to the structure itself.
 * */
#define SZ_UPCAST(ptr, type, field)		\
	((type *)(((char *)(ptr)) - sz_offsetof(type, field)))


#define	avl_first_element_safe(tree, element, node_member)	\
	(avl_is_empty(tree) ? NULL : avl_first_element(tree, element, node_member))

#define avl_last_element_safe(tree, element, node_member)	\
	(avl_is_empty(tree) ? NULL : avl_last_element(tree, element, node_member))

#define avl_next_element_safe(tree, element, node_member)   \
	((element) == NULL || avl_is_last(tree, &(element)->node_member) ? NULL : avl_next_element(element, node_member))

#define avl_for_element_range_safe_sz(tree, first_element, last_element, element, node_member, ptr) \
  for (element = (first_element), ptr = avl_next_element_safe(tree, first_element, node_member); \
       (NULL != element) && (element->node_member.list.prev != &(last_element)->node_member.list); \
       element = ptr, ptr = avl_next_element_safe(tree, ptr, node_member))

#define avl_for_each_element_safe_sz(tree, element, node_member, ptr) \
	avl_for_element_range_safe_sz(tree, avl_first_element_safe(tree, element, node_member), \
	avl_last_element_safe(tree, element, node_member), element, node_member, ptr)

#define avl_for_each_element_safe(tree, element, node_member, ptr) \
	avl_for_element_range_safe(avl_first_element(tree, element, node_member), \
	avl_last_element(tree, element, node_member), element, node_member, ptr)

#define avl_is_last_safe(tree, element, node_member) ((element) == NULL || avl_is_last(tree, &(element)->node_member))

#endif //_DEFINES_H_
