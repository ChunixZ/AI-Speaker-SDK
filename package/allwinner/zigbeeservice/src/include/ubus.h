/*************************************************************************

    > File Name: ubus.h
    > Author: lunan
    > Mail: 6616@shuncom.com 
    > Created Time: 2015年05月20日 星期三 16时00分36秒
 ************************************************************************/

#ifndef _UBUS_H_
#define _UBUS_H_

#include <libubus.h>
#include <pthread.h>
#include "zha_strategy.h"

enum DEBUG_LEVEL{
	DEBUG_OFF = 0,
	DEBUG_DEBUG,
	DEBUG_GENERAL,
	DEBUG_WARN = 8,
	DEBUG_ERROR = 9,
	DEBUG_INPORTANT = 10,
	DEBUG_USER = 11
};


#define debug(level,format,...)		do{	\
										printf("[%s][%d]", __FILE__, __LINE__);	\
										printf(format, ##__VA_ARGS__);	\
										printf("\n");	\
									}while(0)
									

#define SHUNCOM_SDK_VER 			"v1.1.0"

extern const char date_time_format[];

extern struct ubus_context *ctx;

extern pthread_mutex_t mutex_ubus;

extern void ubus_init();

extern const struct blobmsg_policy rule_attrs[__RULE_ATTR_MAX] ;

extern const struct blobmsg_policy rule_cond_attrs[__RULE_ATTR_COND_MAX] ;

extern const struct blobmsg_policy rule_act_attrs[__RULE_ATTR_ACT_MAX];

extern const struct blobmsg_policy group_attrs[__GROUP_ATTR_MAX];

extern const struct blobmsg_policy scene_attrs[__SCENE_ATTR_MAX];
extern const struct blobmsg_policy real_time[__ZHA_REAL_TIME_MAX];
extern const struct blobmsg_policy cloud_debug[__ZHA_DEBUG_MAX];
extern const struct blobmsg_policy cloud_addr_changed[__CLOUD_ADDR_MAX];
extern const struct blobmsg_policy sha256[__SHA256_MAX];


#endif
