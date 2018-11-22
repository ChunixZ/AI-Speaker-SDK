/*************************************************************************
 *
    > File Name: user_cb.h
    > Author: lunan
    > Mail: 6616@shuncom.com 
    > Created Time: 2015年09月11日 星期五 11时55分19秒
 ************************************************************************/

#ifndef _USER_CB_H_
#define _USER_CB_H_

#ifdef __cplusplus
extern "C"
{
#endif


#include "user_types.h"
#include "types.h"

#define MAX_DEVICESTATE_CBS   10


typedef Zstatus_t (*user_allState_cb_t)( deviceInfo_t *devices,uint16_t num );

typedef Zstatus_t (*user_stateChange_cb_t)( deviceInfo_t *dataInfo,uint16_t clusterId);

typedef Zstatus_t (*user_zigbeeModuleStatus_cb_t)(ZigbeeModuleStatus_t status);

typedef Zstatus_t (*user_whiteList_cb_t)(uint8_t *extAddr,uint8_t num);

typedef uint16_t (*user_Zap_znpOutgoing_cb_t)(uint8_t *data,uint16_t len);

typedef int (*user_zigbee_update_online_status_cb_t)(unsigned char ieee_addr[8], char online_or_not);

typedef int (*user_zigbee_register_device_cb_t)(unsigned char ieee_addr[8],
        unsigned int model_id, const char *rand, const char *sign);

typedef int (*user_zigbee_unregister_device_cb_t)(unsigned char ieee_addr[8]);

typedef int (*user_zigbee_groupStateChange_cb_t)(uint8_t ieee_Addr[8],uint8_t epId,uint8_t cmdId,uint8_t status,uint8_t grpCnt,uint16_t *grpList,uint8_t capacity,char *grpName);

typedef int (*user_zigbee_sceneStateChange_cb_t)(uint8_t ieee_Addr[8],uint8_t epId,uint8_t cmdId,uint8_t status,uint8_t grpCnt,uint16_t *grpList,uint8_t capacity,char *grpName);

typedef uint16_t (*user_quanshiData_cb_t)(uint8_t ieee_Addr[8],uint16_t clusterId,uint8_t *data,uint16_t len);

typedef int (*user_zigbee_announce_cb_t)(unsigned char ieee_addr[8],uint16_t nwk_addr);

typedef int (*user_zigbee_cmdIncoming_cb_t)(unsigned char ieee_addr[8],uint8_t epId,uint16_t clusterId,uint8_t cmdId,void *pCmd);

typedef int (*user_zigbee_attrChange_cb_t)(unsigned char ieee_addr[8],uint8_t epId,char *attrName,char  *attValue);

typedef int (*user_zigbee_cmdChange_cb_t)(unsigned char ieee_addr[8],uint8_t epId,char *cmdName,char *cmdValue);

typedef int (*user_zigbee_reportIn_cb_t)(device_info_t *device,uint8_t epId,uint16_t clusterId,deviceReportCmd_t *deviceReport);

typedef int (*shuncom_interpanTest_cb_t)(uint8_t tx_LinkQuality,uint8_t rx_LinkQuality,uint16_t zigbeeModule_version);

typedef int (*user_sendResult_cb_t)(uint8_t *send_result);


typedef struct {

	user_allState_cb_t pfn_allState;

	user_stateChange_cb_t pfn_stateChange;

  user_zigbeeModuleStatus_cb_t pfn_zigbeeModuleStatus;

  user_zigbee_update_online_status_cb_t pfn_zigbeeUpdateOnlineStatue;

  user_zigbee_register_device_cb_t pfn_zigbeeRegisterDevice;

  user_zigbee_unregister_device_cb_t pfn_zigbeeUnRegisterDevice;

  user_zigbee_groupStateChange_cb_t pfn_zigbeeGroupStateChange;

  user_zigbee_sceneStateChange_cb_t pfn_zigbeeSceneStateChange;

  user_zigbee_announce_cb_t pfn_zigbee_announce;

  user_zigbee_cmdIncoming_cb_t pfn_zigbee_cmdIncoming;

  user_zigbee_attrChange_cb_t pfn_zigbee_attrChange;

  user_zigbee_cmdChange_cb_t pfn_zigbee_cmdChange;

  user_zigbee_reportIn_cb_t pfn_zigbee_reportIn;

  user_sendResult_cb_t pfn_sendResultIn;

} user_zigbeeCBs_t;

Zstatus_t user_registerZigbeeCBs(user_zigbeeCBs_t *callbacks);

Zstatus_t user_registerZigbeeModuleDataCBs(user_Zap_znpOutgoing_cb_t callbacks);

Zstatus_t user_registerQuanshi(user_quanshiData_cb_t callbacks);

#ifdef __cplusplus
}
#endif


#endif
