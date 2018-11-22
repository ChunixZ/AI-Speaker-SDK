/*************************************************************************
    > File Name: zigbeeservice.c
    > Author: zyc
    > Mail: zyc@imio.ai 
    > Created Time: 2017/07/02
 ************************************************************************/
#ifndef _ZIGBEE_DEVICE_H_
#define _ZIGBEE_DEVICE_H_

#define DEBUG_1    0

#include <stdio.h>
#include <stdbool.h>
#include "hal_types.h"
#include "types.h"
#include "user_api.h"
#include "user_cb.h"
#include "user_types.h"
#include "zigbeezap.h"

#include "cJSON.h"
#include "ubus.h"
#include "zha_strategy.h"
#include "defines.h"


#define     SUCCESS                                 1
#define     FAILURE                                 0
#define     WRITE_CONTROL                  0
#define     CMD_CONTROL                      0

#define     CMD_SUCCESS                       0
#define     CMD_FAILURE                        1
#define     CMD_PARA_ERROR                 2
#define     CMD_MALLOC_ERROR            3
#define     CMD_NOT_FIND_METHOD    4

#define     IEEE_ADDR_LEN                     8

#define     TYPE_BOOL                             0
#define     TYPE_STRING                         1
#define     TYPE_BYTE                              2
#define     TYPE_SHORT                           3
#define     TYPE_THREE                            4
#define     TYPE_INT                                5
#define     TYPE_LL                                   6
#define     TYPE_SHORT_STRING            7
#define     TYPE_THREE_STRING             8
#define     TYPE_INT_STRING                 9
#define     TYPE_LL_STRING                   10

#define     END_FLAG_BUF                    "xx"

#define     VISIBLE                                  0
#define     DISVISIBLE                            1

/*************************basic****************************/
#define ATTRID_BASIC_ZCL_VERSION                          0x0000
#define ATTRID_BASIC_APPL_VERSION                         0x0001
#define ATTRID_BASIC_STACK_VERSION                        0x0002
#define ATTRID_BASIC_HW_VERSION                           0x0003
#define ATTRID_BASIC_MANUFACTURER_NAME                    0x0004
#define ATTRID_BASIC_MODEL_ID                             0x0005
#define ATTRID_BASIC_DATE_CODE                            0x0006
#define ATTRID_BASIC_POWER_SOURCE                         0x0007
#define ATTRID_BASIC_APPLICATION_PROFILE_VERSION          0x0008
#define ATTRID_BASIC_SW_BUILD_ID                          0x4000


/***************************on off*****************************/
#define ATTRID_ON_OFF                                     0x0000
#define ATTRID_ON_OFF_GLOBAL_SCENE_CTRL                   0x4000
#define ATTRID_ON_OFF_ON_TIME                             0x4001
#define ATTRID_ON_OFF_OFF_WAIT_TIME                       0x4002

/*************************on off switch************************/
#define ATTRID_ON_OFF_SWITCH_TYPE                         0x0000
#define ATTRID_ON_OFF_SWITCH_ACTIONS                      0x0010

/*********************************************************/
#define ATTRID_SE_METERING_CURR_SUMM_DLVD                 0x0000
#define ATTRID_SE_METERING_STATUS                          0x0200
#define ATTRID_SE_METERING_UOM                            0x0300
#define ATTRID_SE_METERING_MULT                           0x0301
#define ATTRID_SE_METERING_DIV                            0x0302    
#define ATTRID_SE_METERING_SUMM_FMTG                          0x0303
#define ATTRID_SE_METERING_DEVICE_TYPE                    0x0306 

/***********************pm25******************************/
#define ATTRID_IOV_BASIC_STATE_TEXT                        0x000E

/*************************zone****************************/
#define ATTRID_SS_ZONE_ID                                0x0011 
#define ATTRID_SS_IAS_ZONE_TYPE                          0x0001
#define ATTRID_SS_IAS_ZONE_STATUS                        0x0002 

/***********************battery******************************/
#define ATTRID_POWER_CFG_BAT_SIZE                         0x0031
#define ATTRID_POWER_CFG_MAINS_ALARM_MASK                 0x0010

/**************************door lock****************************/
#define ATTRID_CLOSURES_LOCK_STATE                       0x0000
#define ATTRID_CLOSURES_LOCK_TYPE                        0x0001
#define ATTRID_CLOSURES_ACTUATOR_ENABLED                 0x0002

/**************************************************************************/
/***          HVAC: Thermostat Cluster Attributes                       ***/
/**************************************************************************/
  // Thermostat information attribute set
#define ATTRID_HVAC_THERMOSTAT_LOCAL_TEMPERATURE           0x0000
#define ATTRID_HVAC_THERMOSTAT_RUNNING_STATE               0x0029
#define ATTRID_HVAC_THERMOSTAT_OCCUPIED_COOLING_SETPOINT   0x0011
#define ATTRID_HVAC_THERMOSTAT_OCCUPIED_HEATING_SETPOINT   0x0012
#define ATTRID_HVAC_THERMOSTAT_ALAM_MASK                   0x001D


/****************************************/
/*** Level Control Cluster Attributes ***/
/****************************************/
#define ATTRID_LEVEL_CURRENT_LEVEL                        0x0000
#define ATTRID_LEVEL_REMAINING_TIME                       0x0001
#define ATTRID_LEVEL_ON_OFF_TRANSITION_TIME               0x0010
#define ATTRID_LEVEL_ON_LEVEL                             0x0011
#define ATTRID_LEVEL_ON_TRANSITION_TIME                   0x0012
#define ATTRID_LEVEL_OFF_TRANSITION_TIME                  0x0013
#define ATTRID_LEVEL_DEFAULT_MOVE_RATE                    0x0014

/*********************fan***********************************/
#define ATTRID_HVAC_FAN_CTRL_FAN_MODE                                    0x0000 // M, R/W, ENUM8
#define ATTRID_HVAC_FAN_CTRL_FAN_SEQUENCE                                0x0001


/*****************************wimdow*******************************/
#define ATTRSET_WINDOW_COVERING_INFO                        0x0000
#define ATTRSET_WINDOW_COVERING_SETTINGS                    0x0010

#define ATTRID_CLOSURES_CURRENT_POSITION_LIFT_PERCENTAGE    ( ATTRSET_WINDOW_COVERING_INFO + 0x0008 )
#define ATTRID_CLOSURES_WINDOW_COVERING_TYPE                ( ATTRSET_WINDOW_COVERING_INFO + 0x0000 )
#define ATTRID_CLOSURES_WINDOW_COVERING_MODE                ( ATTRSET_WINDOW_COVERING_SETTINGS + 0x0007 )

/*******************A_ELECTRICAL_MEASUREMENT******************/
#define ATTRID_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE            0x0505
#define ATTRID_ELECTRICAL_MEASUREMENT_RMS_CURRENT            0x0508    
#define ATTRID_ELECTRICAL_MEASUREMENT_ACTIVE_POWER            0x050B
#define ATTRID_ELECTRICAL_MEASUREMENT_POWER_FACTOR          0x0510    
#define ATTRID_ELECTRICAL_MEASUREMENT_REACTIVE_POWER        0x050E


/*******************************/
/*** On/Off Cluster Commands ***/
/*******************************/
#define COMMAND_OFF                                       0x00
#define COMMAND_ON                                        0x01
#define COMMAND_TOGGLE                                    0x02

// Server commands received (Client-to-Server in ZCL Header)
#define COMMAND_SS_IAS_WD_START_WARNING                   0x00
#define COMMAND_SS_IAS_WD_SQUAWK                          0x01

/***********************************/
/*** Identify Cluster Attributes ***/
/***********************************/
#define ATTRID_IDENTIFY_TIME                             0x0000
#define ATTRID_IDENTIFY_COMMISSION_STATE                 0x0001

/*********************************************/
/***  Appliance Control Cluster Attributes ***/
/********************************************/

// Server Attributes
#define ATTRID_APPLIANCE_CONTROL_START_TIME              0x0000  // M, R, UINT16
#define ATTRID_APPLIANCE_CONTROL_FINISH_TIME             0x0001  // M, R, UINT16
#define ATTRID_APPLIANCE_CONTROL_REMAINING_TIME          0x0002  // O, R, UINT16


/*****************************************************************************/
/***         Occupancy Sensing Cluster Attributes                          ***/
/*****************************************************************************/
    // Occupancy Sensor Configuration attribute set
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_OCCUPANCY                     0x0000 // M, R, BITMAP8
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_OCCUPANCY_SENSOR_TYPE         0x0001 // M, R, ENUM8

    // PIR Configuration attribute set
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_PIR_O_TO_U_DELAY              0x0010 // O, R/W, UINT16
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_PIR_U_TO_O_DELAY              0x0011 // O, R/W, UINT16
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_PIR_U_TO_O_THRESH             0x0012 // O, R/W, UINT8

    // Ultrasonic Configuration attribute set
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_ULTRASONIC_O_TO_U_DELAY       0x0020 // O, R/W, UINT16
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_ULTRASONIC_U_TO_O_DELAY       0x0021 // O, R/W, UINT16
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_ULTRASONIC_U_TO_O_THRESH      0x0022 // O, R/W, UINT8

/********************************/
/*** Group Cluster Attributes ***/
/********************************/
#define ATTRID_GROUP_NAME_SUPPORT                         0x0000


/*********************************/
/*** Scenes Cluster Attributes ***/
/*********************************/
  // Scene Management Information
#define ATTRID_SCENES_COUNT                               0x0000
#define ATTRID_SCENES_CURRENT_SCENE                       0x0001
#define ATTRID_SCENES_CURRENT_GROUP                       0x0002
#define ATTRID_SCENES_SCENE_VALID                         0x0003
#define ATTRID_SCENES_NAME_SUPPORT                        0x0004
#define ATTRID_SCENES_LAST_CFG_BY                         0x0005


typedef struct{
    unsigned short int attribute_id;
    unsigned short int data_type;
    unsigned short int visible;
    unsigned char key[20];
}attribute_info;

typedef struct{
    unsigned short int cluaster_id;
    attribute_info *attributes;
}cluaster_info;



//set_device_status struct
#define COMMAND_CLOSURES_LOCK_DOOR       0x00 // M  zclDoorLock_t
#define COMMAND_CLOSURES_UNLOCK_DOOR   0x01 // M  zclDoorLock_t
#define COMMAND_CLOSURES_TOGGLE_DOOR   0x02 // O  zclDoorLock_t

#define UP_OPEN                "upopen"
#define DOWN_CLOSE       "downclose"
#define STOP                      "stop"
#define LOCK                      "lock"

#define MAX_DEVICES 64

//extern functions
extern int local_list_new(cJSON **);
extern int local_list_ieeeAddr(cJSON **, char *);
extern int bind_subdevices();
extern void get_device_status();
extern uint8_t set_device_status(cJSON *);
extern int unbind_subdevices();
extern void subdevice_bind_report();
extern void set_be();
extern int cloud_is_coordinfo_ok();


#endif
