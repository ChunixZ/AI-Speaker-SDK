/*************************************************************************
    > File Name: zigbeeservice.c
    > Author: zyc
    > Mail: zyc@imio.ai 
    > Created Time: 2017/06/30
 ************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "include/hal_types.h"
#include "include/types.h"
#include "include/user_api.h"
#include "include/user_cb.h"
#include "include/user_types.h"
#include "include/zigbeezap.h"

#include "include/cJSON.h"
#include "include/ubus.h"
#include "include/zha_strategy.h"
#include "include/defines.h"

#include "include/zigbee_device.h"
#include "include/httpclient.h"
#include "include/mqtt_cloud.h"
#include "include/wireless.h"
#include "include/device_json.h"
#include "include/cap.h"
#include "wireless.h"
#include "driver_led_key.h"


#define    ZIGBEE_MODE            1
#define    ZIGBEE_DEVICE          1

//Global variable

char *channal_no;
char *host_ip;
char *send_string;

int heartbeat_timestamp;

bool is_device_ready = false;
extern bool is_wireless_ready;
bool is_httpclient_ok = false;
bool is_device_online = false;
bool is_device_state_change = false;
bool is_onbind = false;

bool is_online_rep = false;
bool is_heartbeat = false;
bool is_heartbeat_normal = true;
bool is_device_data_download = false;
bool is_ota_req = false;
bool is_subdevice_bind_rep = false;
bool is_device_bind_report_req = false;
bool is_device_unbind_report_req = false;
bool is_subdevice_data_upload_req = false;
bool is_subdevice_download_rep = false;
bool is_sub_device_action_rep = false;
bool is_sub_device_action_exec_report_req = false;
bool is_avs_activation = false;

bool is_subdevice_status = false;
//2.15.1
bool is_ai_service_req = false;

//relaod the zigbeeService     98304
int re_login_online();


/************************zigbeeCBs************************/
Zstatus_t zha_list_back_info(deviceInfo_t *device_info, uint16_t num)
{
    uint16_t i;
    static char ieeeaddr_string[17] = {0};
    
    debug(DEBUG_GENERAL,"num of device info %d",num);
    for(i = 0; i < num; i++)
    {
        sprintf(ieeeaddr_string,"%02x%02x%02x%02x%02x%02x%02x%02x",\
            device_info[i].deviceBasic.ieeeAddr[0],\
            device_info[i].deviceBasic.ieeeAddr[1],\
            device_info[i].deviceBasic.ieeeAddr[2],\
            device_info[i].deviceBasic.ieeeAddr[3],\
            device_info[i].deviceBasic.ieeeAddr[4],\
            device_info[i].deviceBasic.ieeeAddr[5],\
            device_info[i].deviceBasic.ieeeAddr[6],\
            device_info[i].deviceBasic.ieeeAddr[7]);
        debug(DEBUG_GENERAL,"id %s",ieeeaddr_string);
    }
    
    return ZSUCCESS;
}


Zstatus_t deviceStateChange(deviceInfo_t *deviceInfo, uint16_t clusterId)
{
    char ieee[17] = {0};
    
    sprintf(ieee,"%02x%02x%02x%02x%02x%02x%02x%02x",\
        deviceInfo->deviceBasic.ieeeAddr[0],\
        deviceInfo->deviceBasic.ieeeAddr[1],\
        deviceInfo->deviceBasic.ieeeAddr[2],\
        deviceInfo->deviceBasic.ieeeAddr[3],\
        deviceInfo->deviceBasic.ieeeAddr[4],\
        deviceInfo->deviceBasic.ieeeAddr[5],\
        deviceInfo->deviceBasic.ieeeAddr[6],\
        deviceInfo->deviceBasic.ieeeAddr[7]);

    debug(DEBUG_ERROR,"device leave ieee[%s]  clusterId[%d]", ieee, clusterId);
    
    
    if(!is_onbind)
    {
        if(is_device_state_change)
        {
            //2.9.1 subdevice_data_upload_req
            if(NULL != deviceInfo)
            {
                /*
                if(deviceInfo->deviceState.rawData.dataLen)
                {
                    debug(1, "--------------------sub_device_data_req----------------------");
                    int buff_size = deviceInfo->deviceState.rawData.dataLen;
                    char buf_recice[buff_size+1];
                    debug(1, "%d", deviceInfo->deviceState.rawData.dataLen);
                    //bytes2hexStr(deviceInfo->deviceState.rawData.data, buf_recice, buff_size+1);
                    //debug(1, "%s", buf_recice);
                }
                else
                {
                    sub_device_data_req(deviceInfo, &send_string);
                    is_subdevice_status = true;
                }
                */
                sub_device_data_req(deviceInfo, &send_string);
                is_subdevice_status = true;
            }
        }
    }
    
    return ZSUCCESS;
}

//gateway modul init sucess
Zstatus_t zigbeeModuleStatus(ZigbeeModuleStatus_t status)
{
    is_device_ready = true;
    
    unsigned char tmp_ieee[8] = {0};
    memset(tmp_ieee,0,8);
    device_setPermitJoin(tmp_ieee,0);
    memset(tmp_ieee,0xff,8);
    device_setPermitJoin(tmp_ieee,0);
    
    return ZSUCCESS;
}

int device_online_offline_cb(unsigned char ieee_addr[8], char online_or_not)
{
    uint8 ieeeaddr_string[18] = {0};

    snprintf(ieeeaddr_string,17,"%02x%02x%02x%02x%02x%02x%02x%02x",\
            ieee_addr[0],ieee_addr[1],ieee_addr[2],ieee_addr[3],\
            ieee_addr[4],ieee_addr[5],ieee_addr[6],ieee_addr[7]);

    debug(DEBUG_ERROR,"ieee[%s] is online_or_not[%d]", ieeeaddr_string, online_or_not);
}

int device_unregister(unsigned char ieee_addr[8])
{
    char ieee[17] = {0};
    
    sprintf(ieee,"%02x%02x%02x%02x%02x%02x%02x%02x",ieee_addr[0],ieee_addr[1]\
            ,ieee_addr[2],ieee_addr[3],ieee_addr[4],ieee_addr[5],ieee_addr[6],ieee_addr[7]);

    debug(DEBUG_ERROR,"device leave ieee[%s]",ieee);
}

int zigbeeGroupStateChange(uint8_t ieee_Addr[8],uint8_t epId,uint8_t cmdId,uint8_t status,uint8_t grpCnt,uint16_t *grpList,uint8_t capacity,char *grpName)
{
    if(cmdId == 0x00)//group add rsp
    {
        debug(DEBUG_ERROR,"group add rsp");
    }
    else if(cmdId == 0x01)//group view
    {
        debug(DEBUG_ERROR,"group view");
    }
    else if(cmdId == 0x02)//group member ship
    {
        debug(DEBUG_ERROR,"group member ship");
    }
    else if(cmdId == 0x03)//group remove rsp
    {
        debug(DEBUG_ERROR,"group remove rsp");
    }
    else//unknow cmd
    {
        debug(DEBUG_ERROR,"unknow cmd");
    }
    if((cmdId == 0x00)||(cmdId == 0x03))//group add rsp
    {
        debug(DEBUG_ERROR,"grpList[0][%d]",grpList[0]);
        debug(DEBUG_ERROR,"epId[%d]",epId);
    }
    else if((cmdId == 0x02)||(cmdId == 0x01))
    {
    }
}

int zigbeeSceneStateChange(uint8_t ieee_Addr[8],uint8_t epId,uint8_t cmdId,uint8_t status,uint8_t grpCnt,uint16_t *grpList,uint8_t capacity,char *grpName)
{
    
}

int pfn_zigbee_cmdIncoming(unsigned char ieee_addr[8],uint8_t epId,uint16_t clusterId,uint8_t cmdId,void *pCmd)
{
	uint8 *p_data = NULL;
	uint8 ieeeaddr_string[18] = {0};
	
	if(!pCmd)
		debug(DEBUG_ERROR,"pCmd is NULL");

	snprintf(ieeeaddr_string,17,"%02x%02x%02x%02x%02x%02x%02x%02x",\
			ieee_addr[0],ieee_addr[1],ieee_addr[2],ieee_addr[3],\
			ieee_addr[4],ieee_addr[5],ieee_addr[6],ieee_addr[7]);
		
	debug(DEBUG_ERROR,"ieee[%s]",ieeeaddr_string);
	debug(DEBUG_ERROR,"epId[%d]",epId);
	debug(DEBUG_ERROR,"clusterId[%04x]",clusterId);
	debug(DEBUG_ERROR,"cmdId[%02x]",cmdId);

	switch(clusterId)
	{
		case ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK:
		{
			switch(cmdId)
			{
				case 0x21:
				{
					debug(DEBUG_ERROR,"programEventCode[]");
					/*
					zclDoorLockProgrammingEventNotification_t *notify_program = (zclDoorLockProgrammingEventNotification_t *)pCmd;
		
					p_data = (uint8 *)pCmd + 11;
					
					debug(DEBUG_ERROR,"programEventSource[%d]",notify_program->programEventSource);
					debug(DEBUG_ERROR,"programEventCode[%d]",notify_program->programEventCode);
					debug(DEBUG_ERROR,"userID[%d]",notify_program->userID);
					debug(DEBUG_ERROR,"pin[%d]",notify_program->pin);
					debug(DEBUG_ERROR,"userType[%d]",notify_program->userType);
					debug(DEBUG_ERROR,"userStatus[%d]",notify_program->userStatus);
					debug(DEBUG_ERROR,"zigBeeLocalTime[%d]",notify_program->zigBeeLocalTime);
					*/
				}
				break;
				case 0x20:
				{
					debug(DEBUG_ERROR,"programEventCode[]");
					/*
					zclDoorLockOperationEventNotification_t *notify_operation = (zclDoorLockOperationEventNotification_t *)pCmd;

					p_data = (uint8 *)pCmd + 9;

					debug(DEBUG_ERROR,"programEventSource[%d]",notify_operation->operationEventSource);
					debug(DEBUG_ERROR,"programEventCode[%d]",notify_operation->operationEventCode);
					debug(DEBUG_ERROR,"userID[%d]",notify_operation->userID);
					debug(DEBUG_ERROR,"pin[%d]",notify_operation->pin);
					debug(DEBUG_ERROR,"zigBeeLocalTime[%d]",notify_operation->zigBeeLocalTime);
					*/
				}
				break;
				default:
					debug(DEBUG_ERROR,"cmdId[%02x] is not support",cmdId);
				break;
			}
		}
		break;
		case ZCL_CLUSTER_ID_SS_IAS_ACE:
		{
			switch(cmdId)
			{
				case 0x04:
				{
					debug(DEBUG_ERROR,"programEventCode[]");
					/*
					zclACEPanelStatusChanged_t *panel_cmd = (zclACEPanelStatusChanged_t *)pCmd;
					
					debug(DEBUG_ERROR,"panelStatus[%d]",panel_cmd->panelStatus);
					debug(DEBUG_ERROR,"secondsRemaining[%d]",panel_cmd->secondsRemaining);
					debug(DEBUG_ERROR,"audibleNotification[%d]",panel_cmd->audibleNotification);
					debug(DEBUG_ERROR,"alarmStatus[%d]",panel_cmd->alarmStatus);
					*/
				}
				break;
				default:
					debug(DEBUG_ERROR,"cmdId[%02x] is not support",cmdId);
				break;
			}
		}
		break;
		default:
			debug(DEBUG_ERROR,"clusterId[%04x] is not support",clusterId);
		break;
	}
}


user_zigbeeCBs_t zigbeeCBs = {
    zha_list_backinfo,      //user_all_device_state_cb_t pfn_all_deviceState;
    deviceStateChange,
    zigbeeModuleStatus,
    device_online_offline_cb,  //device_online_offline_cb,
    NULL,
    device_unregister,
    zigbeeGroupStateChange,
    zigbeeSceneStateChange,
    NULL,// user_zigbee_announce_cb_t pfn_zigbee_announce;
    pfn_zigbee_cmdIncoming,//user_zigbee_cmdIncoming_cb_t pfn_zigbee_cmdIncoming; NULL,//
    NULL,//user_zigbee_attrChange_cb_t pfn_zigbee_attrChange;
    NULL,//user_zigbee_cmdChange_cb_t pfn_zigbee_cmdChange;
    NULL,//user_zigbee_reportIn_cb_t pfn_zigbee_reportIn;
    NULL,//user_sendResult_cb_t pfn_sendResultIn;
};



pthread_t id_cap, id_mqtt, id_httpclient, id_wireless;
pthread_t mqtt_subs, mqtt_heart;

//Send heartbeat to cloud every 30s
void *thread_mqtt_sub_heart(void *p)
{
    sleep(1);
    is_online_rep = true;
    sleep(1);
    is_device_state_change = true;

    while(1)
    {
        is_heartbeat = true;
        sleep(30);
    }
    
    while(1)
    {
        debug(1, "Start... thread_mqtt_sub_heart");
        sleep(2);
    }
    
    pthread_exit(0);
}

void *thread_mqtt_sub_main(void *p)
{
    //Subscribe to the mqtt message and wait for the cloud operation
    if(NULL != channal_no)
    {
        char channal_str[128];
        sprintf(channal_str, "IN@%s", channal_no);
        debug(DEBUG_DEBUG, "%s %s", host_ip, channal_str);
        mqtt_ssl_sub(host_ip, channal_str);  //mqtt_sub    mqtt_ssl_sub
    }
    
    pthread_exit(0);
}

void *thread_mq_main(void *p)
{
    char *send_str;
    
    //1. Wait for channal_no_is Ok
    while(!is_device_online)
    {
        sleep(1);
    }

    //2, Create pthread to wait mqtt message
    //MQTT 接收处理线程，处理过程不能占用过多时间；
    if(pthread_create(&mqtt_subs, NULL, (void *)thread_mqtt_sub_main, NULL))
    {
        printf("Create the pthread_get error!\n");
    }

    if(pthread_create(&mqtt_heart, NULL, (void *)thread_mqtt_sub_heart, NULL))
    {
        printf("Create the pthread_get error!\n");
    }
    
    //device_ai_speaker("现在，妙琦为你服务。");
    led_driver(RE_GREEN);
    
    char channal_str[128];
    sprintf(channal_str, "OUT@%s", channal_no);
    debug(DEBUG_DEBUG, "%s", channal_str);
    
    while(1)
    {
        usleep(20*1000);//20ms
        
        //2.1.1 Devices oline request
        if(is_online_rep)
        {
            device_online_req(&send_str , channal_str);
            debug(DEBUG_DEBUG, "%s", send_str);
            mqtt_ssl_pub(channal_str, send_str);
            is_online_rep = false;
        }
        
        //2.2.1 Devices heart
        if(is_heartbeat)
        {
            device_hb_req(&send_str);
            debug(DEBUG_DEBUG, "%s", send_str);
            mqtt_ssl_pub(channal_str, send_str);
            is_heartbeat = false;
        }
        
        //2.5.2
        if(is_device_data_download)
        {
            device_data_download_rep(&send_str);
            debug(DEBUG_DEBUG, "%s", send_str);
            mqtt_ssl_pub(channal_str, send_str);
            is_device_data_download = false;
        }
        
        //2.6.2 OTA
        if(is_ota_req)
        {
            ota_request_rep(&send_str);
            debug(DEBUG_DEBUG, "%s", send_str);
            mqtt_ssl_pub(channal_str, send_str);
            is_ota_req = false;
        }
        
        //2.8.2 Sub devices bind
        if(is_subdevice_bind_rep)
        {
            sub_device_bind_rep(&send_str);
            debug(DEBUG_DEBUG, "%s", send_str);
            mqtt_ssl_pub(channal_str, send_str);
            is_subdevice_bind_rep = false;
        }
        
        //2.8.3 Sub devices bind
        if(is_device_bind_report_req)
        {
            sub_device_bind_report_req_send(&send_str);
            debug(DEBUG_DEBUG, "%s", send_str);
            mqtt_ssl_pub(channal_str, send_str);
            is_device_bind_report_req = false;
        }
        
        //2.9.1    
        if(is_subdevice_data_upload_req)
        {
            if(NULL != send_string)
            {
                debug(DEBUG_DEBUG, "%s", send_string);
                mqtt_ssl_pub(channal_str, send_string);
                is_subdevice_data_upload_req = false;
                send_string = NULL;
            }
        }
        
        //2.10.2    
        if(is_subdevice_download_rep)
        {
            sub_device_download_rep(&send_str);
            debug(DEBUG_DEBUG, "%s", send_str);
            mqtt_ssl_pub(channal_str, send_str);
            is_subdevice_download_rep = false;
        }
        
        //2.11.2
        if(is_sub_device_action_rep)
        {
            sub_device_action_rep(&send_str);
            //debug(DEBUG_DEBUG, "%s", send_str);
            mqtt_ssl_pub(channal_str, send_str);
            is_sub_device_action_rep = false;
        }
        
        //2.11.3
        if(is_sub_device_action_exec_report_req)
        {
            sub_device_action_exec_report_req(&send_str);
            //debug(DEBUG_DEBUG, "%s", send_str);
            mqtt_ssl_pub(channal_str, send_str);
            is_sub_device_action_exec_report_req = false;
        }
        
        //2.13.2 unbind sub-device
        if(is_device_unbind_report_req)
        {
            unbind_sub_rep(&send_str);
            debug(DEBUG_DEBUG, "%s", send_str);
            mqtt_ssl_pub(channal_str, send_str);
            is_device_unbind_report_req = false;
        }
        
        //2.15.1
        if(is_ai_service_req)
        {
            ai_service_req_send(&send_str);
            debug(DEBUG_DEBUG, "%s", send_str);
            mqtt_ssl_pub(channal_str, send_str);
            is_ai_service_req = false;
        }
    }
    
    mqtt_cleanup();
    debug(1, "------------ thread_mq_main() exit! ------------");
    
   pthread_exit(0);
}

void *thread_httpclient(void *p)
{
    while(!is_httpclient_ok)
    {
        debug(DEBUG_DEBUG, "--------------------thread_httpclient start--------------------");
        HTTPCLIENT_RESULT ret = HTTPCLIENT_ERROR_CONN;
        FILE_RESULT ret_f = FILE_ERROR;
        char *send_str;
        
        //1. Look out the file is exist or not
        ret_f = read_register(&send_str);
        debug(DEBUG_DEBUG, "--------> %d %s", ret_f, send_str);
        
        if(FILE_NOT_EXIST == ret_f)
        {
            ret = httpclient_register();
            if(HTTPCLIENT_OK != ret)
                debug(DEBUG_DEBUG, "--------------httpclient_register() failed---------------\n");
            else
                debug(DEBUG_DEBUG, "--------------httpclient_register() success---------------\n");
        }
        else if(FILE_RELOAD == ret_f)
        {
            httpclient_relaod();
        }
        else
        {
            if(NULL != send_str)
            {
                ret = httpclient_login(send_str);
                if(HTTPCLIENT_OK == ret)
                {
                    ret_f = read_online(&channal_no, &host_ip);
                    if(FILE_OK == ret_f)
                    {
                        //-----------------------> channal_no_there
                        debug(DEBUG_DEBUG, "--------> %s", channal_no);
                        debug(DEBUG_DEBUG, "--------> %s", host_ip);
                        if(NULL != channal_no)
                        {
                            is_httpclient_ok = true;
                            is_device_online = true;
                        }
                    }
                }
                else
                {
                    debug(DEBUG_DEBUG, "--------> httpclient_login error! <--------");
                }
            }
        }
        
        sleep(5);
    }
   
    debug(1, "------------ thread_httpclient() exit! ------------");
    pthread_exit(0);
}

int re_login_online()
{
    debug(1, "----> re_login_online()");
    is_httpclient_ok = false;
    is_device_online = false;
    
    if(pthread_create(&id_httpclient, NULL, (void *)thread_httpclient, NULL))
    {
        printf("Create the thread_httpclient error!\n");
    }
    
    if(pthread_create(&id_mqtt, NULL, (void *)thread_mq_main, NULL))
    {
        printf("Create the thread_mq_main error!\n");
    }
    
    return 0;
}

void *thread_re_login(void *p)
{
    debug(1, "-------- heartbeat is lost --------");
    debug(1, "-------- heartbeat is lost --------");
    debug(1, "-------- heartbeat is lost --------");
    pthread_cancel(mqtt_heart);
    pthread_cancel(id_mqtt);
    if(is_wireless_ready)
    {
        re_login_online();
    }
    
    pthread_exit(0);
}


void *thread_wireless_main(void *p)
{
    wifi_main();

    pthread_exit(0);
}

/*
void *thread_cap_main(void *p)
{
    //cap_main();

    pthread_exit(0);
}
*/

void *thread_main(void *p)
{
    //1. wifi setup
    if(pthread_create(&id_wireless, NULL, (void *)thread_wireless_main, NULL))
    {
        printf("Create the thread_wireless_main error!\n");
    }
    
    //sleep(8);
    debug(1, "get_timestamp() : %s", get_timestamp());
    //2. Start the cap and begin ask to answer
    /*
    if(pthread_create(&id_cap, NULL, (void *)thread_cap_main, NULL))
    {
        printf("Create the thread_cap_main error!\n");
    }
    */
    
#if ZIGBEE_MODE
    debug(DEBUG_DEBUG, "----------------%s----------------", "Start main thread");
    /**/
    //3. Check whether the network Unicom, if not waiting  **Net debug**
    while(!is_wireless_ready)
    {
        sleep(2);
        debug(DEBUG_DEBUG, "------------Network is not ready------------");
    }
    
    baidu_tts_init();
    
    //4. Get mqtt channal_no
    if(pthread_create(&id_httpclient, NULL, (void *)thread_httpclient, NULL))
    {
        printf("Create the thread_httpclient error!\n");
    }
    
    //5. If the network is ready, begin the mqtt pthread
    if(pthread_create(&id_mqtt, NULL, (void *)thread_mq_main, NULL))
    {
        printf("Create the thread_mq_main error!\n");
    }
#endif

    while(1)
    {
        if(is_device_online)
        {
            if(!is_heartbeat_normal)
            {
                is_heartbeat_normal = true;
                
                pthread_t re_login;
                if(pthread_create(&re_login, NULL, (void *)thread_re_login, NULL))
                {
                    printf("Create the pthread_get error!\n");
                }
                else
                {
                    debug(DEBUG_DEBUG, "------------thread_re_login is OK------------");
                }
            }
        }
        
        //6. wait for zigbeeSDK initialization is OK
#if ZIGBEE_DEVICE
        if(!is_device_ready)
        {
            usleep(500 * 1000);
        }
#endif
        usleep(500 * 1000);
    }
    
    pthread_exit(0);
}

void signal_main_handler(int sig)
{
    signal(sig, SIG_DFL);
    fprintf(stderr, _("Aborted by signal %s...\n"), strsignal(sig));
    debug(1, "----main exit----");
    exit (0);
}

int zigbee_main()//int argc, char* argv[]
{
    /**Note: 1
    ** The main thread is used for the initialization of the gateway device
    **/
    printf("*********************************************************\n");
    printf("************sdk version: [%s]**********\n",lib_compileTime());
    printf("*********************************************************\n");

    /**Note: 2
    **Open a new thread to start other threads, such as the listening part of MQTT
    /**/
    pthread_t p_main;
    if(pthread_create(&p_main, NULL, (void *)thread_main, NULL) != 0)
    {
        printf("Create the pthread_get error!\n");
    }
    
    signal(SIGINT, signal_main_handler);
    
    /**Note: 3
    **Init Shuncom SDK and set callback func
    **/
    //init loop event
    uloop_init();
    //init serial  "/dev/ttyATH0"  //zigbeezap(argv[1]);
    zigbeezap("/dev/ttyS2");
    //init zigbee serice 
    zigbee_init("/etc/config/zha.db");
    ubus_init();
    //zigbee callback
    user_registerZigbeeCBs(&zigbeeCBs);
    device_loadManufacturerSupperversion("LUMI","lumi.plug",60);
    debug(DEBUG_GENERAL,"uloop run");
    device_enableWhitelist(1);
    
    /**Note: 4
    ** In main thread: running zigbee_loop()
    **/
    zigbee_loop();
    
    return 0;
}

