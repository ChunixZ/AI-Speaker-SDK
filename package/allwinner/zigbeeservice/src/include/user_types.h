/*************************************************************************
 *
    > File Name: user_typs.h
    > Author: lunan
    > Mail: 6616@shuncom.com 
    > Created Time: 2015年08月25日 星期二 11时04分04秒
 ************************************************************************/

#ifndef _USER_TYPS_H_
#define _USER_TYPS_H_

#ifdef __cplusplus
extern "C"
{
#endif


#include <sys/types.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>
#include <hal_types.h>
#include <libubox/avl.h>
#include <libubox/uloop.h>

#define ZSUCCESS			0x00
#define ZFAILURE			0x01
#define ZMALLOCERROR		0x02
#define ZSENDERROR			0x03

typedef uint16_t Zstatus_t;

#define ZIGBEEMOULDE_READY		0x00
#define ZIGBEEMOULDE_ERROR		0x01


typedef uint8_t ZigbeeModuleStatus_t;
// Zigbee Home Automation Profile Identification
#define ZCL_HA_PROFILE_ID                                       0x0104
#define ZCL_SHUNCOM_PROFILE_ID                                  0xC105

//shuncom device ids
#define ZCL_SHUNCOM_DEVICEID_TARNSPORT		      					0x0001
#define ZCL_SHUNCOM_DEVICEID_SMART_TARNSPORT							0x0002

// Generic Device IDs
#define ZCL_HA_DEVICEID_ON_OFF_SWITCH                           0x0000
#define ZCL_HA_DEVICEID_LEVEL_CONTROL_SWITCH                    0x0001
#define ZCL_HA_DEVICEID_ON_OFF_OUTPUT                           0x0002
#define ZCL_HA_DEVICEID_LEVEL_CONTROLLABLE_OUTPUT               0x0003
#define ZCL_HA_DEVICEID_SCENE_SELECTOR                          0x0004
#define ZCL_HA_DEVICEID_CONFIGURATION_TOOL                      0x0005
#define ZCL_HA_DEVICEID_REMOTE_CONTROL                          0x0006
#define ZCL_HA_DEVICEID_COMBINED_INTERFACE                      0x0007
#define ZCL_HA_DEVICEID_RANGE_EXTENDER                          0x0008
#define ZCL_HA_DEVICEID_MAINS_POWER_OUTLET                      0x0009
#define ZCL_HA_DEVICEID_DOOR_LOCK                               0x000A
#define ZCL_HA_DEVICEID_DOOR_LOCK_CONTROLLER                    0x000B
#define ZCL_HA_DEVICEID_SIMPLE_SENSOR                           0x000C
#define ZCL_HA_DEVICEID_CONSUMPTION_AWARENESS_DEVICE            0x000D
#define ZCL_HA_DEVICEID_HOME_GATEWAY                            0x0050
#define ZCL_HA_DEVICEID_SMART_PLUG                              0x0051
#define ZCL_HA_DEVICEID_WHITE_GOODS                             0x0052
#define ZCL_HA_DEVICEID_METER_INTERFACE                         0x0053

// This is a reserved value which could be used for test purposes
#define ZCL_HA_DEVICEID_TEST_DEVICE                             0x00FF

// Lighting Device IDs
#define ZCL_HA_DEVICEID_ON_OFF_LIGHT                            0x0100
#define ZCL_HA_DEVICEID_DIMMABLE_LIGHT                          0x0101
#define ZCL_HA_DEVICEID_COLORED_DIMMABLE_LIGHT                  0x0102
#define ZCL_HA_DEVICEID_ON_OFF_LIGHT_SWITCH                     0x0103
#define ZCL_HA_DEVICEID_DIMMER_SWITCH                           0x0104
#define ZCL_HA_DEVICEID_COLOR_DIMMER_SWITCH                     0x0105
#define ZCL_HA_DEVICEID_LIGHT_SENSOR                            0x0106
#define ZCL_HA_DEVICEID_OCCUPANCY_SENSOR                        0x0107

// Closures Device IDs
#define ZCL_HA_DEVICEID_SHADE                                   0x0200
#define ZCL_HA_DEVICEID_SHADE_CONTROLLER                        0x0201
#define ZCL_HA_DEVICEID_WINDOW_COVERING_DEVICE                  0x0202
#define ZCL_HA_DEVICEID_WINDOW_COVERING_CONTROLLER              0x0203

// HVAC Device IDs
#define ZCL_HA_DEVICEID_HEATING_COOLING_UNIT                    0x0300
#define ZCL_HA_DEVICEID_THERMOSTAT                              0x0301
#define ZCL_HA_DEVICEID_TEMPERATURE_SENSOR                      0x0302
#define ZCL_HA_DEVICEID_PUMP                                    0x0303
#define ZCL_HA_DEVICEID_PUMP_CONTROLLER                         0x0304
#define ZCL_HA_DEVICEID_PRESSURE_SENSOR                         0x0305
#define ZCL_HA_DEVICEID_FLOW_SENSOR                             0x0306
#define ZCL_HA_DEVICEID_MINI_SPLIT_AC                           0x0307

// Intruder Alarm Systems (IAS) Device IDs
#define ZCL_HA_DEVICEID_IAS_CONTROL_INDICATING_EQUIPMENT        0x0400
#define ZCL_HA_DEVICEID_IAS_ANCILLARY_CONTROL_EQUIPMENT         0x0401
#define ZCL_HA_DEVICEID_IAS_ZONE                                0x0402
#define ZCL_HA_DEVICEID_IAS_WARNING_DEVICE                      0x0403

// Device type to display in LCD
#define ZCL_HA_DEVICE_COORDINATOR       0
#define ZCL_HA_DEVICE_ROUTER            1
#define ZCL_HA_DEVICE_END_DEVICE        2

// Zigbee Light Link Profile Identification
#define ZLL_PROFILE_ID                                     0xc05e

// ZLL Basic Lighting Device IDs
#define ZLL_DEVICEID_ON_OFF_LIGHT                          0x0000
#define ZLL_DEVICEID_ON_OFF_PLUG_IN_UNIT                   0x0010
#define ZLL_DEVICEID_DIMMABLE_LIGHT                        0x0100
#define ZLL_DEVICEID_DIMMABLE_PLUG_IN_UNIT                 0x0110

// ZLL Color Lighting Device IDs
#define ZLL_DEVICEID_COLOR_LIGHT                           0x0200
#define ZLL_DEVICEID_EXTENDED_COLOR_LIGHT                  0x0210
#define ZLL_DEVICEID_COLOR_TEMPERATURE_LIGHT               0x0220

// ZLL Lighting Remotes Device IDs
#define ZLL_DEVICEID_COLOR_CONTORLLER                      0x0800
#define ZLL_DEVICEID_COLOR_SCENE_CONTROLLER                0x0810
#define ZLL_DEVICEID_NON_COLOR_CONTORLLER                  0x0820
#define ZLL_DEVICEID_NON_COLOR_SCENE_CONTROLLER            0x0830
#define ZLL_DEVICEID_CONTROL_BRIDGE                        0x0840
#define ZLL_DEVICEID_ON_OFF_SENSOR                         0x0850


#define ZCL_CLUSTER_ID_GEN_BASIC                             0x0000
#define ZCL_CLUSTER_ID_GEN_POWER_CFG                         0x0001
#define ZCL_CLUSTER_ID_GEN_DEVICE_TEMP_CONFIG                0x0002
#define ZCL_CLUSTER_ID_GEN_IDENTIFY                          0x0003
#define ZCL_CLUSTER_ID_GEN_GROUPS                            0x0004
#define ZCL_CLUSTER_ID_GEN_SCENES                            0x0005
#define ZCL_CLUSTER_ID_GEN_ON_OFF                            0x0006
#define ZCL_CLUSTER_ID_GEN_ON_OFF_SWITCH_CONFIG              0x0007
#define ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL                     0x0008
#define ZCL_CLUSTER_ID_GEN_ALARMS                            0x0009
#define ZCL_CLUSTER_ID_GEN_TIME                              0x000A
#define ZCL_CLUSTER_ID_GEN_LOCATION                          0x000B
#define ZCL_CLUSTER_ID_GEN_ANALOG_INPUT_BASIC                0x000C
#define ZCL_CLUSTER_ID_GEN_ANALOG_OUTPUT_BASIC               0x000D
#define ZCL_CLUSTER_ID_GEN_ANALOG_VALUE_BASIC                0x000E
#define ZCL_CLUSTER_ID_GEN_BINARY_INPUT_BASIC                0x000F
#define ZCL_CLUSTER_ID_GEN_BINARY_OUTPUT_BASIC               0x0010
#define ZCL_CLUSTER_ID_GEN_BINARY_VALUE_BASIC                0x0011
#define ZCL_CLUSTER_ID_GEN_MULTISTATE_INPUT_BASIC            0x0012
#define ZCL_CLUSTER_ID_GEN_MULTISTATE_OUTPUT_BASIC           0x0013
#define ZCL_CLUSTER_ID_GEN_MULTISTATE_VALUE_BASIC            0x0014
#define ZCL_CLUSTER_ID_GEN_COMMISSIONING                     0x0015
#define ZCL_CLUSTER_ID_GEN_PARTITION                         0x0016


#define ZCL_CLUSTER_ID_OTA                                   0x0019

#define ZCL_CLUSTER_ID_GEN_POWER_PROFILE                     0x001A
#define ZCL_CLUSTER_ID_GEN_APPLIANCE_CONTROL                 0x001B

#define ZCL_CLUSTER_ID_GEN_POLL_CONTROL                      0x0020

#define ZCL_CLUSTER_ID_GREEN_POWER_PROXY                     0x0021

#define ZCL_CLUSTER_ID_CLOSURES_SHADE_CONFIG                 0x0100
#define ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK                    0x0101
#define ZCL_CLUSTER_ID_CLOSURES_WINDOW_COVERING              0x0102

#define ZCL_CLUSTER_ID_HVAC_PUMP_CONFIG_CONTROL              0x0200
#define ZCL_CLUSTER_ID_HVAC_THERMOSTAT                       0x0201
#define ZCL_CLUSTER_ID_HVAC_FAN_CONTROL                      0x0202
#define ZCL_CLUSTER_ID_HVAC_DIHUMIDIFICATION_CONTROL         0x0203
#define ZCL_CLUSTER_ID_HVAC_USER_INTERFACE_CONFIG            0x0204

#define ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL                0x0300
#define ZCL_CLUSTER_ID_LIGHTING_BALLAST_CONFIG               0x0301

#define ZCL_CLUSTER_ID_MS_ILLUMINANCE_MEASUREMENT            0x0400
#define ZCL_CLUSTER_ID_MS_ILLUMINANCE_LEVEL_SENSING_CONFIG   0x0401
#define ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT            0x0402
#define ZCL_CLUSTER_ID_MS_PRESSURE_MEASUREMENT               0x0403
#define ZCL_CLUSTER_ID_MS_FLOW_MEASUREMENT                   0x0404
#define ZCL_CLUSTER_ID_MS_RELATIVE_HUMIDITY                  0x0405
#define ZCL_CLUSTER_ID_MS_OCCUPANCY_SENSING                  0x0406

#define ZCL_CLUSTER_ID_SS_IAS_ZONE                           0x0500
#define ZCL_CLUSTER_ID_SS_IAS_ACE                            0x0501
#define ZCL_CLUSTER_ID_SS_IAS_WD                             0x0502

#define ZCL_CLUSTER_ID_PI_GENERIC_TUNNEL                     0x0600
#define ZCL_CLUSTER_ID_PI_BACNET_PROTOCOL_TUNNEL             0x0601
#define ZCL_CLUSTER_ID_PI_ANALOG_INPUT_BACNET_REG            0x0602
#define ZCL_CLUSTER_ID_PI_ANALOG_INPUT_BACNET_EXT            0x0603
#define ZCL_CLUSTER_ID_PI_ANALOG_OUTPUT_BACNET_REG           0x0604
#define ZCL_CLUSTER_ID_PI_ANALOG_OUTPUT_BACNET_EXT           0x0605
#define ZCL_CLUSTER_ID_PI_ANALOG_VALUE_BACNET_REG            0x0606
#define ZCL_CLUSTER_ID_PI_ANALOG_VALUE_BACNET_EXT            0x0607
#define ZCL_CLUSTER_ID_PI_BINARY_INPUT_BACNET_REG            0x0608
#define ZCL_CLUSTER_ID_PI_BINARY_INPUT_BACNET_EXT            0x0609
#define ZCL_CLUSTER_ID_PI_BINARY_OUTPUT_BACNET_REG           0x060A
#define ZCL_CLUSTER_ID_PI_BINARY_OUTPUT_BACNET_EXT           0x060B
#define ZCL_CLUSTER_ID_PI_BINARY_VALUE_BACNET_REG            0x060C
#define ZCL_CLUSTER_ID_PI_BINARY_VALUE_BACNET_EXT            0x060D
#define ZCL_CLUSTER_ID_PI_MULTISTATE_INPUT_BACNET_REG        0x060E
#define ZCL_CLUSTER_ID_PI_MULTISTATE_INPUT_BACNET_EXT        0x060F
#define ZCL_CLUSTER_ID_PI_MULTISTATE_OUTPUT_BACNET_REG       0x0610
#define ZCL_CLUSTER_ID_PI_MULTISTATE_OUTPUT_BACNET_EXT       0x0611
#define ZCL_CLUSTER_ID_PI_MULTISTATE_VALUE_BACNET_REG        0x0612
#define ZCL_CLUSTER_ID_PI_MULTISTATE_VALUE_BACNET_EXT        0x0613
#define ZCL_CLUSTER_ID_PI_11073_PROTOCOL_TUNNEL              0x0614

#define ZCL_CLUSTER_ID_SE_PRICING                            0x0700
#define ZCL_CLUSTER_ID_SE_LOAD_CONTROL                       0x0701
#define ZCL_CLUSTER_ID_SE_SIMPLE_METERING                    0x0702
#define ZCL_CLUSTER_ID_SE_MESSAGE                            0x0703
#define ZCL_CLUSTER_ID_SE_SE_TUNNELING                       0x0704
#define ZCL_CLUSTER_ID_SE_PREPAYMENT                         0x0705
#define ZCL_CLUSTER_ID_SE_ENERGY_MGMT                        0x0706
#define ZCL_CLUSTER_ID_SE_TOU_CALENDAR                       0x0707
#define ZCL_CLUSTER_ID_SE_DEVICE_MGMT                        0x0708
#define ZCL_CLUSTER_ID_SE_EVENTS                             0x0709
#define ZCL_CLUSTER_ID_SE_MDU_PAIRING                        0x070A

#define ZCL_CLUSTER_ID_GEN_KEY_ESTABLISHMENT                 0x0800

#define ZCL_CLUSTER_ID_HA_APPLIANCE_IDENTIFICATION           0x0B00
#define ZCL_CLUSTER_ID_HA_METER_IDENTIFICATION               0x0B01
#define ZCL_CLUSTER_ID_HA_APPLIANCE_EVENTS_ALERTS            0x0B02
#define ZCL_CLUSTER_ID_HA_APPLIANCE_STATISTICS               0x0B03
#define ZCL_CLUSTER_ID_HA_ELECTRICAL_MEASUREMENT             0x0B04
#define ZCL_CLUSTER_ID_HA_DIAGNOSTIC                         0x0B05

//Data type 

#define ZCL_DATATYPE_NO_DATA                            0x00
#define ZCL_DATATYPE_DATA8                              0x08
#define ZCL_DATATYPE_DATA16                             0x09
#define ZCL_DATATYPE_DATA24                             0x0a
#define ZCL_DATATYPE_DATA32                             0x0b
#define ZCL_DATATYPE_DATA40                             0x0c
#define ZCL_DATATYPE_DATA48                             0x0d
#define ZCL_DATATYPE_DATA56                             0x0e
#define ZCL_DATATYPE_DATA64                             0x0f
#define ZCL_DATATYPE_BOOLEAN                            0x10
#define ZCL_DATATYPE_BITMAP8                            0x18
#define ZCL_DATATYPE_BITMAP16                           0x19
#define ZCL_DATATYPE_BITMAP24                           0x1a
#define ZCL_DATATYPE_BITMAP32                           0x1b
#define ZCL_DATATYPE_BITMAP40                           0x1c
#define ZCL_DATATYPE_BITMAP48                           0x1d
#define ZCL_DATATYPE_BITMAP56                           0x1e
#define ZCL_DATATYPE_BITMAP64                           0x1f
#define ZCL_DATATYPE_UINT8                              0x20
#define ZCL_DATATYPE_UINT16                             0x21
#define ZCL_DATATYPE_UINT24                             0x22
#define ZCL_DATATYPE_UINT32                             0x23
#define ZCL_DATATYPE_UINT40                             0x24
#define ZCL_DATATYPE_UINT48                             0x25
#define ZCL_DATATYPE_UINT56                             0x26
#define ZCL_DATATYPE_UINT64                             0x27
#define ZCL_DATATYPE_INT8                               0x28
#define ZCL_DATATYPE_INT16                              0x29
#define ZCL_DATATYPE_INT24                              0x2a
#define ZCL_DATATYPE_INT32                              0x2b
#define ZCL_DATATYPE_INT40                              0x2c
#define ZCL_DATATYPE_INT48                              0x2d
#define ZCL_DATATYPE_INT56                              0x2e
#define ZCL_DATATYPE_INT64                              0x2f
#define ZCL_DATATYPE_ENUM8                              0x30
#define ZCL_DATATYPE_ENUM16                             0x31
#define ZCL_DATATYPE_SEMI_PREC                          0x38
#define ZCL_DATATYPE_SINGLE_PREC                        0x39
#define ZCL_DATATYPE_DOUBLE_PREC                        0x3a
#define ZCL_DATATYPE_OCTET_STR                          0x41
#define ZCL_DATATYPE_CHAR_STR                           0x42
#define ZCL_DATATYPE_LONG_OCTET_STR                     0x43
#define ZCL_DATATYPE_LONG_CHAR_STR                      0x44
#define ZCL_DATATYPE_ARRAY                              0x48
#define ZCL_DATATYPE_STRUCT                             0x4c
#define ZCL_DATATYPE_SET                                0x50
#define ZCL_DATATYPE_BAG                                0x51
#define ZCL_DATATYPE_TOD                                0xe0
#define ZCL_DATATYPE_DATE                               0xe1
#define ZCL_DATATYPE_UTC                                0xe2
#define ZCL_DATATYPE_CLUSTER_ID                         0xe8
#define ZCL_DATATYPE_ATTR_ID                            0xe9
#define ZCL_DATATYPE_BAC_OID                            0xea
#define ZCL_DATATYPE_IEEE_ADDR                          0xf0
#define ZCL_DATATYPE_128_BIT_SEC_KEY                    0xf1
#define ZCL_DATATYPE_UNKNOWN                            0xff

/*****************************************************************************/
/***    Illuminance Measurement Cluster Attributes                         ***/
/*****************************************************************************/
    // Illuminance Measurement Information attribute set
#define ATTRID_MS_ILLUMINANCE_MEASURED_VALUE                             0x0000
#define ATTRID_MS_ILLUMINANCE_MIN_MEASURED_VALUE                         0x0001
#define ATTRID_MS_ILLUMINANCE_MAX_MEASURED_VALUE                         0x0002
#define ATTRID_MS_ILLUMINANCE_TOLERANCE                                  0x0003
#define ATTRID_MS_ILLUMINANCE_LIGHT_SENSOR_TYPE                          0x0004

    // Illuminance Measurement Settings attribute set
// #define ATTRID_MS_ILLUMINANCE_MIN_PERCENT_CHANGE                         0x0100
// #define ATTRID_MS_ILLUMINANCE_MIN_ABSOLUTE_CHANGE                        0x0101

  /*** Light Sensor Type attribute values ***/
#define MS_ILLUMINANCE_LIGHT_SENSOR_PHOTODIODE                           0x00
#define MS_ILLUMINANCE_LIGHT_SENSOR_CMOS                                 0x01
#define MS_ILLUMINANCE_LIGHT_SENSOR_UNKNOWN                              0xFF

/*****************************************************************************/
/***    Illuminance Level Sensing Configuration Cluster Attributes         ***/
/*****************************************************************************/
    // Illuminance Level Sensing Information attribute set
#define ATTRID_MS_ILLUMINANCE_LEVEL_STATUS                               0x0000
#define ATTRID_MS_ILLUMINANCE_LEVEL_LIGHT_SENSOR_TYPE                    0x0001

/***  Level Status attribute values  ***/
#define MS_ILLUMINANCE_LEVEL_ON_TARGET                                   0x00
#define MS_ILLUMINANCE_LEVEL_BELOW_TARGET                                0x01
#define MS_ILLUMINANCE_LEVEL_ABOVE_TARGET                                0x02

/***  Light Sensor Type attribute values  ***/
#define MS_ILLUMINANCE_LEVEL_LIGHT_SENSOR_PHOTODIODE                     0x00
#define MS_ILLUMINANCE_LEVEL_LIGHT_SENSOR_CMOS                           0x01
#define MS_ILLUMINANCE_LEVEL_LIGHT_SENSOR_UNKNOWN                        0xFF

    // Illuminance Level Sensing Settings attribute set
#define ATTRID_MS_ILLUMINANCE_TARGET_LEVEL                               0x0010

/*****************************************************************************/
/***    Temperature Measurement Cluster Attributes                         ***/
/*****************************************************************************/
  // Temperature Measurement Information attributes set
#define ATTRID_MS_TEMPERATURE_MEASURED_VALUE                             0x0000 // M, R, INT16
#define ATTRID_MS_TEMPERATURE_MIN_MEASURED_VALUE                         0x0001 // M, R, INT16
#define ATTRID_MS_TEMPERATURE_MAX_MEASURED_VALUE                         0x0002 // M, R, INT16
#define ATTRID_MS_TEMPERATURE_TOLERANCE                                  0x0003 // O, R, UINT16

  // Temperature Measurement Settings attributes set
#define ATTRID_MS_TEMPERATURE_MIN_PERCENT_CHANGE                         0x0010
#define ATTRID_MS_TEMPERATURE_MIN_ABSOLUTE_CHANGE                        0x0011

/*****************************************************************************/
/***    Pressure Measurement Cluster Attributes                            ***/
/*****************************************************************************/
  // Pressure Measurement Information attribute set
#define ATTRID_MS_PRESSURE_MEASUREMENT_MEASURED_VALUE                    0x0000
#define ATTRID_MS_PRESSURE_MEASUREMENT_MIN_MEASURED_VALUE                0x0001
#define ATTRID_MS_PRESSURE_MEASUREMENT_MAX_MEASURED_VALUE                0x0002
#define ATTRID_MS_PRESSURE_MEASUREMENT_TOLERANCE                         0x0003

  // Pressure Measurement Settings attribute set
// #define ATTRID_MS_PRESSURE_MEASUREMENT_MIN_PERCENT_CHANGE                0x0100
// #define ATTRID_MS_PRESSURE_MEASUREMENT_MIN_ABSOLUTE_CHANGE               0x0101

/*****************************************************************************/
/***        Flow Measurement Cluster Attributes                            ***/
/*****************************************************************************/
  // Flow Measurement Information attribute set
#define ATTRID_MS_FLOW_MEASUREMENT_MEASURED_VALUE                        0x0000
#define ATTRID_MS_FLOW_MEASUREMENT_MIN_MEASURED_VALUE                    0x0001
#define ATTRID_MS_FLOW_MEASUREMENT_MAX_MEASURED_VALUE                    0x0002
#define ATTRID_MS_FLOW_MEASUREMENT_TOLERANCE                             0x0003

  // Flow Measurement Settings attribute set
// #define ATTRID_MS_FLOW_MEASUREMENT_MIN_PERCENT_CHANGE                    0x0100
// #define ATTRID_MS_FLOW_MEASUREMENT_MIN_ABSOLUTE_CHANGE                   0x0101

/*****************************************************************************/
/***        Relative Humidity Cluster Attributes                           ***/
/*****************************************************************************/
  // Relative Humidity Information attribute set
#define ATTRID_MS_RELATIVE_HUMIDITY_MEASURED_VALUE                       0x0000
#define ATTRID_MS_RELATIVE_HUMIDITY_MIN_MEASURED_VALUE                   0x0001
#define ATTRID_MS_RELATIVE_HUMIDITY_MAX_MEASURED_VALUE                   0x0002
#define ATTRID_MS_RELATIVE_HUMIDITY_TOLERANCE                            0x0003

/*****************************************************************************/
/***         Occupancy Sensing Cluster Attributes                          ***/
/*****************************************************************************/
    // Occupancy Sensor Configuration attribute set
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_OCCUPANCY                     0x0000 // M, R, BITMAP8
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_OCCUPANCY_SENSOR_TYPE         0x0001 // M, R, ENUM8

/*** Occupancy Sensor Type Attribute values ***/
#define MS_OCCUPANCY_SENSOR_TYPE_PIR                                     0x00
#define MS_OCCUPANCY_SENSOR_TYPE_ULTRASONIC                              0x01
#define MS_OCCUPANCY_SENSOR_TYPE_PIR_AND_ULTRASONIC                      0x02

    // PIR Configuration attribute set
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_PIR_O_TO_U_DELAY              0x0010 // O, R/W, UINT16
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_PIR_U_TO_O_DELAY              0x0011 // O, R/W, UINT16
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_PIR_U_TO_O_THRESH             0x0012 // O, R/W, UINT8


#define ALINK_CLUSTERID             0xfe01


#define ALINK_ATTRIBUTEID_NEIGHBORTABLESIZE   0x0000
#define ALINK_ATTRIBUTEID_CHILDTABLESIZE    0x1000
#define ALINK_RSSI                0x2001

#define ALINK_CLUSTERID_PM2_5             0xfe02
#define ALINK_ATTRID_PM2_5_MEASURE                    0x0000
#define ALINK_ATTRID_PM2_5_ATTRID_MINMEASURE           0x0001
#define ALINK_ATTRID_PM2_5_ATTRID_MAXMEASURE           0x0002
#define ALINK_ATTRID_PM2_5_ATTRID_TOLERANCE            0x0003 



#define ALINK_CLUSTERID_C02                 0xfe03
#define ALINK_ATTRID_C02_MEASURE                    0x0000
#define ALINK_ATTRID_C02_ATTRID_MINMEASURE           0x0001
#define ALINK_ATTRID_C02_ATTRID_MAXMEASURE           0x0002
#define ALINK_ATTRID_C02_ATTRID_TOLERANCE            0x0003 

#define ALINK_CLUSTERID_FORMALDEHYDE              0xfe04
#define ALINK_ATTRID_FORMALDEHYDE_MEASURE                   0x0000
#define ALINK_ATTRID_FORMALDEHYDE_ATTRID_MINMEASURE           0x0001
#define ALINK_ATTRID_FORMALDEHYDE_ATTRID_MAXMEASURE           0x0002
#define ALINK_ATTRID_FORMALDEHYDE_ATTRID_TOLERANCE            0x0003 



#define MAX_NAME_LEN					32
#define MAX_SENSOR_NUM_PER_DEVICE   10
#define MAX_SENSOR_DATA_LEN			20


/*** Lock State Attribute types ***/
#define CLOSURES_LOCK_STATE_NOT_FULLY_LOCKED			0x00	
#define CLOSURES_LOCK_STATE_LOCKED						0x01	
#define CLOSURES_LOCK_STATE_UNLOCKED					0x02	

#define INFRARED_REPEAT_WORKMODE_LEARNING			0x01
#define INFRARED_REPEAT_WORKMODE_CONTROL			0x02

#define MAX_DATA_LEN						100

#define MAX_ATTRI_DATA_LEN						20

#define INVALID_DEVICE_STATE_DATA					0xFF
		
#define MAX_MANUFACTURERNAME_LEN				20

#define MAX_ATTRI_NAME_LEN				32


#define MAX_ATTRIBUTES_PER_DEVICE		10

#define EXT_ADDR_LEN          8

#define MAX_GROUP_ATTRNAME_LEN    10
#define MAX_GROUP_MEMBERS_NUM    400
#define MAX_GROUP_ATTR_NUM    400

#ifndef BREAK_UINT64
#define BREAK_UINT64( var, ByteNum ) \
          (uint8)((uint64_t)(((var) >>((ByteNum) * 8)) & 0x00FF))
#endif
/* takes a byte out of a uint32 : var - uint32,  ByteNum - byte to take out (0 - 3) */
#ifndef BREAK_UINT32
#define BREAK_UINT32( var, ByteNum ) \
          (uint8_t)((uint32_t)(((var) >>((ByteNum) * 8)) & 0x00FF))

#endif

#ifndef BUILD_UINT32
#define BUILD_UINT32(Byte0, Byte1, Byte2, Byte3) \
          ((uint32_t)((uint32_t)((Byte0) & 0x00FF) \
          + ((uint32_t)((Byte1) & 0x00FF) << 8) \
          + ((uint32_t)((Byte2) & 0x00FF) << 16) \
          + ((uint32_t)((Byte3) & 0x00FF) << 24)))
#endif

#ifndef BUILD_UINT16
#define BUILD_UINT16(loByte, hiByte) \
          ((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))
#endif

#ifndef HI_UINT16
#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#endif
#ifndef LO_UINT16
#define LO_UINT16(a) ((a) & 0xFF)
#endif

#ifndef BUILD_UINT8
#define BUILD_UINT8(hiByte, loByte) \
          ((uint8_t)(((loByte) & 0x0F) + (((hiByte) & 0x0F) << 4)))
#endif

#ifndef HI_UINT8
#define HI_UINT8(a) (((a) >> 4) & 0x0F)
#endif
#ifndef LO_UINT8
#define LO_UINT8(a) ((a) & 0x0F)
#endif

typedef struct rule_timeout_str_s {
	struct rule_timeout_str_s * next;
  struct uloop_timeout timeout;
	uint16_t rule_id;
} rule_timeout_str_t;


struct rule_mnt_t {
	struct avl_tree hdr;
	unsigned int count;
  rule_timeout_str_t *rule_timeout_str;
};


#define MAX_ATTRIBUTE_SIZE	50
/* Attribute tuple */
typedef struct
{
	bool valid;
	uint16_t attr_id;
	uint8_t attr_type;
	uint8_t attr_val[MAX_ATTRIBUTE_SIZE];
  char attr_name[32];
} attribute_info_t;

/* Attribute information for a particular Cluster */
#define MAX_ATTRIBUTES		10
typedef struct
{
	uint16_t cluster_id;
	uint8_t	num_attributes;
	attribute_info_t attribute_list[MAX_ATTRIBUTES];
} cluster_info_t;



extern struct rule_mnt_t	rule_mnt;

extern pthread_mutex_t		rule_mutex;

typedef struct {
	uint8 ieeeAddr[EXT_ADDR_LEN];
	uint8 extPanId[EXT_ADDR_LEN];
	uint8 channel;
	uint16 panId;
  uint8 nwkKey[16];
	uint8 permitJoin;
	uint8 whitelistEnable;
}coordInfo_t ;


typedef struct
{
	uint8_t ieeeAddr[EXT_ADDR_LEN];
	uint8_t ManufacturerName_len;
	char ManufacturerName[20] ;
	uint8_t LocationDescription_len;
	char LocationDescription[20];
	uint8_t HWversion;
	uint8_t SWversion;
	uint8_t endpointId;
	uint16_t deviceId;
	uint16_t profileId;
	uint8_t online;
}deviceBasic_t;

typedef struct
{
  uint8_t zoneId;
  uint16_t zoneType;
  uint16_t status;
} zoneState_t;


typedef struct
{
  uint8_t on;
  uint8_t bri;
  uint8_t hue;
  uint8_t sat;
  uint16_t colortemp;
} lightState_t;

typedef struct
{
  uint8_t status;
} onoffState_t;


typedef struct
{
	uint16_t clusterId;
	uint16_t attrId;
	uint8_t dataType;
	uint8_t data[MAX_SENSOR_DATA_LEN];
} sensor_t;


typedef struct
{
	uint16_t clusterId;
	uint16_t attrId;
	uint8_t dataType;
	uint8_t data[MAX_SENSOR_DATA_LEN];
} rawAttr_t;




typedef struct
{
	uint8_t sensorNum;
	sensor_t sensor[MAX_SENSOR_NUM_PER_DEVICE];
} simpleSensor_t;

typedef struct
{
	uint8_t attrNum;
	rawAttr_t attr[MAX_SENSOR_NUM_PER_DEVICE];
} rawAttrs_t;



typedef struct
{
	uint8_t dataLen;
	uint8_t data[MAX_DATA_LEN];
} rawData_t;


typedef struct
{
	uint8_t percentage;
} percentage_t;




typedef union
{
  onoffState_t onoffState;
  lightState_t lightState;
  zoneState_t zoneState;
  simpleSensor_t simpleSensor;
  rawData_t rawData;
  percentage_t percentage;
  rawAttrs_t rawAttrs;
} deviceState_t;


typedef struct
{
	deviceBasic_t  deviceBasic;
	deviceState_t  deviceState;
} deviceInfo_t;

typedef struct attrInfo_s
{
  char *attrName;
  char *attrValue;
  struct attrInfo_s *next;
} attrInfo_t;


typedef struct deviceInfoString_s
{
	uint8_t ieeeAddr[EXT_ADDR_LEN];
	uint8_t endpointId;
	uint16_t deviceId;
	uint16_t profileId;
	uint8_t online;
  attrInfo_t *attrInfo;
  struct deviceInfoString_s *next;
} deviceInfoString_t;



typedef struct
{
	uint8_t ieeeAddr[EXT_ADDR_LEN];
  uint8_t endpointId;
  uint16_t profileId;
  uint16_t deviceId;
  uint8_t status;
} groupMemberInfo_t;


typedef struct
{
  char attrName[MAX_GROUP_ATTRNAME_LEN];
  uint8_t data[MAX_ATTRI_DATA_LEN];
} groupAttrInfo_t;


typedef struct
{
	char  name[MAX_NAME_LEN];
	uint16_t id;
	uint8_t	 visible;
  groupMemberInfo_t groupMember[MAX_GROUP_MEMBERS_NUM];
  uint16_t groupMembersNum;
  groupAttrInfo_t groupAttr[MAX_GROUP_ATTR_NUM];
  uint16_t groupAttrsNum;
} groupInfo_t;


#define MAX_SCENE_MEMBER_NUM   100

typedef struct
{
	char name[MAX_NAME_LEN];
	uint8_t id;
  uint16_t groupId;
  uint16_t iconId;
  deviceInfo_t sceneMember[MAX_SCENE_MEMBER_NUM];
  uint16_t sceneMembersNum;
} sceneInfo_t;



#define COND_TYPE_TIMER       1
#define COND_TYPE_TRIGGER     2

struct shuncom_tm {
  uint8 tm_sec;
  uint8 tm_min;
  uint8 tm_hour;
  uint8 tm_mday;
  uint8 tm_mon;
  uint16 tm_year;
  uint8 tm_wday;
  uint16 tm_yday;
};


typedef struct rule_conditions_s {
	struct rule_conditions_s * next;
	/*
	 * 1 timer
	 * 2 trigger
	 * */
  uint8_t conditionIdx;
	uint8_t times_triggered;
	uint16_t cond_type;
	union{
		struct shuncom_tm act_time;
		struct {
			/* source device IEEE address */
			uint8_t ieeeAddr[EXT_ADDR_LEN];
			uint8_t endpointId;
			/*
			 * -1 less than
			 *  0 equal
			 *  1  great than
			 *  2 change
			 *  */
			int operater_type;
      char attrName[MAX_ATTRI_NAME_LEN];
      char value[MAX_ATTRI_DATA_LEN];
		}triggered_t;
	}rule_cond;
}rule_conditions_t;

typedef struct {
	struct avl_node avl;
  uint8_t targetId[EXT_ADDR_LEN +1];
  struct uloop_timeout timeout;
  uint16_t ruleId;
  uint8_t actionIdx;
}rule_delayAction_t;



typedef struct rule_actions_s {
	struct rule_actions_s * next;
  uint8_t actionIdx;
  uint16_t delay_timeout;
	/*
	 * 1 device
	 * 2 group
	 * 3 scene
	 * 4 rule
	 * */
	uint8_t target_type;
	union {
		uint8_t ieeeAddr[EXT_ADDR_LEN];
		uint16_t nwkAddr;
		uint8_t scene_id;
		uint16_t ruleId;
	}target_id;

	uint8_t target_ep;
	/*
	 * 1 on
	 * 2 bri
	 * 3 hue
	 * 4 sat
	 * 5 color temp
	 * */
  char attrName[MAX_ATTRI_NAME_LEN];
  char value[MAX_ATTRI_DATA_LEN];
}rule_actions_t;

struct rule {
	struct avl_node avl;
	char  name[MAX_NAME_LEN];
	uint16_t id;
	/*
	 * 1 enabled
	 * 0 disabled
	 * */
	uint8_t	 state;
	struct tm create_time;
	struct tm last_triggered;
	/*
	 * the times of the rule be triggered
	 * */
	uint8_t times_triggered;
  char *conditions_expression;
	rule_conditions_t  *conditions;
	rule_actions_t	   *actions;
};

typedef struct rule rule_t_t;

extern uint16_t nwk_coordHeartBeatInterval ;

extern uint16_t minReportInterval ;
extern uint16_t maxReportInterval ;

extern uint16_t zigbeeModuleVersion;

extern char zigbeeModuleDate[12];

#ifdef __cplusplus
}
#endif


#endif
