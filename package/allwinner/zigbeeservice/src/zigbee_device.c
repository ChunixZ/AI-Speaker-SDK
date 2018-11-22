/*************************************************************************
    > File Name: zigbee_device.c
    > Author: zyc
    > Mail: zyc@imio.ai 
    > Created Time: 2017/06/30
 ************************************************************************/
#include "include/zigbee_device.h"
#include <stdio.h>
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
#include "include/device_json.h"


int setdevice_status;
extern bool is_sub_device_action_exec_report_req;
extern int bind_sumdevices;
extern bool is_onbind;


attribute_info BASIC[] = {
    {ATTRID_BASIC_HW_VERSION,TYPE_BYTE,VISIBLE,"hver"},
    {ATTRID_BASIC_ZCL_VERSION,TYPE_BYTE,VISIBLE,"sver"},
    {ATTRID_BASIC_MANUFACTURER_NAME,TYPE_STRING,VISIBLE,"fac"},
    {ATTRID_BASIC_MODEL_ID,TYPE_STRING,VISIBLE,"dsp"},
    {ATTRID_BASIC_DATE_CODE,TYPE_STRING,VISIBLE,"date"},
    {ATTRID_BASIC_POWER_SOURCE,TYPE_BYTE,VISIBLE,"ps"},
    {ATTRID_BASIC_SW_BUILD_ID,TYPE_STRING,VISIBLE,"swid"},
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info ON_OFF[] = {
    {ATTRID_ON_OFF,TYPE_BOOL,VISIBLE,"on"},
    {0x8000,TYPE_BOOL,VISIBLE,"childlock"},
    {0x8001,TYPE_BYTE,VISIBLE,"backlight"},
    {0xf000,TYPE_BYTE,VISIBLE,"plug"},
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info ON_OFF_SWITCH_CONFIG[] = {
    {ATTRID_ON_OFF_SWITCH_TYPE,TYPE_BOOL,VISIBLE,"on"},
    {ATTRID_ON_OFF_SWITCH_ACTIONS,TYPE_BYTE,VISIBLE,"act"},
    {0,0,VISIBLE,END_FLAG_BUF}
};
    
attribute_info SE_SIMPLE_METERING[] = {
    {ATTRID_SE_METERING_CURR_SUMM_DLVD,TYPE_LL,VISIBLE,"energy"},
    {ATTRID_SE_METERING_STATUS,TYPE_LL,VISIBLE,"sta"},
    {ATTRID_SE_METERING_UOM,TYPE_BYTE,VISIBLE,"unit"},
    {ATTRID_SE_METERING_MULT,TYPE_THREE,VISIBLE,"mult"},
    {ATTRID_SE_METERING_DIV,TYPE_THREE,VISIBLE,"div"},
    {ATTRID_SE_METERING_SUMM_FMTG,TYPE_BYTE,VISIBLE,"sum"},
    {ATTRID_SE_METERING_DEVICE_TYPE,TYPE_BYTE,VISIBLE,"type"},
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info HA_ELECTRICAL_MEASUREMENT[] = {
    {ATTRID_ELECTRICAL_MEASUREMENT_RMS_VOLTAGE,TYPE_SHORT,VISIBLE,"volt"},
    {ATTRID_ELECTRICAL_MEASUREMENT_RMS_CURRENT,TYPE_SHORT,VISIBLE,"curr"},
    {ATTRID_ELECTRICAL_MEASUREMENT_ACTIVE_POWER,TYPE_SHORT,VISIBLE,"actp"},
    {ATTRID_ELECTRICAL_MEASUREMENT_REACTIVE_POWER,TYPE_SHORT,VISIBLE,"reactp"},
    {ATTRID_ELECTRICAL_MEASUREMENT_POWER_FACTOR,TYPE_BYTE,VISIBLE,"facp"},
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info HVAC_FAN_CONTROL[] = {
    {ATTRID_HVAC_FAN_CTRL_FAN_MODE,TYPE_BYTE,VISIBLE,"fanmode"},
    {ATTRID_HVAC_FAN_CTRL_FAN_SEQUENCE,TYPE_BYTE,VISIBLE,"fanseq"},
    {0x8000,TYPE_BYTE,VISIBLE,"powermode"},
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info MS_TEMPERATURE_MEASUREMENT[] = {
    {ATTRID_MS_TEMPERATURE_MEASURED_VALUE,TYPE_SHORT_STRING,VISIBLE,"meatemp"},
    {ATTRID_MS_TEMPERATURE_MIN_MEASURED_VALUE,TYPE_SHORT_STRING,VISIBLE,"minmeatemp"},
    {ATTRID_MS_TEMPERATURE_MAX_MEASURED_VALUE,TYPE_SHORT_STRING,VISIBLE,"maxmeatemp"},
    {0,0,VISIBLE,END_FLAG_BUF}
};
          
attribute_info MS_RELATIVE_HUMIDITY[] = {
    {ATTRID_MS_RELATIVE_HUMIDITY_MEASURED_VALUE,TYPE_SHORT_STRING,VISIBLE,"humi"},
    {ATTRID_MS_RELATIVE_HUMIDITY_MIN_MEASURED_VALUE,TYPE_SHORT_STRING,VISIBLE,"minhumi"},
    {ATTRID_MS_RELATIVE_HUMIDITY_MAX_MEASURED_VALUE,TYPE_SHORT_STRING,VISIBLE,"maxhumi"},
    {ATTRID_MS_RELATIVE_HUMIDITY_TOLERANCE,TYPE_SHORT,VISIBLE,"allowerror"},
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info GEN_ANALOG_INPUT_BASIC[] = {
    {ATTRID_IOV_BASIC_STATE_TEXT,TYPE_SHORT,VISIBLE,"pm25"},
    {ATTRID_IOV_BASIC_STATE_TEXT,TYPE_SHORT,VISIBLE,"voc"},//////////ep
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info SS_IAS_ZONE[] = {
    {ATTRID_SS_IAS_ZONE_STATUS,TYPE_SHORT,VISIBLE,"sta"},
    {ATTRID_SS_IAS_ZONE_TYPE,TYPE_SHORT,VISIBLE,"type"},
    {ATTRID_SS_ZONE_ID,TYPE_BYTE,VISIBLE,"zid"},
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info SS_IAS_WD[] = {
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info MS_ILLUMINANCE_MEASUREMENT[] = {
    {ATTRID_MS_ILLUMINANCE_MEASURED_VALUE,TYPE_SHORT_STRING,VISIBLE,"nlux"},
    {ATTRID_MS_ILLUMINANCE_MIN_MEASURED_VALUE,TYPE_SHORT,VISIBLE,"minlux"},
    {ATTRID_MS_ILLUMINANCE_MAX_MEASURED_VALUE,TYPE_SHORT,VISIBLE,"maxlux"},
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info MS_ILLUMINANCE_LEVEL_SENSING_CONFIG[] = {
    {ATTRID_MS_ILLUMINANCE_LEVEL_STATUS,TYPE_BYTE,VISIBLE,"llux"},
    {ATTRID_MS_ILLUMINANCE_TARGET_LEVEL,TYPE_SHORT_STRING,VISIBLE,"tlux"},
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info GEN_POWER_CFG[] = {
    {ATTRID_POWER_CFG_BAT_SIZE,TYPE_BYTE,VISIBLE,"bats"},
    {ATTRID_POWER_CFG_MAINS_ALARM_MASK,TYPE_BYTE,VISIBLE,"alm"},
    {0x0021,TYPE_BYTE,VISIBLE,"batpt"},
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info CLOSURES_DOOR_LOCK[] = {
    {ATTRID_CLOSURES_LOCK_STATE,TYPE_BYTE,VISIBLE,"sta"},
    {ATTRID_CLOSURES_LOCK_TYPE,TYPE_BYTE,VISIBLE,"type"},
    {ATTRID_CLOSURES_ACTUATOR_ENABLED,TYPE_BOOL,VISIBLE,"en"},
    {0x0040,TYPE_SHORT,VISIBLE,"alm"},
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info HVAC_THERMOSTAT[] = {
    {ATTRID_HVAC_THERMOSTAT_ALAM_MASK,TYPE_BYTE,VISIBLE,"tempalm"},
    {ATTRID_HVAC_THERMOSTAT_LOCAL_TEMPERATURE,TYPE_SHORT_STRING,VISIBLE,"tgtemp"},
    {ATTRID_HVAC_THERMOSTAT_OCCUPIED_COOLING_SETPOINT,TYPE_SHORT_STRING,VISIBLE,"coolset"},
    {ATTRID_HVAC_THERMOSTAT_OCCUPIED_HEATING_SETPOINT,TYPE_SHORT_STRING,VISIBLE,"heatset"},
    {0x8000,TYPE_BYTE,VISIBLE,"NumOfAirconditions"},
    {0x001c,TYPE_BYTE,VISIBLE,"workmode"},
    {0,0,VISIBLE,END_FLAG_BUF}
};
         
attribute_info GEN_LEVEL_CONTROL[] = {
    {ATTRID_LEVEL_CURRENT_LEVEL,TYPE_BYTE,VISIBLE,"bri"},
    {ATTRID_LEVEL_REMAINING_TIME,TYPE_SHORT,VISIBLE,"remaintime"},
    {ATTRID_LEVEL_ON_OFF_TRANSITION_TIME,TYPE_SHORT,VISIBLE,"onofftran"},
    {ATTRID_LEVEL_ON_LEVEL,TYPE_BYTE,VISIBLE,"lonl"},
    {ATTRID_LEVEL_ON_TRANSITION_TIME,TYPE_SHORT,VISIBLE,"opntran"},
    {ATTRID_LEVEL_OFF_TRANSITION_TIME,TYPE_SHORT,VISIBLE,"clstran"},
    {ATTRID_LEVEL_DEFAULT_MOVE_RATE,TYPE_BYTE,VISIBLE,"rate"},
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info CLOSURES_WINDOW_COVERING[] = {
    {ATTRID_CLOSURES_CURRENT_POSITION_LIFT_PERCENTAGE,TYPE_BYTE,VISIBLE,"pt"},
    {ATTRID_CLOSURES_WINDOW_COVERING_TYPE,TYPE_BYTE,VISIBLE,"type"},
    {ATTRID_CLOSURES_WINDOW_COVERING_MODE,TYPE_BYTE,VISIBLE,"workmode"},
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info LIGHTING_COLOR_CONTROL[] = {
    {0x0000,TYPE_BYTE,VISIBLE,"hue"},
    {0x0001,TYPE_BYTE,VISIBLE,"sat"},
    {0x0007,TYPE_SHORT,VISIBLE,"ctp"},
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info AL_FE01[] = {
   {0X2001,TYPE_BYTE,VISIBLE,"lqi"},
   {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info AL_FE02[] = {
   {0X0000,TYPE_SHORT,VISIBLE,"pm25"},
   {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info AL_FE03[] = {
   {0X0000,TYPE_SHORT,VISIBLE,"CO2"},
   {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info AL_FE04[] = {
   {0X0000,TYPE_SHORT,VISIBLE,"formaldehyde"},
   {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info AL_FE05[] = {
    {0X0000,TYPE_BYTE,VISIBLE,"val"},
    {0,0,VISIBLE,END_FLAG_BUF}
};


attribute_info APPLIANCE_CONTROL[] = {
    {ATTRID_APPLIANCE_CONTROL_START_TIME,TYPE_SHORT,VISIBLE,"starttime"},
    {0,0,VISIBLE,END_FLAG_BUF}
};


attribute_info OCCUPANCY_SENSING[] = {
    {ATTRID_MS_OCCUPANCY_SENSING_CONFIG_OCCUPANCY,TYPE_BYTE,VISIBLE,"occupancy"},
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info BINARY_INPUT_BASIC[] = {
    {0x0000,TYPE_BYTE,VISIBLE,"plugval2"},
    {0,0,VISIBLE,END_FLAG_BUF}
};


attribute_info ANALOG_INPUT_BASIC[] = {
    {0x0000,TYPE_BYTE,VISIBLE,"plugval1"},
    {0x000e,TYPE_SHORT,VISIBLE,"ANAINPUT0e"},
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info IDENTIFY[] = {
    {ATTRID_IDENTIFY_TIME,TYPE_SHORT,VISIBLE,"identifytime"},
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info GROUPS[] = {
    {ATTRID_GROUP_NAME_SUPPORT,TYPE_BYTE,VISIBLE,"namesupport"},
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info SCENES[] = {
    {ATTRID_SCENES_COUNT,TYPE_BYTE,VISIBLE,"scenecount"},
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info GEN_TIME[] = {
    {0,0,VISIBLE,END_FLAG_BUF}
};

attribute_info OTA[] = {
    {0,0,VISIBLE,END_FLAG_BUF}
};


cluaster_info cluasters[] = {
    {ZCL_CLUSTER_ID_GEN_BASIC,BASIC},
    {ZCL_CLUSTER_ID_GEN_POWER_CFG,GEN_POWER_CFG},
    {ZCL_CLUSTER_ID_GEN_ON_OFF,ON_OFF},
    {ZCL_CLUSTER_ID_GEN_ON_OFF_SWITCH_CONFIG,ON_OFF_SWITCH_CONFIG},
    {ZCL_CLUSTER_ID_SE_SIMPLE_METERING,SE_SIMPLE_METERING},
    {ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL,GEN_LEVEL_CONTROL},
    {ZCL_CLUSTER_ID_GEN_ANALOG_INPUT_BASIC,GEN_ANALOG_INPUT_BASIC},
    {ZCL_CLUSTER_ID_CLOSURES_DOOR_LOCK,CLOSURES_DOOR_LOCK},
    {ZCL_CLUSTER_ID_CLOSURES_WINDOW_COVERING,CLOSURES_WINDOW_COVERING},
    {ZCL_CLUSTER_ID_HVAC_THERMOSTAT,HVAC_THERMOSTAT},
    {ZCL_CLUSTER_ID_HVAC_FAN_CONTROL,HVAC_FAN_CONTROL},
    {ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL,LIGHTING_COLOR_CONTROL},
    {ZCL_CLUSTER_ID_MS_ILLUMINANCE_MEASUREMENT,MS_ILLUMINANCE_MEASUREMENT},
    {ZCL_CLUSTER_ID_MS_ILLUMINANCE_LEVEL_SENSING_CONFIG,MS_ILLUMINANCE_LEVEL_SENSING_CONFIG},
    {ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT,MS_TEMPERATURE_MEASUREMENT},
    {ZCL_CLUSTER_ID_MS_RELATIVE_HUMIDITY,MS_RELATIVE_HUMIDITY},
    {ZCL_CLUSTER_ID_SS_IAS_ZONE,SS_IAS_ZONE},
    {ZCL_CLUSTER_ID_SS_IAS_WD,SS_IAS_WD},
    {ZCL_CLUSTER_ID_HA_ELECTRICAL_MEASUREMENT,HA_ELECTRICAL_MEASUREMENT},
    {0xfe01,AL_FE01},
    {0xfe02,AL_FE02},
    {0xfe03,AL_FE03},
    {0xfe04,AL_FE04},
    {0xfe05,AL_FE05},
    
    {ZCL_CLUSTER_ID_GEN_APPLIANCE_CONTROL,APPLIANCE_CONTROL},
    {ZCL_CLUSTER_ID_MS_OCCUPANCY_SENSING,OCCUPANCY_SENSING},
    {ZCL_CLUSTER_ID_GEN_BINARY_INPUT_BASIC,BINARY_INPUT_BASIC},
    {ZCL_CLUSTER_ID_GEN_ANALOG_INPUT_BASIC,ANALOG_INPUT_BASIC},
        

    {ZCL_CLUSTER_ID_GEN_IDENTIFY,IDENTIFY},
    {ZCL_CLUSTER_ID_GEN_GROUPS,GROUPS},
    {ZCL_CLUSTER_ID_GEN_SCENES,SCENES},

    {ZCL_CLUSTER_ID_GEN_TIME,GEN_TIME},
    {ZCL_CLUSTER_ID_OTA,OTA},
    {0,(attribute_info *)NULL},
};


unsigned short HA_PROFILE_DEVICE[] = {
    ZCL_HA_DEVICEID_DIMMABLE_LIGHT,
    ZCL_HA_DEVICEID_ON_OFF_LIGHT,
    ZCL_HA_DEVICEID_COLORED_DIMMABLE_LIGHT,
    ZCL_HA_DEVICEID_MAINS_POWER_OUTLET,
    ZCL_HA_DEVICEID_REMOTE_CONTROL,
    ZCL_HA_DEVICEID_DOOR_LOCK,
    ZCL_HA_DEVICEID_ON_OFF_SWITCH,
    ZCL_HA_DEVICEID_SMART_PLUG,
    ZCL_HA_DEVICEID_WHITE_GOODS,
    ZCL_HA_DEVICEID_SIMPLE_SENSOR,
    ZCL_HA_DEVICEID_LIGHT_SENSOR,
    ZCL_HA_DEVICEID_TEMPERATURE_SENSOR,
    ZCL_HA_DEVICEID_PUMP,
    ZCL_HA_DEVICEID_HEATING_COOLING_UNIT,
    ZCL_HA_DEVICEID_THERMOSTAT,
    ZCL_HA_DEVICEID_IAS_ZONE,
    ZCL_HA_DEVICEID_WINDOW_COVERING_DEVICE,
    ZCL_HA_DEVICEID_SCENE_SELECTOR,
    ZCL_HA_DEVICEID_WINDOW_COVERING_CONTROLLER,
    ZCL_HA_DEVICEID_DIMMER_SWITCH,
    0xffff
};        
                
unsigned short ZLL_PROFILE_DEVICE[] = {
    ZLL_DEVICEID_COLOR_LIGHT,
    ZLL_DEVICEID_EXTENDED_COLOR_LIGHT,
    ZLL_DEVICEID_DIMMABLE_LIGHT,
    ZLL_DEVICEID_COLOR_TEMPERATURE_LIGHT,
    0xffff
};

unsigned short SHUNCOM_PROFILE_ID[] = {
    ZCL_SHUNCOM_DEVICEID_TARNSPORT,
    ZCL_SHUNCOM_DEVICEID_SMART_TARNSPORT,
    0xffff
};





int local_is_lvmi_device(cJSON * p_device)
{
    char * temp = cJSON_Print(p_device);
    if(temp)
        debug(DEBUG_ERROR,"p_device[%s]",temp);
    else
        debug(DEBUG_ERROR,"temp is NULL");
    free(temp);
    temp = NULL;
    
    cJSON * p_dsp = cJSON_GetObjectItem(p_device,"dsp");

    if(!p_dsp)
    {
        debug(DEBUG_ERROR,"p_dsp is NULL");
        return FAILURE;
    }
    if((strlen(p_dsp->valuestring) >= 4) && (memcmp(p_dsp->valuestring,"lumi",4) == 0))
        return SUCCESS;
    else
    {
        debug(DEBUG_ERROR,"p_dsp->valuestring[%s]",p_dsp->valuestring);
        return FAILURE;
    }
}

int local_find_device(unsigned short profile_id,unsigned short device_id)
{
    int i = 0;
    unsigned short * profile = NULL;
    
    if(ZCL_HA_PROFILE_ID == profile_id)
        profile = HA_PROFILE_DEVICE;
    else if(ZLL_PROFILE_ID == profile_id)
        profile = ZLL_PROFILE_DEVICE;
    else if(ZCL_SHUNCOM_PROFILE_ID == profile_id)
        profile = SHUNCOM_PROFILE_ID;
    else
    {
        debug(DEBUG_ERROR,"couldn't find right profile id,profile id error[%d]",profile_id);
        return FAILURE;
    }

    while(profile[i] != 0xffff)
    {
        if(device_id == profile[i])
            return SUCCESS;
        else
            i = i + 1;
    }

    return FAILURE;
}

cluaster_info * local_find_cluster(unsigned short cluster_id,cluaster_info * cluasters)
{
    int i = 0;

    if(cluasters == NULL)
        return (cluaster_info *)NULL;
    while(cluasters[i].attributes != NULL)
    {
        //debug(DEBUG_ERROR,"cluster_id[%d] destcluster_id[%d]",cluster_id,cluasters[i].cluaster_id);
        if(cluster_id == cluasters[i].cluaster_id)
            return (&(cluasters[i]));
        i = i + 1;
    }

    return (cluaster_info *)NULL;
}

attribute_info * local_find_attribute(unsigned short attribute_id,cluaster_info * cluster)
{
    int i = 0;

    if(cluster == NULL)
        return (attribute_info *)NULL;

    while(strcmp(cluster->attributes[i].key,END_FLAG_BUF) != 0)
    {
        //debug(DEBUG_ERROR,"attribute_id[%d] dest_attribute_id[%d]",cluster->attributes[i].attribute_id,attribute_id);
        if(attribute_id == cluster->attributes[i].attribute_id)
            return (&(cluster->attributes[i]));
        i = i + 1;
    }

    return (attribute_info *)NULL;
}


int local_list_new(cJSON **r_result)
{
    device_info_t *device = NULL;
    device_info_t *pdevice = NULL;
    cluaster_info *cluaster = NULL;
    attribute_info *attribute = NULL;
    uint8_t ieeeAddr[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    char ieeeaddr_string[17] = {0};
    coordInfo_t coordInfo = {0};
    int i  = 0;
    int j  = 0;
    int k = 0;
    int finded = 0;

    cJSON * p_devices = cJSON_CreateArray();
    if(!p_devices)
    {
        debug(DEBUG_ERROR,"couldn't find device");
        *r_result =  NULL;
        return CMD_MALLOC_ERROR;
    }

    cJSON * result = cJSON_CreateObject();
    if(!result)
    {
        cJSON_Delete(p_devices);
        p_devices = NULL;
        debug(DEBUG_ERROR,"couldn't find device");
        *r_result =  NULL;
        return CMD_MALLOC_ERROR;
    }

    coord_getInfo(&coordInfo);
    
    avl_for_each_element_safe(&(device_table_head.hdr), device, avl, pdevice) 
    {
        if((memcmp(device->extAddr,coordInfo.ieeeAddr,8) != 0) && (memcmp(ieeeAddr,device->extAddr,8) != 0))
        {
            for(i = 0;i < device->numOfEps;i++)
            {
                cJSON * p_device = cJSON_CreateObject();
                if(!p_device)
                {
                    debug(DEBUG_ERROR,"device create error");
                    continue;
                }
#if 1
                if(FAILURE == local_find_device(device->epList[i].profileId,device->epList[i].deviceId))
                {
                    cJSON_Delete(p_device);
                    p_device = NULL;
                    debug(DEBUG_ERROR,"device profile_id[%d] device_id[%d] error",device->epList[i].profileId,device->epList[i].deviceId);
                    continue;
                }
#endif
                snprintf(ieeeaddr_string,17,"%02x%02x%02x%02x%02x%02x%02x%02x",\
                device->extAddr[0],device->extAddr[1],device->extAddr[2],\
                device->extAddr[3],device->extAddr[4],device->extAddr[5],\
                device->extAddr[6],device->extAddr[7]);
        
                cJSON_AddStringToObject(p_device,"id",ieeeaddr_string);
                cJSON_AddBoolToObject(p_device,"ol",device->online);
                cJSON_AddNumberToObject(p_device,"ep",device->epList[i].endpointId);
                cJSON_AddNumberToObject(p_device,"pid",device->epList[i].profileId);
                cJSON_AddNumberToObject(p_device,"did",device->epList[i].deviceId);
                
                for(j = 0;j < device->epList[i].num_op_clusters;j++)
                {
                    cluaster = NULL;
                    cluaster = local_find_cluster(device->epList[i].op_cluster_list[j].cluster_id,cluasters);
                    if(cluaster == NULL)
                    {
                        debug(DEBUG_ERROR,"conn't find cluster_id[%d]",device->epList[i].op_cluster_list[j].cluster_id);
                        continue;
                    }
                    //debug(DEBUG_ERROR,"cluster_id[%d]",device->epList[i].op_cluster_list[j].cluster_id);
                    for(k = 0;k < device->epList[i].op_cluster_list[j].num_attributes;k++)
                    {
                        attribute = NULL;
                        attribute = local_find_attribute(device->epList[i].op_cluster_list[j].attribute_list[k].attr_id,cluaster);
                        if(attribute == NULL)
                        {
                            debug(DEBUG_ERROR,"conn't find attribute_id[%d]",device->epList[i].op_cluster_list[j].attribute_list[k].attr_id);
                            continue ;
                        }
                        //debug(DEBUG_ERROR,"attribute_id[%d]",device->epList[i].op_cluster_list[j].attribute_list[k].attr_id);
                        //debug(DEBUG_ERROR,"attribute->visible[%d]",attribute->visible);
                        if(attribute->visible != VISIBLE)
                        {
                            debug(DEBUG_ERROR,"attribute_id[%d] is disvisible",device->epList[i].op_cluster_list[j].attribute_list[k].attr_id);
                            continue ;
                        }
                        //debug(DEBUG_ERROR,"attribute->key[%s]",attribute->key);
                        if(attribute->data_type == TYPE_STRING)
                            cJSON_AddStringToObject(p_device,attribute->key,device->epList[i].op_cluster_list[j].attribute_list[k].attr_val + 1);
                        else if(attribute->data_type == TYPE_BOOL)
                            cJSON_AddBoolToObject(p_device,attribute->key,device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0]);
                        else if(attribute->data_type == TYPE_BYTE)
                            cJSON_AddNumberToObject(p_device,attribute->key,device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0]);
                        else if(attribute->data_type == TYPE_SHORT_STRING)
                        {
                            short temp = (short)((((unsigned short)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x00ff) << 8)\
                                                    + (unsigned short)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0]);
                            unsigned char buf_temp [10] = {0};
                            sprintf(buf_temp,"%d.%u%u",temp/100,(temp%100)/10,temp%10);
                            cJSON_AddStringToObject(p_device,attribute->key,buf_temp);
                        }
                        else if(attribute->data_type == TYPE_SHORT)
                        {
                            unsigned short temp = (((unsigned short)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x00ff) << 8)\
                                                    + (unsigned short)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0];
                            if((attribute->attribute_id == ATTRID_HVAC_THERMOSTAT_OCCUPIED_HEATING_SETPOINT) \
                                && (cluaster->cluaster_id == ZCL_CLUSTER_ID_HVAC_THERMOSTAT))
                            {
                                temp = temp/100;
                            }
                            cJSON_AddNumberToObject(p_device,attribute->key,temp);
                        }
                        else if(attribute->data_type == TYPE_THREE_STRING)
                        {
                            unsigned int temp = (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[2] & 0x000000ff) << 16)\
                                                    + (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x000000ff) << 8)\
                                                    + (unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0];
                            unsigned char buf_temp[30] = {0};
                            sprintf(buf_temp,"%d",temp);
                            cJSON_AddStringToObject(p_device,attribute->key,buf_temp);
                        }
                        else if(attribute->data_type == TYPE_THREE)
                        {
                            unsigned int temp = (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[2] & 0x000000ff) << 16)\
                                                    + (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x000000ff) << 8)\
                                                    + (unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0];
                            cJSON_AddNumberToObject(p_device,attribute->key,temp);
                        }
                        else if(attribute->data_type == TYPE_INT_STRING)
                        {
                            unsigned int temp = (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[3] & 0x000000ff) << 24)\
                                                    + (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[2] & 0x000000ff) << 16)\
                                                    + (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x000000ff) << 8)\
                                                    + (unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0];
                            unsigned char buf_temp[30] = {0};
                            sprintf(buf_temp,"%d",temp);
                            cJSON_AddStringToObject(p_device,attribute->key,buf_temp);
                        }
                        else if(attribute->data_type == TYPE_INT)
                        {
                            unsigned int temp = (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[3] & 0x000000ff) << 24)\
                                                    + (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[2] & 0x000000ff) << 16)\
                                                    + (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x000000ff) << 8)\
                                                    + (unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0];
                            cJSON_AddNumberToObject(p_device,attribute->key,temp);
                        }
                        else if(attribute->data_type == TYPE_LL_STRING)
                        {
                            unsigned long long temp = (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0] & 0x00000000000000ff) << 0)\
                                                    + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x00000000000000ff) << 8)\
                                                    + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[2] & 0x00000000000000ff) << 16)\
                                                    + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[3] & 0x00000000000000ff) << 24)\
                                                    + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[4] & 0x00000000000000ff) << 32)\
                                                    + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[5] & 0x00000000000000ff) << 40)\
                                                    + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[6] & 0x00000000000000ff) << 48)\
                                                    + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[7] & 0x00000000000000ff) << 56);
                            unsigned char buf_temp[40] = {0};
                            sprintf(buf_temp,"%lld",temp);
                            cJSON_AddStringToObject(p_device,attribute->key,buf_temp);
                        }
                        else if(attribute->data_type == TYPE_LL)
                        {
                            unsigned long long temp = (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0] & 0x00000000000000ff) << 0)\
                                                    + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x00000000000000ff) << 8)\
                                                    + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[2] & 0x00000000000000ff) << 16)\
                                                    + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[3] & 0x00000000000000ff) << 24)\
                                                    + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[4] & 0x00000000000000ff) << 32)\
                                                    + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[5] & 0x00000000000000ff) << 40)\
                                                    + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[6] & 0x00000000000000ff) << 48)\
                                                    + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[7] & 0x00000000000000ff) << 56);
                            cJSON_AddNumberToObject(p_device,attribute->key,temp);
                        }
                        else
                        {
                            debug(DEBUG_ERROR,"attribute date type error[%d]",attribute->data_type);
                            continue;
                        }
                    }
                }
                if((FAILURE == local_find_device(device->epList[i].profileId,device->epList[i].deviceId)) \
                    && (FAILURE == local_is_lvmi_device(p_device)))
                {
                    cJSON_Delete(p_device);
                    p_device = NULL;
                    debug(DEBUG_ERROR,"device profile_id[%d] device_id[%d] error",device->epList[i].profileId,device->epList[i].deviceId);
                    continue;
                }
                cJSON_AddItemToArray(p_devices,p_device);
            }
            finded = 1;
        }
        //debug(DEBUG_DEBUG,"********* next device *********");
    }

    if(finded != 1)
    {
        cJSON_Delete(p_devices);
        p_devices = NULL;
        cJSON_Delete(result);
        result = NULL;
        debug(DEBUG_ERROR,"couldn't find device");
        *r_result =  NULL;
        return CMD_FAILURE;
    }

    cJSON_AddItemToObject(result,"devices",p_devices);
    *r_result =  result;
    return CMD_SUCCESS;
}


char ieeeAddr_new[MAX_DEVICES][17];
int find_new = 0;
    
void write_device_in_white_list(void)
{
    int i  = 0;
    int j = 0;
    int find_flag = 0;
    uint16_t numOfDevices = 0;
    uint16_t num_ieeeAddr = 0;
    uint8_t ieeeAddr[200*8] = {0};
    char ieeeaddr_string[17] = {0};
    deviceInfo_t * devices = NULL;

    numOfDevices = device_getDevicesList_fill(&devices);
    num_ieeeAddr = device_getWhitelist(ieeeAddr);

    for(i = 0;i < numOfDevices;i++)
    {
        for(j = 0;j < num_ieeeAddr;j++)
        {
            if(memcmp(devices[i].deviceBasic.ieeeAddr,ieeeAddr + j*8,8) == 0)
            {
                find_flag = 1;
                break;
            }
        }
        
        snprintf(ieeeaddr_string,17,"%02x%02x%02x%02x%02x%02x%02x%02x",\
                devices[i].deviceBasic.ieeeAddr[0],devices[i].deviceBasic.ieeeAddr[1],\
                devices[i].deviceBasic.ieeeAddr[2],devices[i].deviceBasic.ieeeAddr[3],\
                devices[i].deviceBasic.ieeeAddr[4],devices[i].deviceBasic.ieeeAddr[5],\
                devices[i].deviceBasic.ieeeAddr[6],devices[i].deviceBasic.ieeeAddr[7]);
        debug(DEBUG_ERROR,"device[%s]",ieeeaddr_string);
            
        if(find_flag == 0)
        {
            if(ZSUCCESS != device_writeWhitelist(devices[i].deviceBasic.ieeeAddr))
            {
                debug(DEBUG_ERROR,"write device[%s] in whitelist failure",ieeeaddr_string);
            }
            else
            {
                debug(DEBUG_ERROR,"write device[%s] in whitelist success",ieeeaddr_string);
                //get new devices
                debug(DEBUG_DEBUG, "%s", "-----------------------------------------------bind ");
                sprintf(ieeeAddr_new[find_new], "%s", ieeeaddr_string);
                find_new++;
            }
        }
        else
        {
            debug(DEBUG_ERROR,"device[%s] is in white list",ieeeaddr_string);
            find_flag = 0;
        }
    }
}

int cloud_is_coordinfo_ok(void)
{
    coordInfo_t coordInfo;
    
    coord_getInfo(&coordInfo);
    
    char wrong_ieee[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    if(memcmp(coordInfo.ieeeAddr,wrong_ieee,8) == 0)
    {
        debug(DEBUG_ERROR,"coordination ieee is wrong");
        return FAILURE;
    }
    else
    {
        //SCprintf("coordination ieee is right");
        debug(1, "coordination ieee is right %s", coordInfo.ieeeAddr);
        return SUCCESS;
    }
}



///////////////////////////////////////////////////////////////////

int local_list_ieeeAddr(cJSON **r_result, char *ieeeAddr_str)
{
    device_info_t *device = NULL;
    device_info_t *pdevice = NULL;
    cluaster_info *cluaster = NULL;
    attribute_info *attribute = NULL;
    uint8_t ieeeAddr[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    char ieeeaddr_string[17] = {0};
    coordInfo_t coordInfo = {0};
    int i  = 0;
    int j  = 0;
    int k = 0;
    int finded = 0;

    cJSON * p_devices = cJSON_CreateArray();
    if(!p_devices)
    {
        debug(DEBUG_ERROR,"couldn't find device");
        *r_result =  NULL;
        return CMD_MALLOC_ERROR;
    }

    cJSON * result = cJSON_CreateObject();
    if(!result)
    {
        cJSON_Delete(p_devices);
        p_devices = NULL;
        debug(DEBUG_ERROR,"couldn't find device");
        *r_result =  NULL;
        return CMD_MALLOC_ERROR;
    }

    coord_getInfo(&coordInfo);
    
    avl_for_each_element_safe(&(device_table_head.hdr), device, avl, pdevice) 
    {
        if((memcmp(device->extAddr,coordInfo.ieeeAddr,8) != 0) && (memcmp(ieeeAddr,device->extAddr,8) != 0))
        {
            for(i = 0;i < device->numOfEps;i++)
            {
                cJSON * p_device = cJSON_CreateObject();
                if(!p_device)
                {
                    debug(DEBUG_ERROR,"device create error");
                    continue;
                }
#if 1
                if(FAILURE == local_find_device(device->epList[i].profileId,device->epList[i].deviceId))
                {
                    cJSON_Delete(p_device);
                    p_device = NULL;
                    debug(DEBUG_ERROR,"device profile_id[%d] device_id[%d] error",device->epList[i].profileId,device->epList[i].deviceId);
                    continue;
                }
#endif
                snprintf(ieeeaddr_string,17,"%02x%02x%02x%02x%02x%02x%02x%02x",\
                device->extAddr[0],device->extAddr[1],device->extAddr[2],\
                device->extAddr[3],device->extAddr[4],device->extAddr[5],\
                device->extAddr[6],device->extAddr[7]);
                //debug(DEBUG_DEBUG,"------------------------------");
                //debug(DEBUG_DEBUG,"device %s", ieeeaddr_string);
        
                if(0 == memcmp(ieeeaddr_string, ieeeAddr_str, 16))
                {
                    cJSON_AddStringToObject(p_device,"id",ieeeaddr_string);
                    cJSON_AddBoolToObject(p_device,"ol",device->online);
                    cJSON_AddNumberToObject(p_device,"ep",device->epList[i].endpointId);
                    cJSON_AddNumberToObject(p_device,"pid",device->epList[i].profileId);
                    cJSON_AddNumberToObject(p_device,"did",device->epList[i].deviceId);
                    
                    for(j = 0;j < device->epList[i].num_op_clusters;j++)
                    {
                        cluaster = NULL;
                        cluaster = local_find_cluster(device->epList[i].op_cluster_list[j].cluster_id,cluasters);
                        if(cluaster == NULL)
                        {
                            debug(DEBUG_ERROR,"conn't find cluster_id[%d]",device->epList[i].op_cluster_list[j].cluster_id);
                            continue;
                        }
                        //debug(DEBUG_ERROR,"cluster_id[%d]",device->epList[i].op_cluster_list[j].cluster_id);
                        for(k = 0;k < device->epList[i].op_cluster_list[j].num_attributes;k++)
                        {
                            attribute = NULL;
                            attribute = local_find_attribute(device->epList[i].op_cluster_list[j].attribute_list[k].attr_id,cluaster);
                            if(attribute == NULL)
                            {
                                debug(DEBUG_ERROR,"conn't find attribute_id[%d]",device->epList[i].op_cluster_list[j].attribute_list[k].attr_id);
                                continue ;
                            }
                            //debug(DEBUG_ERROR,"attribute_id[%d]",device->epList[i].op_cluster_list[j].attribute_list[k].attr_id);
                            //debug(DEBUG_ERROR,"attribute->visible[%d]",attribute->visible);
                            if(attribute->visible != VISIBLE)
                            {
                                debug(DEBUG_ERROR,"attribute_id[%d] is disvisible",device->epList[i].op_cluster_list[j].attribute_list[k].attr_id);
                                continue ;
                            }
                            //debug(DEBUG_ERROR,"attribute->key[%s]",attribute->key);
                            if(attribute->data_type == TYPE_STRING)
                                cJSON_AddStringToObject(p_device,attribute->key,device->epList[i].op_cluster_list[j].attribute_list[k].attr_val + 1);
                            else if(attribute->data_type == TYPE_BOOL)
                                cJSON_AddBoolToObject(p_device,attribute->key,device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0]);
                            else if(attribute->data_type == TYPE_BYTE)
                                cJSON_AddNumberToObject(p_device,attribute->key,device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0]);
                            else if(attribute->data_type == TYPE_SHORT_STRING)
                            {
                                short temp = (short)((((unsigned short)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x00ff) << 8)\
                                                        + (unsigned short)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0]);
                                unsigned char buf_temp [10] = {0};
                                sprintf(buf_temp,"%d.%u%u",temp/100,(temp%100)/10,temp%10);
                                cJSON_AddStringToObject(p_device,attribute->key,buf_temp);
                            }
                            else if(attribute->data_type == TYPE_SHORT)
                            {
                                unsigned short temp = (((unsigned short)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x00ff) << 8)\
                                                        + (unsigned short)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0];
                                if((attribute->attribute_id == ATTRID_HVAC_THERMOSTAT_OCCUPIED_HEATING_SETPOINT) \
                                    && (cluaster->cluaster_id == ZCL_CLUSTER_ID_HVAC_THERMOSTAT))
                                {
                                    temp = temp/100;
                                }
                                cJSON_AddNumberToObject(p_device,attribute->key,temp);
                            }
                            else if(attribute->data_type == TYPE_THREE_STRING)
                            {
                                unsigned int temp = (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[2] & 0x000000ff) << 16)\
                                                        + (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x000000ff) << 8)\
                                                        + (unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0];
                                unsigned char buf_temp[30] = {0};
                                sprintf(buf_temp,"%d",temp);
                                cJSON_AddStringToObject(p_device,attribute->key,buf_temp);
                            }
                            else if(attribute->data_type == TYPE_THREE)
                            {
                                unsigned int temp = (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[2] & 0x000000ff) << 16)\
                                                        + (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x000000ff) << 8)\
                                                        + (unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0];
                                cJSON_AddNumberToObject(p_device,attribute->key,temp);
                            }
                            else if(attribute->data_type == TYPE_INT_STRING)
                            {
                                unsigned int temp = (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[3] & 0x000000ff) << 24)\
                                                        + (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[2] & 0x000000ff) << 16)\
                                                        + (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x000000ff) << 8)\
                                                        + (unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0];
                                unsigned char buf_temp[30] = {0};
                                sprintf(buf_temp,"%d",temp);
                                cJSON_AddStringToObject(p_device,attribute->key,buf_temp);
                            }
                            else if(attribute->data_type == TYPE_INT)
                            {
                                unsigned int temp = (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[3] & 0x000000ff) << 24)\
                                                        + (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[2] & 0x000000ff) << 16)\
                                                        + (((unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x000000ff) << 8)\
                                                        + (unsigned int)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0];
                                cJSON_AddNumberToObject(p_device,attribute->key,temp);
                            }
                            else if(attribute->data_type == TYPE_LL_STRING)
                            {
                                unsigned long long temp = (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0] & 0x00000000000000ff) << 0)\
                                                        + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x00000000000000ff) << 8)\
                                                        + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[2] & 0x00000000000000ff) << 16)\
                                                        + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[3] & 0x00000000000000ff) << 24)\
                                                        + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[4] & 0x00000000000000ff) << 32)\
                                                        + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[5] & 0x00000000000000ff) << 40)\
                                                        + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[6] & 0x00000000000000ff) << 48)\
                                                        + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[7] & 0x00000000000000ff) << 56);
                                unsigned char buf_temp[40] = {0};
                                sprintf(buf_temp,"%lld",temp);
                                cJSON_AddStringToObject(p_device,attribute->key,buf_temp);
                            }
                            else if(attribute->data_type == TYPE_LL)
                            {
                                unsigned long long temp = (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[0] & 0x00000000000000ff) << 0)\
                                                        + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[1] & 0x00000000000000ff) << 8)\
                                                        + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[2] & 0x00000000000000ff) << 16)\
                                                        + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[3] & 0x00000000000000ff) << 24)\
                                                        + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[4] & 0x00000000000000ff) << 32)\
                                                        + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[5] & 0x00000000000000ff) << 40)\
                                                        + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[6] & 0x00000000000000ff) << 48)\
                                                        + (((unsigned long long)device->epList[i].op_cluster_list[j].attribute_list[k].attr_val[7] & 0x00000000000000ff) << 56);
                                cJSON_AddNumberToObject(p_device,attribute->key,temp);
                            }
                            else
                            {
                                debug(DEBUG_ERROR,"attribute date type error[%d]",attribute->data_type);
                                continue;
                            }
                        }
                    }
                    if((FAILURE == local_find_device(device->epList[i].profileId,device->epList[i].deviceId)) \
                        && (FAILURE == local_is_lvmi_device(p_device)))
                    {
                        cJSON_Delete(p_device);
                        p_device = NULL;
                        debug(DEBUG_ERROR,"device profile_id[%d] device_id[%d] error",device->epList[i].profileId,device->epList[i].deviceId);
                        continue;
                    }
                    cJSON_AddItemToArray(p_devices,p_device);
                }
            }
            finded = 1;
        }
        //debug(DEBUG_DEBUG,"********* next device *********");
    }

    if(finded != 1)
    {
        cJSON_Delete(p_devices);
        p_devices = NULL;
        cJSON_Delete(result);
        result = NULL;
        debug(DEBUG_ERROR,"couldn't find device");
        *r_result =  NULL;
        return CMD_FAILURE;
    }

    cJSON_AddItemToObject(result,"devices",p_devices);
    *r_result =  result;
    return CMD_SUCCESS;
}


//2.8.3 sub devices bind report req
void subdevice_bind_report()
{
    printf("--------------------------------------------------------------\n");
    debug(DEBUG_DEBUG, "%d", find_new);
    
    int i;
    char *bind_finish;
    cJSON * r_result = NULL;
    cJSON *devices;
    cJSON *sub_device;
    cJSON *root_str;
    cJSON *devices_new;
    
    root_str = cJSON_CreateObject();
    cJSON_AddItemToObject(root_str, "devices", devices_new = cJSON_CreateArray());
    
    for(i = 0; i < find_new; i++)
    {
        //1. Get the new sub_device ieeeAddr
        local_list_ieeeAddr(&r_result,  ieeeAddr_new[i]);
        bind_finish = cJSON_Print(r_result);
        if(bind_finish != NULL)
        {
            debug(DEBUG_DEBUG, "device info[%s]", bind_finish);
            //2. Get type
            if(cJSON_GetObjectItem(r_result, "devices"))
            {
                devices = cJSON_GetObjectItem(r_result, "devices");
                sub_device = cJSON_GetArrayItem(devices, 0);
                debug(DEBUG_DEBUG, "%s\n", cJSON_Print(sub_device));
                cJSON_AddItemToArray(devices_new, sub_device);
                sub_device = NULL;
            }
            else
            {
                debug(DEBUG_DEBUG, "device info null");
                cJSON_Delete(r_result);
                r_result = NULL;
            }
        }
        else
        {
            debug(DEBUG_DEBUG, "device info null");
            cJSON_Delete(r_result);
            r_result = NULL;
        }
    }
    
    debug(DEBUG_DEBUG, "%s\n", cJSON_Print(root_str));
    bind_sumdevices = find_new;
    debug(DEBUG_DEBUG, "%d", bind_sumdevices);
    sub_device_bind_report_req(root_str);
    
    //cJSON_Delete(r_result);
    find_new = 0;
    r_result = NULL;
    bind_finish = NULL;
    
}


void *bind_thread(void *p)
{
    debug(DEBUG_DEBUG, "%s", "-------------------------------------------begin bind");
    int step = 0;
    bool is_bind = true;
    
    while(FAILURE == cloud_is_coordinfo_ok())
    {
        sleep(2);
    }
    
    debug(DEBUG_DEBUG, "%s", "-------------------------------cloud_is_coordinfo_ok ");
    
    while(is_bind)
    {
        sleep(1);
        
        switch(step)
        {
            case 1://disable whitelist
            {
                device_enableWhitelist(0);
                debug(DEBUG_DEBUG,"disable whitelist");
                is_onbind = true;
                break;
            }
            case 2://open permit join ,240s
            {
                uint8_t ieeeaddr[8] = {0};
                memset(ieeeaddr,0xff,8);
                device_setPermitJoin(ieeeaddr, 60);//240
                debug(DEBUG_DEBUG,"open permit join ,240s");
                break;
            }
            case 62://enable whitelist
            {
                device_enableWhitelist(1);
                step = 0;
                debug(DEBUG_DEBUG,"enable whitelist");
                is_bind = false;
                is_onbind = false;
                /**Note: 
                **Get the gateway scan results, return the string
                **/
                subdevice_bind_report();
                break;
            }
            default:
            {
                cJSON * r_result = NULL;
                char * send_data = NULL;
                local_list_new(&r_result);
                send_data = cJSON_Print(r_result);
                if(send_data != NULL)
                {
                    debug(DEBUG_DEBUG,"device info[%s]",send_data);
                    cJSON_Delete(r_result);
                    r_result = NULL;
                    free(send_data);
                    send_data = NULL;
                }
                else
                {
                    cJSON_Delete(r_result);
                    r_result = NULL;
                }
                write_device_in_white_list();
                break;
            }
        }
        step++;
    }
    debug(DEBUG_DEBUG, "%s", "-----------------------------------------------end ");
    
    pthread_exit(0);
}


//2.8 bind sub devices
int bind_subdevices()
{
    int ret;
    pthread_t bind_thread_t;
    
    if(pthread_create(&bind_thread_t, NULL, bind_thread, (void *)NULL) != 0)
    {
        debug(DEBUG_ERROR," pthread of cloud thread creates error:%s ",strerror(errno)); 
        ret = 0;
    }
    else
    {
        debug(DEBUG_DEBUG," pthread of cloud thread creates successfully ");
        ret = -1;
    }
    
    if(ret >= 0)
        return 0;
    else 
        return -1;
}


//2.13 unbind sub devices
int unbind_subdevices()
{
    Zstatus_t ret;
    ret = device_cleanWhitelist();
    /**
    ** initalize gateway
    **/
    if(ZSUCCESS == ret)
        ret = 0;
    else
        ret = -1;
    
    if(ret >= 0)
        return 0;
    else 
        return -1;
}



//3. Get devices status
void get_device_status()
{
    //1. Get devices status
    cJSON * r_result = NULL;
    char * send_data = NULL;
    local_list_new(&r_result);
    send_data = cJSON_Print(r_result);
    if(send_data != NULL)
    {
        printf("------------------------------------------------------\n");
        debug(DEBUG_DEBUG,"device info[%s]",send_data);
        cJSON_Delete(r_result);
        r_result = NULL;
        free(send_data);
        send_data = NULL;
    }
    else
    {
        cJSON_Delete(r_result);
        r_result = NULL;
    }
    
}

//4. Set devices status
/*****Notice :
* The current writable device is only the curtain motor and door locks, 
*  LED lights have not yet access 
******/
uint8_t set_device_status(cJSON *root_reg)
{
    debug(DEBUG_DEBUG, "%s", cJSON_Print(root_reg));
    if(NULL != root_reg)
    {
        cJSON *type;
        cJSON *ep;
        cJSON *id;
        
        int did;
        cJSON *on;
        cJSON *bri;
        cJSON *hue;
        cJSON *sat;
        cJSON *ctp;
        
        if(cJSON_GetObjectItem(root_reg, "type"))
        {
            type = cJSON_GetObjectItem(root_reg, "type");
            debug(1, "%s", cJSON_Print(type));
            if(cJSON_GetObjectItem(root_reg, "dev_id"))
            {
                int endpoint;
                int ret = -1;
                deviceState_t  deviceState;
                
                id = cJSON_GetObjectItem(root_reg, "dev_id");
                //Verify that the ID is on
                char ieeeAddr_str[17];
                char ieeeaddr_string[17] = {0};
                deviceInfo_t *device_info = NULL;
                //get set_device_statu result
                setdevice_status = 0;
                
                sprintf(ieeeAddr_str, "%s", id->valuestring);
                get_sub_devices_status(&device_info, ieeeAddr_str);
                
                if(NULL != device_info)
                {
                    snprintf(ieeeaddr_string, 17,"%02x%02x%02x%02x%02x%02x%02x%02x",\
                            device_info[0].deviceBasic.ieeeAddr[0],\
                            device_info[0].deviceBasic.ieeeAddr[1],\
                            device_info[0].deviceBasic.ieeeAddr[2],\
                            device_info[0].deviceBasic.ieeeAddr[3],\
                            device_info[0].deviceBasic.ieeeAddr[4],\
                            device_info[0].deviceBasic.ieeeAddr[5],\
                            device_info[0].deviceBasic.ieeeAddr[6],\
                            device_info[0].deviceBasic.ieeeAddr[7]);
                    debug(1, "%s", ieeeaddr_string);
                }
                else
                {
                    debug(DEBUG_DEBUG, "--------dev_id not exists--------");
                    return -1;
                }
                
                if(cJSON_GetObjectItem(root_reg, "ep"))
                {
                    ep = cJSON_GetObjectItem(root_reg, "ep");
                }
                if(ep->valueint)
                {
                    endpoint = ep->valueint;
                    debug(DEBUG_DEBUG, "endpoint = %d", endpoint);
                }
                
                if(device_info[0].deviceBasic.deviceId)
                {
                    did = device_info[0].deviceBasic.deviceId;
                    debug(1, "deviceId = %d", did);
                }
                
                /////////////////////////////////////////////////////
                if(0 == strncmp(type->valuestring, "10010-01-51001", 14))  //motor
                {
                    debug(DEBUG_DEBUG, "--------motor--------");
                    char status[10];
                    
                    if(cJSON_GetObjectItem(root_reg, "sw"))
                    {
                        on = cJSON_GetObjectItem(root_reg, "sw");
                        sprintf(status, "%s", on->valuestring);
                        debug(DEBUG_DEBUG, "%s", status);
                        if(8 != endpoint)
                        {
                            endpoint = 8;
                        }
                        if(0 == strncmp(status, "upopen", 6))
                        {
                            ret = device_setDeviceAttr(device_info[0].deviceBasic.ieeeAddr, endpoint, "upopen", NULL);
                            debug(DEBUG_DEBUG, "%d    %s", ret, "--------upopen--------");
                        }
                        else if(0 == strncmp(status, "downclose", 9))
                        {
                            ret = device_setDeviceAttr(device_info[0].deviceBasic.ieeeAddr, endpoint, "downclose", NULL);
                            debug(DEBUG_DEBUG, "%d    %s", ret, "--------downclose--------");
                        }
                        else if(0 == strncmp(status, "stop", 4))
                        {
                            ret = device_setDeviceAttr(device_info[0].deviceBasic.ieeeAddr, endpoint, "stop", NULL);
                            debug(DEBUG_DEBUG, "%d    %s", ret, "--------stop--------");
                        }
                        else
                        {
                            ret = -1;
                            debug(DEBUG_DEBUG, "    %s", "--------motor error--------");
                        }
                    }
                }
                if(0 == strncmp(type->valuestring, "10010-01-10001", 14))  //door lock
                {
                    debug(DEBUG_DEBUG, "--------door lock--------");
                    int status;
                    
                    if(cJSON_GetObjectItem(root_reg, "switch"))
                    {
                        on = cJSON_GetObjectItem(root_reg, "switch");
                        status = on->valueint;
                        debug(DEBUG_DEBUG, "%d", status);
                        if(1 != endpoint)
                        {
                            endpoint = 1;
                        }
                        if(status >= 0 && status <=2)
                        {
                            deviceState.onoffState.status = status;
                            ret = device_setDeviceAttr(device_info[0].deviceBasic.ieeeAddr, endpoint, "lock", &deviceState.onoffState.status);
                        }
                    }
                }
                if(0 == strncmp(type->valuestring, "10010-01-09001", 14))  //Intelligent socket
                {
                    debug(DEBUG_DEBUG, "--------Intelligent socket--------");
                    int status;
                    
                    if(cJSON_GetObjectItem(root_reg, "switch"))
                    {
                        on = cJSON_GetObjectItem(root_reg, "switch");
                        status = on->valueint;
                        debug(DEBUG_DEBUG, "%d", status);
                        if(1 != endpoint)
                        {
                            if(device_info[0].deviceBasic.deviceId == 512)
                            {
                                endpoint = 11;
                            }
                            else
                            {
                                endpoint = 1;
                            }
                        }
                        if(status >= 0 && status <=2)
                        {
                            deviceState.lightState.on = status;
                            ret = device_setDeviceAttr(device_info[0].deviceBasic.ieeeAddr, endpoint, "on", &deviceState.lightState.on);
                        }
                    }
                }
                if(0 == strncmp(type->valuestring, "10010-01-22001", 14))  //switch
                {
                    debug(DEBUG_DEBUG, "--------switch--------");
                    int status;
                    
                    if(cJSON_GetObjectItem(root_reg, "switch"))
                    {
                        on = cJSON_GetObjectItem(root_reg, "switch");
                        status = on->valueint;
                        debug(DEBUG_DEBUG, "%d", status);
                        
                        if(status >= 0 && status <=2)
                        {
                            deviceState.lightState.on = status;
                            ret = device_setDeviceAttr(device_info[0].deviceBasic.ieeeAddr, endpoint, "on", &deviceState.lightState.on);
                        }
                    }
                }
                if(0 == strncmp(type->valuestring, "10010-01-25001", 14))  //LED controller 
                {
                    debug(DEBUG_DEBUG, "--------LED controller --------");
                    int status;
                    int i_bri;
                    int i_hue;
                    int i_sat;
                    int i_ctp;
                    
                    if(did)
                    {
                        if(did == 512)
                        {
                            endpoint = 11;
                            debug(1, "ZLL %d", did);
                        }
                        else
                        {
                            endpoint = 1;
                        }
                    }
                    else
                    {
                        endpoint = 1;
                    }
                    
                    if(cJSON_GetObjectItem(root_reg, "switch"))
                    {
                        on = cJSON_GetObjectItem(root_reg, "switch");
                        status = on->valueint;
                        debug(DEBUG_DEBUG, "%d", status);
                        if(status >= 0 && status <=2)
                        {
                            deviceState.lightState.on = status;
                            ret = device_setDeviceAttr(device_info[0].deviceBasic.ieeeAddr, endpoint, "on", &deviceState.lightState.on);
                        }
                        if(!status)
                        {
                            return 0;
                        }
                    }
                    if(cJSON_GetObjectItem(root_reg, "bri"))
                    {
                        bri = cJSON_GetObjectItem(root_reg, "bri");
                        i_bri = bri->valueint;
                        debug(DEBUG_DEBUG, "%d", i_bri);
                        if(i_bri >= 1 && i_bri <=254)
                        {
                            deviceState.lightState.bri = i_bri;
                            ret = device_setDeviceAttr(device_info[0].deviceBasic.ieeeAddr, endpoint, "bri", &deviceState.lightState.bri);
                        }
                    }
                    if(cJSON_GetObjectItem(root_reg, "hue"))
                    {
                        hue = cJSON_GetObjectItem(root_reg, "hue");
                        i_hue = hue->valueint;
                        debug(DEBUG_DEBUG, "%d", i_hue);
                        if(i_hue >= 1 && i_hue <=254)
                        {
                            deviceState.lightState.hue = i_hue;
                            ret = device_setDeviceAttr(device_info[0].deviceBasic.ieeeAddr, endpoint, "hue", &deviceState.lightState.hue);
                        }
                    }
                    if(cJSON_GetObjectItem(root_reg, "ctp"))
                    {
                        ctp = cJSON_GetObjectItem(root_reg, "ctp");
                        i_ctp = ctp->valueint;
                        debug(DEBUG_DEBUG, "%d", i_ctp);
                        if(i_ctp >= 1 && i_ctp <=254)
                        {
                            deviceState.lightState.colortemp = i_ctp;
                            ret = device_setDeviceAttr(device_info[0].deviceBasic.ieeeAddr, endpoint, "colortemp", &deviceState.lightState.colortemp);
                        }
                    }
                    if(cJSON_GetObjectItem(root_reg, "sat"))
                    {
                        sat = cJSON_GetObjectItem(root_reg, "sat");
                        i_sat = sat->valueint;
                        debug(DEBUG_DEBUG, "%d", i_sat);
                        if(i_sat >= 1 && i_sat <=254)
                        {
                            deviceState.lightState.sat = i_sat;
                            ret = device_setDeviceAttr(device_info[0].deviceBasic.ieeeAddr, endpoint, "sat", &deviceState.lightState.sat);
                        }
                    }
                }
                if(0 == strncmp(type->valuestring, "10010-01-52801", 14))  //Hue 
                {
                    debug(DEBUG_DEBUG, "--------Hue controller --------");
                    int status;
                    int i_bri;
                    int i_hue;
                    int i_sat;
                    int i_ctp;
                
                    if(1 != endpoint)
                    {
                        endpoint = 11;
                    }
                    else
                    {
                        endpoint = 11;
                    }
                    if(cJSON_GetObjectItem(root_reg, "switch"))
                    {
                        on = cJSON_GetObjectItem(root_reg, "switch");
                        status = on->valueint;
                        debug(DEBUG_DEBUG, "%d", status);
                        if(status >= 0 && status <=2)
                        {
                            deviceState.lightState.on = status;
                            ret = device_setDeviceAttr(device_info[0].deviceBasic.ieeeAddr, endpoint, "on", &deviceState.lightState.on);
                        }
                    }
                    if(cJSON_GetObjectItem(root_reg, "bri"))
                    {
                        bri = cJSON_GetObjectItem(root_reg, "bri");
                        i_bri = bri->valueint;
                        debug(DEBUG_DEBUG, "%d", i_bri);
                        if(i_bri >= 1 && i_bri <=254)
                        {
                            deviceState.lightState.bri = i_bri;
                            ret = device_setDeviceAttr(device_info[0].deviceBasic.ieeeAddr, endpoint, "bri", &deviceState.lightState.bri);
                        }
                    }
                    if(cJSON_GetObjectItem(root_reg, "hue"))
                    {
                        hue = cJSON_GetObjectItem(root_reg, "hue");
                        i_hue = hue->valueint;
                        debug(DEBUG_DEBUG, "%d", i_hue);
                        if(i_hue >= 1 && i_hue <=254)
                        {
                            deviceState.lightState.hue = i_hue;
                            ret = device_setDeviceAttr(device_info[0].deviceBasic.ieeeAddr, endpoint, "hue", &deviceState.lightState.hue);
                        }
                    }
                    if(cJSON_GetObjectItem(root_reg, "ctp"))
                    {
                        ctp = cJSON_GetObjectItem(root_reg, "ctp");
                        i_ctp = ctp->valueint;
                        debug(DEBUG_DEBUG, "%d", i_ctp);
                        if(i_ctp >= 1 && i_ctp <=254)
                        {
                            deviceState.lightState.colortemp = i_ctp;
                            ret = device_setDeviceAttr(device_info[0].deviceBasic.ieeeAddr, endpoint, "colortemp", &deviceState.lightState.colortemp);
                        }
                    }
                    if(cJSON_GetObjectItem(root_reg, "sat"))
                    {
                        sat = cJSON_GetObjectItem(root_reg, "sat");
                        i_sat = sat->valueint;
                        debug(DEBUG_DEBUG, "%d", i_sat);
                        if(i_sat >= 1 && i_sat <=254)
                        {
                            deviceState.lightState.sat = i_sat;
                            ret = device_setDeviceAttr(device_info[0].deviceBasic.ieeeAddr, endpoint, "sat", &deviceState.lightState.sat);
                        }
                    }
                }
                if(0 == strncmp(type->valuestring, "10010-01-26001", 14))  //Dimming panel
                {
                    debug(DEBUG_DEBUG, "--------Dimming panel--------");
                    int status;
                    int i_bri;
                    
                    if(1 != endpoint)
                    {
                        endpoint = 1;
                    }
                    if(cJSON_GetObjectItem(root_reg, "switch"))
                    {
                        on = cJSON_GetObjectItem(root_reg, "switch");
                        status = on->valueint;
                        debug(DEBUG_DEBUG, "%d", status);
                        if(status >= 0 && status <=2)
                        {
                            deviceState.lightState.on = status;
                            ret = device_setDeviceAttr(device_info[0].deviceBasic.ieeeAddr, endpoint, "on", &deviceState.lightState.on);
                        }
                    }
                    if(cJSON_GetObjectItem(root_reg, "bri"))
                    {
                        bri = cJSON_GetObjectItem(root_reg, "bri");
                        i_bri = bri->valueint;
                        debug(DEBUG_DEBUG, "%d", i_bri);
                        if(i_bri >= 1 && i_bri <=254)
                        {
                            deviceState.lightState.bri = i_bri;
                            ret = device_setDeviceAttr(device_info[0].deviceBasic.ieeeAddr, endpoint, "bri", &deviceState.lightState.bri);
                        }
                    }
                }
                
                //get result
                if(ZSUCCESS == ret)
                {
                    setdevice_status = 0;
                }
                else
                {
                    setdevice_status = -1;
                }
            }
        }
        is_sub_device_action_exec_report_req = true;
    }
    else
    {
        
    }
    
    return 0;
}

//00124b000e4fbe1e
void set_be()
{
    uint16_t set_status;
    uint8_t ieeeaddr1[EXT_ADDR_LEN] = {0x00,0x12,0x4b,0x00,0x0e,0x4f,0xbe,0x1e};
    uint8_t ieeeaddr2[EXT_ADDR_LEN] = {0x00,0x12,0x4b,0x00,0x0c,0xb8,0x71,0xb6};
    uint8_t ieeeaddr3[EXT_ADDR_LEN] = {0x00,0x12,0x4b,0x00,0x08,0xec,0xd2,0x23};
    uint8_t ieeeaddr4[EXT_ADDR_LEN] = {0x00,0x12,0x4b,0x00,0x0e,0x4f,0xd9,0x21};
    uint8_t endpoint = 1;
    deviceState_t  deviceState;
    int i;
    
    for(i = 0; i < 3; i++)
    {
        deviceState.lightState.on = 1;
        set_status = device_setDeviceAttr(ieeeaddr1, endpoint, "on", &deviceState.lightState.on);
        set_status = device_setDeviceAttr(ieeeaddr2, endpoint, "on", &deviceState.lightState.on);
        set_status = device_setDeviceAttr(ieeeaddr3, 1, "on", &deviceState.lightState.on);
        set_status = device_setDeviceAttr(ieeeaddr3, 2, "on", &deviceState.lightState.on);
        set_status = device_setDeviceAttr(ieeeaddr3, 3, "on", &deviceState.lightState.on);
        set_status = device_setDeviceAttr(ieeeaddr4, endpoint, "on", &deviceState.lightState.on);
        sleep(3);
        
        deviceState.lightState.on = 0;
        set_status = device_setDeviceAttr(ieeeaddr1, endpoint, "on", &deviceState.lightState.on);
        set_status = device_setDeviceAttr(ieeeaddr2, endpoint, "on", &deviceState.lightState.on);
        set_status = device_setDeviceAttr(ieeeaddr3, 1, "on", &deviceState.lightState.on);
        set_status = device_setDeviceAttr(ieeeaddr3, 2, "on", &deviceState.lightState.on);
        set_status = device_setDeviceAttr(ieeeaddr3, 3, "on", &deviceState.lightState.on);
        set_status = device_setDeviceAttr(ieeeaddr4, endpoint, "on", &deviceState.lightState.on);
        sleep(3);
    }
    
    deviceState.lightState.on = 1;
    set_status = device_setDeviceAttr(ieeeaddr4, endpoint, "on", &deviceState.lightState.on);
    sleep(3);
    
    for(i = 1; i < 255; i += 10)
    {
        deviceState.lightState.bri = i;
        set_status = device_setDeviceAttr(ieeeaddr4, endpoint, "bri", &deviceState.lightState.bri);
        usleep(500000);
    }
    
    for(i = 1; i < 255; i += 10)
    {
        deviceState.lightState.hue = i;
        set_status = device_setDeviceAttr(ieeeaddr4, endpoint, "hue", &deviceState.lightState.bri);
        usleep(500000);
    }
    
    for(i = 1; i < 255; i += 10)
    {
        deviceState.lightState.sat = i;
        set_status = device_setDeviceAttr(ieeeaddr4, endpoint, "sat", &deviceState.lightState.bri);
        usleep(500000);
    }
    
    for(i = 153; i < 499; i += 10)
    {
        deviceState.lightState.colortemp = i;
        set_status = device_setDeviceAttr(ieeeaddr4, endpoint, "colortemp", &deviceState.lightState.bri);
        usleep(500000);
    }
    
    deviceState.lightState.on = 0;
    set_status = device_setDeviceAttr(ieeeaddr4, endpoint, "on", &deviceState.lightState.on);
    sleep(3);
}



