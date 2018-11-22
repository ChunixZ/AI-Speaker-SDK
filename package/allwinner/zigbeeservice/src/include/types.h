/*************************************************************************

    > File Name: types.h
    > Author: lunan
    > Mail: 6616@shuncom.com 
    > Created Time: 2015年05月14日 星期四 11时40分25秒
 ************************************************************************/

#ifndef _TYPES_H_
#define _TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
 * Includes
 *****************************************************************************/
#include <stdio.h>
#include "stdint.h"
#include "stdbool.h"
#include "libubox/uloop.h"
#include "libubox/avl.h"
#if 0//#ifdef ALINK
#include "zap_interface.h"
#include "zcl.h"
#endif
#include "user_types.h"

/**************************************************************************
* constants
*************************************************************************/

#define MAX_VALID_ATTRIBUTES		20
#define MAX_ENDPOINTS		10
#define MAX_CLUSTERS_ON_EP	10
#define MAX_PENDING_ATTRIBUTES 16
#define MAX_DEVICES_PER_FILE   10

#define ZIGBEE_NETWORK_STATE_UNAVAILABLE  0
#define ZIGBEE_NETWORK_STATE_INITIALIZING 1
#define ZIGBEE_NETWORK_STATE_READY        2

#define NOT_NULL	((void *)1)

#define MAX_DEVICE_INFO_MSG_LEN		10
/******************************************************************************
 * Types
 *****************************************************************************/

/*********************************************************************
 * MACROS
 */
#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)

/*
#define BUILD_UINT16(loByte, hiByte) \
          ((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#define BUILD_UINT32(Byte0, Byte1, Byte2, Byte3) \
          ((uint32_t)((uint32_t)((Byte0) & 0xFF) \
          + ((uint32_t)((Byte1) & 0x00FF) << 8) \
          + ((uint32_t)((Byte2) & 0x0000FF) << 16) \
          + ((uint32_t)((Byte3) & 0x000000FF) << 24)))

 */
#define BUILD_UINT64(Byte0, Byte1, Byte2, Byte3, Byte4, Byte5, Byte6, Byte7) \
          ((uint64_t)((uint64_t)((Byte0) & 0xFF) \
          + ((uint64_t)((Byte1) & 0x00FF) << 8) \
          + ((uint64_t)((Byte2) & 0x0000FF) << 16) \
          + ((uint64_t)((Byte3) & 0x000000FF) << 24) \
          + ((uint64_t)((Byte4) & 0x00000000FF) << 32) \
          + ((uint64_t)((Byte5) & 0x0000000000FF) << 40) \
          + ((uint64_t)((Byte6) & 0x000000000000FF) << 48) \
          + ((uint64_t)((Byte7) & 0x00000000000000FF) << 56)))

#define Z_EXTADDR_LEN      8 


#ifndef ARM_ANDROID
#define debug_print(param...)   do{  \
                      if(debugLevel != 0)  \
                      {  \
                          printf(param);\
                      }  \
                    }while(0)
#else
#include <android/log.h>
#define debug_print(param...)  ((void)__android_log_print(ANDROID_LOG_INFO, "sdkMsg", param))
#endif

extern uint8_t debugLevel; 

struct device_table {
	struct avl_tree hdr;
};


typedef struct {
	uint8_t endpoint; // end point identifier
	uint16_t profileID; // Profile identifier
	uint16_t deviceID; // Device identifier
	uint8_t version; // Version
	uint8_t numInClusters; // the number of in clusters
	uint16_t * pInClusterList; // the list of the in clusters
	uint8_t numOutClusters; // the number of out clusters
	uint16_t * pOutClusterList; // the list of the out clusters
}simpleDesc_t;

// End point information record entry
typedef struct {
	uint8_t IEEEAddr[Z_EXTADDR_LEN]; // the IEEE address of the device
	uint8_t extPanId[Z_EXTADDR_LEN]; // the ext pan id of the network
	uint16_t nwkAddr; // Network address
	uint16_t parentAddr; // parent address
	uint8_t devType; // the type of the device
	uint8_t permitJoin; // the flag of permit join
	uint8_t depth;// the depth in the network
	uint8_t lqi; // the lqi of the device to coordinator
	char* deviceName;
	uint8_t status;
	uint8_t flags;
	uint8_t numEndpoint; // the number of end points
	simpleDesc_t * pSimpleDesc; // the simple description list for each end point
} nodeInfo_t;

#if 0
typedef struct
{
	uint16_t len;
	uint8_t subsystem;
	uint8_t cmd_id;
} pkt_buf_hdr_t;

typedef struct
{
	pkt_buf_hdr_t header;
	uint8_t packed_protobuf_packet[];
} pkt_buf_t;
#endif
typedef struct zcl_attribute_s{
	uint16_t cluster_id;
	uint16_t attribute_id;
	uint8_t data_type;
	uint8_t data_len;
	uint8_t data[MAX_ATTRIBUTE_SIZE];
}zcl_attribute_t;

typedef struct
{
	uint64_t extAddr;
	uint8_t endpoint;
	uint32_t groupaddr;
} zb_addr_t;

/* Cluster and Attribute information for a particular Endpoint */
#define  EP_RULE_TABLE_MAX	 100

typedef struct ep_ruleTable_s {
  struct ep_ruleTable_s *next;
	uint16_t ruleId;
} ep_ruleTable_t;


/* Information for pending attribute read commands */
typedef struct pending_attribute_info_s
{
  struct pending_attribute_info_s *next;
	uint8_t sequence_num;
	uint16_t cluster_id;
  void *readCmd;
} pending_attribute_info_t;



typedef struct
{
	uint8_t	endpointId;
	uint16_t profileId;
	uint16_t deviceId;
	uint8_t num_ip_clusters;
	cluster_info_t ip_cluster_list[MAX_CLUSTERS_ON_EP];
	uint8_t	num_op_clusters;
	cluster_info_t op_cluster_list[MAX_CLUSTERS_ON_EP];
	bool deviceStateVaild;
	bool deviceStateChanged;
	bool report_device;
	uint16_t maxReportInterval;
	uint16_t minReportInterval;
//	device_state_t device_state;
  uint8_t HWversion;
	uint8_t SWversion;
	uint8_t simple_desc_finished;
  ep_ruleTable_t *ep_ruleTable;
//  pending_attribute_info_t *pending_attribute_info;
} endpoint_info_t;

typedef enum
{
	NONE = 0,
	BINDSELECT
} bind_info_t;

typedef enum
{
	BINDING_MODE_BIND = 0,
	BINDING_MODE_UNBIND
} binding_mode_t;

typedef struct
{
	uint8_t lqi_sent;
	uint8_t lqi_start;
	uint8_t active_ep_sent;
} scan_state_t;



/* Display-related information for a Device in the network */
typedef struct device_info
{
	struct  avl_node avl;
	endpoint_info_t epList[MAX_ENDPOINTS];
	uint8_t  extAddr[EXT_ADDR_LEN];
	uint16_t nwkAddr;
	uint16_t manufacturerId;
	uint16_t supperversion;
	bool online;
	uint8_t numOfEps;
	char msg[MAX_DEVICE_INFO_MSG_LEN];
	uint8_t deviceType;
	uint8_t permitJoin;
	scan_state_t scanState;
	time_t timestamp;
  uint8_t sourceRouteTable[23];
} device_info_t;
/* Network Info to be displayed */

typedef struct
{
  uint16 attrID;             // atrribute ID
  uint8  dataType;           // attribute data type
  uint8  *attrData;          // this structure is allocated, so the data is HERE
                             // - the size depends on the data type of attrID
} deviceReport_t;

// Report Attributes Command format
typedef struct
{
  uint8       numAttr;       // number of reports in the list
  deviceReport_t attrList[];    // attribute report list
} deviceReportCmd_t;



extern struct device_table device_table_head;
//extern void zb_compose_address(zb_addr_t * addr,GwAddressStructT * dstaddr);

#ifdef __cplusplus
}
#endif


#endif
