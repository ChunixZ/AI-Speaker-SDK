/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 *
 * Alibaba Group retains all right, title and interest (including all
 * intellectual property rights) in and to this computer program, which is
 * protected by applicable intellectual property laws.  Unless you have
 * obtained a separate written license from Alibaba Group., you are not
 * authorized to utilize all or a part of this computer program for any
 * purpose (including reproduction, distribution, modification, and
 * compilation into object code), and you must immediately destroy or
 * return to Alibaba Group all copies of this computer program.  If you
 * are licensed by Alibaba Group, your rights to utilize this computer
 * program are limited by the terms of that license.  To obtain a license,
 * please contact Alibaba Group.
 *
 * This computer program contains trade secrets owned by Alibaba Group.
 * and, unless unauthorized by Alibaba Group in writing, you agree to
 * maintain the confidentiality of this computer program and related
 * information and to not disclose this computer program and related
 * information to any other person or entity.
 *
 * THIS COMPUTER PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTIES, AND
 * Alibaba Group EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * INCLUDING THE WARRANTIES OF MERCHANTIBILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, TITLE, AND NONINFRINGEMENT.
 */

/* header file for alink zigbee device */
#ifndef _ALINK_EXPORT_ZIGBEE_H_
#define _ALINK_EXPORT_ZIGBEE_H_

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

/** @defgroup zigbee_device zigbee API
 *  @{
 */

/** @defgroup zigbee_struct struct
 *  @{
 */


/**
 * command argument
 */
typedef struct cmd_argument {
	char arg_name[32];
	unsigned char data_type;
    unsigned int mask_code; /*only used for argument of bitmap type*/
    struct cmd_argument *next_arg;
    struct cmd_argument *child_argument;/*only used for argument of bitmap type*/
} cmd_argument_t;

/**
 * attr element
 */
typedef struct attr_element {
    char element_name[32];
    unsigned char data_type;/**< defined by zigbee protocol */
    unsigned int mask_code; /*only used for attribute of bitmap type*/
    struct attr_element *next_element;
    struct attr_element *child_element;
} attr_element_t;


/**
 * alink attribute profile description
 */
typedef struct attr_profile {
    char attr_name[32];
    unsigned char data_type;
    unsigned short cluster_id;
    unsigned short attribute_id;
    attr_element_t *attr_element;
    unsigned char reserved[4];
} attr_profile_t;

/**
 * alink command profile description
 */
typedef struct cmd_profile{
    char cmd_name[32];
    unsigned short cluster_id;
    unsigned char cmd_id;
    unsigned char direction; /**< 0:c to s; 1:s to c */
    cmd_argument_t *args;
    unsigned char reserved[4];
} cmd_profile_t;

/** @} */ //end of key struct

/** @defgroup zigbee_status register & report status
 *  @{
 */

/**
 * @brief register and authentication zigbee device to aliyun server
 *
 * @param[in] ieee_addr: 8 bytes zigbee device long address
 * @param[in] model_id: 2 bytes device model identification
 * @param[in] rand: 16 bytes random characters string, end with '\0'
 * @param[in] sign: 16 bytes characters of signature, end with '\0', md5(rand|secret)
 * @retval 0 on success, -1 when params invalid
 * @see None.
 * @note when joined zigbee network, invoke this function to register sub device
 */
int alink_zigbee_register_device(unsigned char ieee_addr[8],
        unsigned int model_id, const char *rand, const char *sign);


/**
 * @brief unregister zigbee device to aliyun server
 *
 * @param[in] ieee_addr: 8 bytes zigbee device long address
 * @retval 0 on success, -1 when params invalid
 * @see None.
 * @note when factory reset, invoke this function to unregister sub device
 */
int alink_zigbee_unregister_device(unsigned char ieee_addr[8]);

/**
 * @brief update zigbee device online status, online or offline
 *
 * @param[in] ieee_addr: 8 bytes zigbee device long address
 * @param[in] online_or_not: 1 online; 0 offline
 *
 * @retval 0 on success, -1 when params invalid
 * @see None.
 * @note alink sdk will keep syncing subdev status with aliyun server
 */
int alink_zigbee_update_online_status(unsigned char ieee_addr[8], char online_or_not);

/**
 * @brief report zigbee device attribute list to server, at least 1 attribute should be include
 *
 * @param[in] ieee_addr: zigbee device's long address
 * @param[in] endpoint_id: zigbee device's endpoint
 * @param[in] attr_name[]: NULL-terminated array which hold the attr name list
 * @param[in] attr_value[]: NULL-terminated array which hold the attr value list,
 *            attr_name[x] & attr_value[x] is a key-value pair.
 * @retval 0 when successfully got response from cloud,
 *          otherwise this func will block until timeout and -1 will return
 * @see None.
 * @note when connection with server is unstable, this func will block
 *      until got response from server or timeout.
 */
int alink_zigbee_report_attrs(unsigned char ieee_addr[8], unsigned char endpoint_id,
        const char *attr_name[], const char *attr_value[]);

/** @} */ //end of register & report status

/** @defgroup zigbee_callback callback
 *  @{
 */

/**
 * zigbee event, used by alink_register_callback function
 */
enum ALINK_ZIGBEE_CALLBACK {
    /**
     * int callback_zigbee_get_attr(unsigned char ieee_addr[8], unsigned char endpoint_id,
     *                  const char *attr_name);
     */
    ALINK_ZIGBEE_GET_DEVICE_STATUS = 30,

    /**
     * int callback_zigbee_set_attr(unsigned char ieee_addr[8], unsigned char endpoint_id,
     *                  const char *attr_name, const char *attr_value);
     */
    ALINK_ZIGBEE_SET_DEVICE_STATUS,

    /**
     * int callback_zigbee_execute_cmd(unsigned char ieee_addr[8],
     *                  unsigned char endpoint_id, const char *cmd_name,
     *                  const char *cmd_args);
     *
     * @brief execute zigbee device cmd(service)
     *
     * @param[in] ieee_addr: zigbee ieee address
     * @param[in] endpoint_id: zigbee port
     * @param[in] cmd_name: command name
     * @param[in] cmd_args: command params, json format
     * @retval  0 on success, otherwise -1 will return
     */
    ALINK_ZIGBEE_EXECUTE_DEVICE_CMD,

    /**
     * int callback_zigbee_update_attr_profile(st_attr_profile *profile[]);
     *
     * @brief update gateway zigbee sub device's attr profile
     * @param[in] profile: pointer array of device attr profile struct,pointer array end with NULL.
     * @retval  0 on success, otherwise -1 will return
     * @see None.
     * @note None.
     */
    ALINK_ZIGBEE_UPDATE_ATTR_PROFILE,

    /**
     * int callback_zigbee_update_cmd_profile(st_cmd_profile *profile[]);
     *
     * @brief update gateway zigbee sub device's cmd profile
     * @param[in] profile: pointer array of device cmd profile struct,pointer array end with NULL.
     * @retval  0 on success, otherwise -1 will return
     * @see None.
     * @note None.
     */
    ALINK_ZIGBEE_UPDATE_CMD_PROFILE,

    /**
     * int callback_zigbee_remove_device(unsigned char ieee_addr[8]);
     *
     * @brief remove zigbee sub device's from zigbee network
     * @param[in] ieee_addr: zigbee ieee address
     * @retval  0 on success, otherwise -1 will return
     * @see None.
     * @note None.
     */
    ALINK_ZIGBEE_REMOVE_DEVICE,

	/**
     * int callback_zigbee_permit_join(uint8_t duration);
     *
     * @brief remove zigbee sub device's from zigbee network
     * @param[in] duration: permit join duration time, unit: second
	 * 					duration: 0, disable join; 255, enable join forever
     * @retval  0 on success, otherwise -1 will return
     * @see None.
     * @note None.
     */
    ALINK_ZIGBEE_PERMIT_JOIN,

};

/** @} */ //end of callback
/** @} */ //end of zigbee export api

#if defined(__cplusplus) /* If this is a C++ compiler, use C linkage */
}
#endif

#endif
