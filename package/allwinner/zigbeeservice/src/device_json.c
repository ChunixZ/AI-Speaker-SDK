/*************************************************************************
    > File Name: device_json.c
    > Author: zyc
    > Mail: zyc@imio.ai 
    > Created Time: 2017/06/30
 ************************************************************************/
#include "device_json.h"
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cJSON.h"
#include "zigbee_device.h"
#include "user_types.h"
#include "ubus.h"
#include "httpclient.h"
#include "cap.h"
#include "wireless.h"
#include "driver_led_key.h"



int bind_sumdevices = -1;
bool is_people = false;
bool is_bind_speech = false;
//extern bool is_online_rep;
extern bool is_heartbeat_normal;
extern bool is_subdevice_bind_rep;
extern bool is_device_bind_report_req;
extern bool is_device_unbind_report_req;
extern bool is_sub_device_action_rep;
extern bool is_subdevice_download_rep;
extern bool is_subdevice_data_upload_req;
extern bool is_device_data_download;
extern bool is_ota_req;
extern bool is_avs_activation;
extern bool is_ai_service_req;
extern bool is_onbind;
extern bool is_device_ready;
extern bool is_reply;


char timestamp[12];
char *trxid;
char *trxid_down;
char *trxid_dev;
char *trx_device;
char *trxid_ota;
char *trxid_avs;
bool is_ota_download = false;
bool is_subdevice_data_upload = false;
bool is_subdevice_data_download = false;
deviceInfo_t *devices_Info;
OTA_STATUS is_ota_status = OTA_NORMAL;

const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";



/********************************
*          device status functions           **
*********************************/
char *get_timestamp()
{
    unsigned long int seconds;
    //static char timestamp[12];

    memset(timestamp, 0, sizeof(char)*12);
    do
    {
        seconds = (int)time(NULL);
        //debug(DEBUG_DEBUG, "%ld", seconds);
    }
    while(seconds <= 1513758000);
    
    sprintf(timestamp, "%10ld", seconds);
    //debug(DEBUG_DEBUG, "%s", timestamp);
    
    return timestamp;
}

char* get_random_string(int length)
{
    int flag, i;
    char* string;
    
    srand((unsigned) time(NULL ));
    if ((string = (char*) malloc(length)) == NULL )
    {
        printf("Malloc failed!flag:14\n");
        return 0;
    }

    for (i = 0; i < length - 1; i++)
    {
        flag = rand() % 3;
        switch (flag)
        {
            case 0:
                string[i] = 'A' + rand() % 26;
                break;
            case 1:
                string[i] = 'a' + rand() % 26;
                break;
            case 2:
                string[i] = '0' + rand() % 10;
                break;
            default:
                string[i] = 'x';
                break;
        }
    }
    string[length - 1] = '\0';
    
    return string;
} 

//6 random + timestamp + mac  --->  md5
char *get_trxid(int num)
{
    static char string_str[128];
    char *str;
    char *time;
    
    str = get_random_string(num);
    time = get_timestamp();
    sprintf(string_str, "%s%s", str, time);
    printf("%s\n", string_str);
    
    return string_str;
}


char* StrSHA256(const char* str, long long length, char* sha256)
{
    char *pp, *ppend;
    long int l, i, W[64], T1, T2, A, B, C, D, E, F, G, H, H0, H1, H2, H3, H4, H5, H6, H7;
    H0 = 0x6a09e667, H1 = 0xbb67ae85, H2 = 0x3c6ef372, H3 = 0xa54ff53a;
    H4 = 0x510e527f, H5 = 0x9b05688c, H6 = 0x1f83d9ab, H7 = 0x5be0cd19;
    
    long int K[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
    };
    
    l = length + ((length % 64 >= 56) ? (128 - length % 64) : (64 - length % 64));
    
    if (!(pp = (char*)malloc((unsigned long)l))) 
        return 0;
    
    for (i = 0; i < length; pp[i + 3 - 2 * (i % 4)] = str[i], i++);
    for (pp[i + 3 - 2 * (i % 4)] = 128, i++; i < l; pp[i + 3 - 2 * (i % 4)] = 0, i++);
    
    *((long*)(pp + l - 4)) = length << 3;
    *((long*)(pp + l - 8)) = length >> 29;
    
    for (ppend = pp + l; pp < ppend; pp += 64)
    {
        for (i = 0; i < 16; W[i] = ((long*)pp)[i], i++);
        for (i = 16; i < 64; W[i] = (SHA256_O1(W[i - 2]) + W[i - 7] + SHA256_O0(W[i - 15]) + W[i - 16]), i++);
        A = H0, B = H1, C = H2, D = H3, E = H4, F = H5, G = H6, H = H7;
        for (i = 0; i < 64; i++){
            T1 = H + SHA256_E1(E) + SHA256_Ch(E, F, G) + K[i] + W[i];
            T2 = SHA256_E0(A) + SHA256_Maj(A, B, C);
            H = G, G = F, F = E, E = D + T1, D = C, C = B, B = A, A = T1 + T2;
        }
        H0 += A, H1 += B, H2 += C, H3 += D, H4 += E, H5 += F, H6 += G, H7 += H;
    }
    
    free(pp - l);
    sprintf(sha256, "%08X%08X%08X%08X%08X%08X%08X%08X", H0, H1, H2, H3, H4, H5, H6, H7);
    
    return sha256;
}

char * base64_encode( const unsigned char * bindata, char * base64, int binlength )
{
    int i, j;
    unsigned char current;

    for ( i = 0, j = 0 ; i < binlength ; i += 3 )
    {
        current = (bindata[i] >> 2) ;
        current &= (unsigned char)0x3F;
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)(bindata[i] << 4 ) ) & ( (unsigned char)0x30 ) ;
        if ( i + 1 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+1] >> 4) ) & ( (unsigned char) 0x0F );
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)(bindata[i+1] << 2) ) & ( (unsigned char)0x3C ) ;
        if ( i + 2 >= binlength )
        {
            base64[j++] = base64char[(int)current];
            base64[j++] = '=';
            break;
        }
        current |= ( (unsigned char)(bindata[i+2] >> 6) ) & ( (unsigned char) 0x03 );
        base64[j++] = base64char[(int)current];

        current = ( (unsigned char)bindata[i+2] ) & ( (unsigned char)0x3F ) ;
        base64[j++] = base64char[(int)current];
    }
    base64[j] = '\0';
    return base64;
}

int base64_decode( const char *base64, unsigned char *bindata)
{
    int i, j;
    unsigned char k;
    unsigned char temp[4];
    for ( i = 0, j = 0; base64[i] != '\0' ; i += 4 )
    {
        memset( temp, 0xFF, sizeof(temp) );
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i] )
                temp[0]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+1] )
                temp[1]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+2] )
                temp[2]= k;
        }
        for ( k = 0 ; k < 64 ; k ++ )
        {
            if ( base64char[k] == base64[i+3] )
                temp[3]= k;
        }

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[0] << 2))&0xFC)) |
                ((unsigned char)((unsigned char)(temp[1]>>4)&0x03));
        if ( base64[i+2] == '=' )
            break;

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[1] << 4))&0xF0)) |
                ((unsigned char)((unsigned char)(temp[2]>>2)&0x0F));
        if ( base64[i+3] == '=' )
            break;

        bindata[j++] = ((unsigned char)(((unsigned char)(temp[2] << 6))&0xF0)) |
                ((unsigned char)(temp[3]&0x3F));
    }
    return j;
}

//avs 43~128 characters
char* genRandomString(int length)
{
    int flag, i, flags;
    char* string;
    srand((unsigned) time(NULL ));
    
    if ((string = (char*) malloc(length)) == NULL )
    {
        printf("Malloc failed!flag:14\n");
        return 0;
    }

    for (i = 0; i < length - 1; i++)
    {
        flag = rand() % 7;
        //printf("--flag = %d--\n", flag);
        switch (flag)
        {  
            case 0:
                string[i] = 'A' + rand() % 26;
                break;
            case 1:
                string[i] = 'a' + rand() % 26;
                break;
            case 2:
                string[i] = '0' + rand() % 10;
                break;
            case 3:
                string[i] = 'A' + rand() % 26;
                break;
            case 4:
                string[i] = 'a' + rand() % 26;
                break;
            case 5:
                string[i] = '0' + rand() % 10;
                break;
            case 6:
            {
                flags = rand() % 4;
                //printf("--flags = %d--\n", flags);
                switch(flags)
                {
                    case 0:
                        string[i] = '-';
                        break;
                    case 1:
                        string[i] = '.';
                        break;
                    case 2:
                        string[i] = '~';
                        break;
                    case 3:
                        string[i] = '_';
                        break;
                }
                break;
            }
            default:
                string[i] = 'x';
                break;
        }
    }
    string[length - 1] = '\0';
    
    return string;
}

int avs_code_verifier(char **avs_code, char **challenge_code)
{
    int i, j, flag = 1;
    static char *str;
    char sha256[1024];
    static char base64[1024];
    
    srand((int)time(0));
    
    while(flag)
    {
        j = 1 + (int)(128.0 * rand() / (RAND_MAX + 1.0));
        if(j <= 128 && (j >= 43))
        {
            flag = 0;
            //printf(" %d \n", j);
            str = genRandomString(j);
            //printf("%s\n", str);
            *avs_code = str;
            //code_challenge = BASE64URL-ENCODE(SHA256(ASCII(code_verifier)))
            StrSHA256(str, sizeof(str)-1, sha256);  // sizeof()计算的结果包含了末尾的'\0'应减1
            //printf("%s\n", sha256);
            base64_encode(sha256, base64, strlen(sha256));
            //printf("%s\n", base64);
            *challenge_code = base64;
        }
    }
    
    return 0;
}


//get_subdevices_status use 
int get_sub_devices_status(deviceInfo_t **device, char *ieeeaddr_str)
{
    int i = 0;
    int numOfDevices = 0;
    char ieeeaddr_string[17] = {0};
    static deviceInfo_t *devices = NULL;
    //char *ieeeaddr_str = "00124b0007fdd64f";
    
    numOfDevices = device_getDevicesList_fill(&devices);
    for(i = 0; i < numOfDevices; i++)
    {
        snprintf(ieeeaddr_string, 17,"%02x%02x%02x%02x%02x%02x%02x%02x",\
            devices[i].deviceBasic.ieeeAddr[0],\
            devices[i].deviceBasic.ieeeAddr[1],\
            devices[i].deviceBasic.ieeeAddr[2],\
            devices[i].deviceBasic.ieeeAddr[3],\
            devices[i].deviceBasic.ieeeAddr[4],\
            devices[i].deviceBasic.ieeeAddr[5],\
            devices[i].deviceBasic.ieeeAddr[6],\
            devices[i].deviceBasic.ieeeAddr[7]);
        //debug(1, "%s", ieeeaddr_string);
        
        if(strncmp(ieeeaddr_string, ieeeaddr_str, 16) == 0)
        {
            *device = &devices[i];
            //printf("-----------------------------------------\n");  
            debug(1, "ID = %s\n",ieeeaddr_string);
            /*
            printf("ol = %d\n",devices[i].deviceBasic.online);
            printf("ep = %d\n",devices[i].deviceBasic.endpointId);
            printf("pid = %d\n",devices[i].deviceBasic.profileId);
            printf("did = %d\n",devices[i].deviceBasic.deviceId);
            printf("facid = %d\n",devices[i].deviceBasic.ManufacturerName);
            printf("zid = %d\n",devices[i].deviceState.zoneState.zoneId);
            printf("type = %d\n",devices[i].deviceState.zoneState.zoneType);
            printf("sta = %d\n",devices[i].deviceState.zoneState.status);
            printf("pt = %d\n",devices[i].deviceState.percentage.percentage);
            printf("-----------------------------------------\n");  
            */
            break;
        }
    }
    
    //*device = NULL;
    
    return 0;
}


//get_subdevices status
int get_subdevices_status(cJSON **r_result_s, cJSON *r_result, char *cmd, char *cmd_str)
{
    //1. Parse cJSON data
    cJSON *root_req;
    cJSON *subdevice_data;
    cJSON *sub_bind;
    cJSON *sub_device;
    
    char *device_time_str = get_timestamp();
    char *ip_addr = "10.0.1.186";
    //char *trxid = "012345678987654321";
    char *dev_type = "10010-01";
    
    root_req = cJSON_CreateObject();
    cJSON_AddItemToObject(root_req, cmd, subdevice_data = cJSON_CreateObject());
    cJSON_AddItemToObject(subdevice_data, cmd_str, sub_bind = cJSON_CreateArray());
    cJSON_AddStringToObject(subdevice_data, "device_time", device_time_str);
    cJSON_AddStringToObject(subdevice_data, "ip_addr", ip_addr);
    if(is_subdevice_data_upload)
    {
        cJSON_AddStringToObject(subdevice_data, "trxid", trxid_dev);
    }
    else if(is_subdevice_data_download)
    {
        cJSON_AddStringToObject(subdevice_data, "trxid", trxid_down);
    }
    else
    {
        cJSON_AddStringToObject(subdevice_data, "trxid", trxid);
    }
    
    cJSON_AddStringToObject(subdevice_data, "type", dev_type);
    
    //2. Get local_list_new(), get sub devices status,
    int i, j, sum;
    char type_sub[15];
    cJSON *devices_list;
    cJSON *devices;
    cJSON *loop_devices;
    cJSON *loop_id;
    
    cJSON *id;
    cJSON *ol;
    cJSON *ep;
    cJSON *did;
    
    cJSON *type;
    cJSON *sta;
    cJSON *pt;
    
    cJSON *on;
    cJSON *bri;
    cJSON *hue;
    cJSON *sat;
    cJSON *ctp;
    
    char *send_data = NULL;
    send_data = cJSON_Print(r_result);
    
    if(send_data != NULL)
    {
        //debug(DEBUG_DEBUG,"device info[%s]", send_data);
        //report here  Get the device type
        devices_list = cJSON_GetObjectItem(r_result, "devices");
        sum = cJSON_GetArraySize(devices_list);
        deviceInfo_t *device_info = NULL;
        char ieeeaddr_string[17] = {0};
        char ieeeAddr_str[17];
        
        //switch number
        int switch_type = 0;
        int ol_int;
        char *id_string;
        //loop
        char id_Addr[17] = {0};
        
        for(i = 0; i < sum; i++)
        {
            cJSON *status_info;
            status_info = cJSON_CreateArray();

            devices = cJSON_GetArrayItem(devices_list, i);
            debug(1, "--------> i = %d", i);
            //debug(1, "%s", cJSON_Print(devices));
            id = cJSON_GetObjectItem(devices, "id");
            ep = cJSON_GetObjectItem(devices, "ep");
            did = cJSON_GetObjectItem(devices, "did");
            sprintf(ieeeAddr_str, "%s", id->valuestring);
            debug(1, "%s\n", ieeeAddr_str);
            
            if(0 == (strncmp((id->valuestring), id_Addr, 16)))
            {
                debug(1, "the same id_Addr ", id->valuestring);
                continue;
            }
            
            if(is_subdevice_data_upload)
            {
                device_info = devices_Info;
            }
            else
            {
                get_sub_devices_status(&device_info, ieeeAddr_str);
            }
            
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
            
            if(cJSON_GetObjectItem(devices, "ol"))
            {
                ol = cJSON_GetObjectItem(devices, "ol");
                if(0 == strncmp(cJSON_Print(ol), "true", 4))
                {
                    ol = cJSON_Parse("1");
                    //debug(1, "devices---->    %s    %d", cJSON_Print(ol), ol->valueint);
                }
                if(0 == strncmp(cJSON_Print(ol), "false", 5))
                {
                    ol = cJSON_Parse("0");
                    //debug(1, "devices---->    %s    %d", cJSON_Print(ol), ol->valueint);
                }
            }
            else
            {
                char ol_str[4];
                sprintf(ol_str, "%d", device_info[0].deviceBasic.online);
                ol = cJSON_Parse(ol_str);
                //debug(1, "%s", cJSON_Print(ol));
            }
            
            if(((int)did->valueint) == 1026)
            {
                cJSON_AddItemToArray(sub_bind, sub_device = cJSON_CreateObject());
                
                //'type' is not sure here, 
                if(cJSON_GetObjectItem(devices, "type"))
                {
                    type = cJSON_GetObjectItem(devices, "type");
                }
                else
                {
                    char type_str[4];
                    sprintf(type_str, "%d", device_info[0].deviceState.zoneState.zoneType);
                    type = cJSON_Parse(type_str);
                }
                
                if(cJSON_GetObjectItem(devices, "sta"))
                {///////////////////////////////////////
                    sta = cJSON_GetObjectItem(devices, "sta");
                    debug(1, "%s", cJSON_Print(sta));
                }
                else
                {
                    char sta_str[4];
                    sprintf(sta_str, "%d", device_info[0].deviceState.zoneState.status);
                    sta = cJSON_Parse(sta_str);
                    debug(1, "%s", cJSON_Print(sta));
                }
                
                //add exception
                if((sta->valueint) != 0 || (sta->valueint) != 1)
                {
                    cJSON_AddNumberToObject(sub_device, "err", sta->valueint);
                    
                    if((sta->valueint) % 2 == 0)
                    {
                        sta = cJSON_Parse("0");
                    }
                    else
                    {
                        sta = cJSON_Parse("1");
                    }
                }
                else
                {
                    cJSON_AddNumberToObject(sub_device, "err", 0);
                }
                
                
                switch((int)type->valueint)
                {
                    case 13: //infrared sensor
                    {
                        sprintf(type_sub, "%s", "10010-01-13001");
                        cJSON_AddStringToObject(sub_device, "type", type_sub);
                        break;
                    }
                    case 21: //door sensor
                    {
                        sprintf(type_sub, "%s", "10010-01-21001");
                        cJSON_AddStringToObject(sub_device, "type", type_sub);
                        break;
                    }
                    case 40: //smoke transducer
                    {
                        sprintf(type_sub, "%s", "10010-01-40001");
                        cJSON_AddStringToObject(sub_device, "type", type_sub);
                        break;
                    }
                    case 42: //water transducer
                    {
                        sprintf(type_sub, "%s", "10010-01-18001");
                        cJSON_AddStringToObject(sub_device, "type", type_sub);
                        break;
                    }
                    default:
                    {
                        sprintf(type_sub, "10010-01-%d001", type->valueint);
                        cJSON_AddStringToObject(sub_device, "type", type_sub);
                        break;
                    }
                }
                
                cJSON_AddNumberToObject(sub_device, "ep", ep->valueint);
                cJSON_AddNumberToObject(sub_device, "ol", ol->valueint);
                cJSON_AddNumberToObject(sub_device, "switch", sta->valueint);
                cJSON_AddStringToObject(sub_device, "dev_id", id->valuestring);
                cJSON_AddNumberToObject(sub_device, "type_id", type->valueint);
                //debug(DEBUG_DEBUG, "%s", cJSON_Print(sub_device));
                sub_device = NULL;
            }
            
            if(((int)did->valueint) == 514) //motor
            {
                cJSON_AddItemToArray(sub_bind, sub_device = cJSON_CreateObject());
                
                if(cJSON_GetObjectItem(devices, "pt"))
                {
                    pt = cJSON_GetObjectItem(devices, "pt");
                    //debug(1, "%s", cJSON_Print(pt));
                }
                else
                {
                    char pt_str[6];
                    sprintf(pt_str, "%d", device_info[0].deviceState.percentage.percentage);
                    pt = cJSON_Parse(pt_str);
                    //debug(1, "%s", cJSON_Print(pt));
                }
                
                sprintf(type_sub, "%s", "10010-01-51001");
                cJSON_AddStringToObject(sub_device, "type", type_sub);
                cJSON_AddNumberToObject(sub_device, "ep", ep->valueint);
                cJSON_AddNumberToObject(sub_device, "pt", pt->valueint);//
                cJSON_AddNumberToObject(sub_device, "ol", ol->valueint);
                cJSON_AddStringToObject(sub_device, "dev_id", id->valuestring);
                //debug(DEBUG_DEBUG, "%s", cJSON_Print(sub_device));
                sub_device = NULL;
            }
            
            if(((int)did->valueint) == 10) //door lock
            {
                cJSON_AddItemToArray(sub_bind, sub_device = cJSON_CreateObject());
                
                if(cJSON_GetObjectItem(devices, "on"))
                {
                    on = cJSON_GetObjectItem(devices, "on");
                    if(0 == strncmp(cJSON_Print(on), "true", 4))
                    {
                        on = cJSON_Parse("1");
                        //debug(1, "devices---->    %s    %d", cJSON_Print(on), on->valueint);
                    }
                    if(0 == strncmp(cJSON_Print(on), "false", 5))
                    {
                        on = cJSON_Parse("0");
                        //debug(1, "devices---->    %s    %d", cJSON_Print(on), on->valueint);
                    }
                }
                else
                {
                    char on_str[4];
                    sprintf(on_str, "%d", device_info[0].deviceState.onoffState.status);
                    on = cJSON_Parse(on_str);
                    //debug(1, "%s", cJSON_Print(on));
                }
                
                sprintf(type_sub, "%s", "10010-01-10001");
                cJSON_AddStringToObject(sub_device, "type", type_sub);
                cJSON_AddNumberToObject(sub_device, "ep", ep->valueint);
                cJSON_AddNumberToObject(sub_device, "switch", on->valueint);
                cJSON_AddNumberToObject(sub_device, "ol", ol->valueint);
                cJSON_AddStringToObject(sub_device, "dev_id", id->valuestring);
                //debug(DEBUG_DEBUG, "%s", cJSON_Print(sub_device));
                sub_device = NULL;
            }
            
            if(((int)did->valueint) == 9) //Intelligent socket
            {
                cJSON_AddItemToArray(sub_bind, sub_device = cJSON_CreateObject());
                if(cJSON_GetObjectItem(devices, "on"))
                {
                    on = cJSON_GetObjectItem(devices, "on");
                    if(0 == strncmp(cJSON_Print(on), "true", 4))
                    {
                        on = cJSON_Parse("1");
                        //debug(1, "devices---->    %s    %d", cJSON_Print(on), on->valueint);
                    }
                    if(0 == strncmp(cJSON_Print(on), "false", 5))
                    {
                        on = cJSON_Parse("0");
                        //debug(1, "devices---->    %s    %d", cJSON_Print(on), on->valueint);
                    }
                }
                else
                {
                    char on_str[4];
                    sprintf(on_str, "%d", device_info[0].deviceState.lightState.on);
                    on = cJSON_Parse(on_str);
                    //debug(1, "%s    %d", cJSON_Print(on), on->valueint);
                }
                
                sprintf(type_sub, "%s", "10010-01-09001");
                cJSON_AddStringToObject(sub_device, "type", type_sub);
                cJSON_AddStringToObject(sub_device, "dev_id", id->valuestring);
                cJSON_AddStringToObject(sub_device, "plug_type", "11");
                cJSON_AddNumberToObject(sub_device, "ep", ep->valueint);
                cJSON_AddNumberToObject(sub_device, "ol", ol->valueint);
                cJSON_AddNumberToObject(sub_device, "switch", on->valueint);
                //debug(DEBUG_DEBUG, "%s", cJSON_Print(sub_device));
                sub_device = NULL;
            }
            
            if((((int)did->valueint) == 258) || (((int)did->valueint) == 512)) //LED controller 
            {
                cJSON_AddItemToArray(sub_bind, sub_device = cJSON_CreateObject());
                
                if(cJSON_GetObjectItem(devices, "on"))
                {
                    on = cJSON_GetObjectItem(devices, "on");
                    if(0 == strncmp(cJSON_Print(on), "true", 4))
                    {
                        on = cJSON_Parse("1");
                        //debug(1, "devices---->    %s    %d", cJSON_Print(on), on->valueint);
                    }
                    if(0 == strncmp(cJSON_Print(on), "false", 5))
                    {
                        on = cJSON_Parse("0");
                        //debug(1, "devices---->    %s    %d", cJSON_Print(on), on->valueint);
                    }
                }
                else
                {
                    char on_str[4];
                    sprintf(on_str, "%d", device_info[0].deviceState.lightState.on);
                    on = cJSON_Parse(on_str);
                    //debug(1, "%s    %d", cJSON_Print(on), on->valueint);
                }
                
                if(cJSON_GetObjectItem(devices, "bri"))
                {
                    bri = cJSON_GetObjectItem(devices, "bri");
                    //debug(1, "%s    %d", cJSON_Print(bri), bri->valueint);
                }
                else
                {
                    char bri_str[4];
                    sprintf(bri_str, "%d", device_info[0].deviceState.lightState.bri);
                    bri = cJSON_Parse(bri_str);
                    //debug(1, "%s    %d", cJSON_Print(bri), bri->valueint);
                }
                
                if(cJSON_GetObjectItem(devices, "hue"))
                {
                    hue = cJSON_GetObjectItem(devices, "hue");
                    //debug(1, "%s    %d", cJSON_Print(hue), hue->valueint);
                }
                else
                {
                    char hue_str[4];
                    sprintf(hue_str, "%d", device_info[0].deviceState.lightState.hue);
                    hue = cJSON_Parse(hue_str);
                    //debug(1, "%s    %d", cJSON_Print(hue), hue->valueint);
                }
                
                if(cJSON_GetObjectItem(devices, "sat"))
                {
                    sat = cJSON_GetObjectItem(devices, "sat");
                    //debug(1, "%s    %d", cJSON_Print(sat), sat->valueint);
                }
                else
                {
                    char sat_str[4];
                    sprintf(sat_str, "%d", device_info[0].deviceState.lightState.sat);
                    sat = cJSON_Parse(sat_str);
                    //debug(1, "%s    %d", cJSON_Print(sat), sat->valueint);
                }
                
                if(cJSON_GetObjectItem(devices, "ctp"))
                {
                    ctp = cJSON_GetObjectItem(devices, "ctp");
                    //debug(1, "%s    %d", cJSON_Print(ctp), ctp->valueint);
                }
                else
                {
                    char ctp_str[4];
                    sprintf(ctp_str, "%d", device_info[0].deviceState.lightState.colortemp);
                    ctp = cJSON_Parse(ctp_str);
                    //debug(1, "%s    %d", cJSON_Print(ctp), ctp->valueint);
                }
                
                sprintf(type_sub, "%s", "10010-01-25001");
                cJSON_AddStringToObject(sub_device, "type", type_sub);
                cJSON_AddStringToObject(sub_device, "dev_id", id->valuestring);
                cJSON_AddNumberToObject(sub_device, "ep", ep->valueint);
                cJSON_AddNumberToObject(sub_device, "ol", ol->valueint);
                cJSON_AddNumberToObject(sub_device, "switch", on->valueint);
                cJSON_AddNumberToObject(sub_device, "bri", bri->valueint);
                cJSON_AddNumberToObject(sub_device, "hue", hue->valueint);
                cJSON_AddNumberToObject(sub_device, "sat", sat->valueint);
                cJSON_AddNumberToObject(sub_device, "ctp", ctp->valueint);
                //debug(DEBUG_DEBUG, "%s", cJSON_Print(sub_device));
                sub_device = NULL;
            }
            
            if(((int)did->valueint) == 528) //Hue
            {
                cJSON_AddItemToArray(sub_bind, sub_device = cJSON_CreateObject());
                
                if(cJSON_GetObjectItem(devices, "on"))
                {
                    on = cJSON_GetObjectItem(devices, "on");
                    if(0 == strncmp(cJSON_Print(on), "true", 4))
                    {
                        on = cJSON_Parse("1");
                        //debug(1, "devices---->    %s    %d", cJSON_Print(on), on->valueint);
                    }
                    if(0 == strncmp(cJSON_Print(on), "false", 5))
                    {
                        on = cJSON_Parse("0");
                        //debug(1, "devices---->    %s    %d", cJSON_Print(on), on->valueint);
                    }
                }
                else
                {
                    char on_str[4];
                    sprintf(on_str, "%d", device_info[0].deviceState.lightState.on);
                    on = cJSON_Parse(on_str);
                    //debug(1, "%s    %d", cJSON_Print(on), on->valueint);
                }
                
                if(cJSON_GetObjectItem(devices, "bri"))
                {
                    bri = cJSON_GetObjectItem(devices, "bri");
                    //debug(1, "%s    %d", cJSON_Print(bri), bri->valueint);
                }
                else
                {
                    char bri_str[4];
                    sprintf(bri_str, "%d", device_info[0].deviceState.lightState.bri);
                    bri = cJSON_Parse(bri_str);
                    //debug(1, "%s    %d", cJSON_Print(bri), bri->valueint);
                }
                
                if(cJSON_GetObjectItem(devices, "hue"))
                {
                    hue = cJSON_GetObjectItem(devices, "hue");
                    //debug(1, "%s    %d", cJSON_Print(hue), hue->valueint);
                }
                else
                {
                    char hue_str[4];
                    sprintf(hue_str, "%d", device_info[0].deviceState.lightState.hue);
                    hue = cJSON_Parse(hue_str);
                    //debug(1, "%s    %d", cJSON_Print(hue), hue->valueint);
                }
                
                if(cJSON_GetObjectItem(devices, "sat"))
                {
                    sat = cJSON_GetObjectItem(devices, "sat");
                    //debug(1, "%s    %d", cJSON_Print(sat), sat->valueint);
                }
                else
                {
                    char sat_str[4];
                    sprintf(sat_str, "%d", device_info[0].deviceState.lightState.sat);
                    sat = cJSON_Parse(sat_str);
                    //debug(1, "%s    %d", cJSON_Print(sat), sat->valueint);
                }
                
                if(cJSON_GetObjectItem(devices, "ctp"))
                {
                    ctp = cJSON_GetObjectItem(devices, "ctp");
                    //debug(1, "%s    %d", cJSON_Print(ctp), ctp->valueint);
                }
                else
                {
                    char ctp_str[4];
                    sprintf(ctp_str, "%d", device_info[0].deviceState.lightState.colortemp);
                    ctp = cJSON_Parse(ctp_str);
                    //debug(1, "%s    %d", cJSON_Print(ctp), ctp->valueint);
                }
                
                sprintf(type_sub, "%s", "10010-01-52801");
                cJSON_AddStringToObject(sub_device, "type", type_sub);
                cJSON_AddStringToObject(sub_device, "dev_id", id->valuestring);
                cJSON_AddNumberToObject(sub_device, "ep", ep->valueint);
                cJSON_AddNumberToObject(sub_device, "ol", ol->valueint);
                cJSON_AddNumberToObject(sub_device, "switch", on->valueint);
                cJSON_AddNumberToObject(sub_device, "bri", bri->valueint);
                cJSON_AddNumberToObject(sub_device, "hue", hue->valueint);
                cJSON_AddNumberToObject(sub_device, "sat", sat->valueint);
                cJSON_AddNumberToObject(sub_device, "ctp", ctp->valueint);
                //debug(DEBUG_DEBUG, "%s", cJSON_Print(sub_device));
                sub_device = NULL;
            }
            
            if(((int)did->valueint) == 260) //Dimming panel
            {
                for(j = 0; j < sum; j++)
                {
                    cJSON *sta_info;
                    cJSON *ep_now;
                    sta_info = cJSON_CreateObject();
                    loop_devices = cJSON_GetArrayItem(devices_list, j);
                    loop_id = cJSON_GetObjectItem(loop_devices, "id");
                    ep_now = cJSON_GetObjectItem(loop_devices, "ep");
                    
                    if(0 == (strncmp((loop_id->valuestring), (id->valuestring), 16)))
                    {
                        //debug(1, "loop_id is %s", loop_id->valuestring);
                        switch_type += 1;
                        id_string = id->valuestring;
                        ol_int = ol->valueint;
                        
                        if(cJSON_GetObjectItem(loop_devices, "on"))
                        {
                            on = cJSON_GetObjectItem(loop_devices, "on");
                            if(0 == strncmp(cJSON_Print(on), "true", 4))
                            {
                                on = cJSON_Parse("1");
                                //debug(1, "loop_devices---->    %s    %d", cJSON_Print(on), on->valueint);
                            }
                            if(0 == strncmp(cJSON_Print(on), "false", 5))
                            {
                                on = cJSON_Parse("0");
                                //debug(1, "loop_devices---->    %s    %d", cJSON_Print(on), on->valueint);
                            }
                        }
                        else
                        {
                            char on_str[4];
                            sprintf(on_str, "%d", device_info[0].deviceState.lightState.on);
                            on = cJSON_Parse(on_str);
                            //debug(1, "%s    %d", cJSON_Print(on), on->valueint);
                        }
                        
                        if(cJSON_GetObjectItem(loop_devices, "bri"))
                        {
                            bri = cJSON_GetObjectItem(loop_devices, "bri");
                            //debug(1, "%s    %d", cJSON_Print(bri), bri->valueint);
                        }
                        else
                        {
                            char bri_str[4];
                            sprintf(bri_str, "%d", device_info[0].deviceState.lightState.bri);
                            bri = cJSON_Parse(bri_str);
                            //debug(1, "%s    %d", cJSON_Print(bri), bri->valueint);
                        }
                        
                        cJSON_AddNumberToObject(sta_info, "switch", on->valueint);//maybe is none
                        cJSON_AddNumberToObject(sta_info, "bri", bri->valueint);
                        cJSON_AddNumberToObject(sta_info, "ep", ep_now->valueint);
                        //debug(1, "%s", cJSON_Print(sta_info));
                        cJSON_AddItemToArray(status_info, sta_info);
                        sta_info = NULL;
                    }
                }
                
                //debug(1, "%s", cJSON_Print(status_info));
                cJSON_AddItemToArray(sub_bind, sub_device = cJSON_CreateObject());
                cJSON_AddItemToObject(sub_device, "sta", status_info);
                sprintf(type_sub, "%s", "10010-01-26001");
                cJSON_AddStringToObject(sub_device, "type", type_sub);
                cJSON_AddNumberToObject(sub_device, "switch_type", switch_type);
                cJSON_AddStringToObject(sub_device, "dev_id", id_string);
                cJSON_AddNumberToObject(sub_device, "ol", ol_int);
                
                //debug(DEBUG_DEBUG, "%s", cJSON_Print(sub_device));
                switch_type = 0;
            }
            
            if(((int)did->valueint) == 0) //switch
            {
                //a. loop 
                for(j = 0; j < sum; j++)
                {
                    //debug(1, "j = %d", j);
                    cJSON *sta_info;
                    cJSON *ep_now;
                    sta_info = cJSON_CreateObject();
                    loop_devices = cJSON_GetArrayItem(devices_list, j);
                    loop_id = cJSON_GetObjectItem(loop_devices, "id");
                    ep_now = cJSON_GetObjectItem(loop_devices, "ep");
                    //debug(1, "%s", loop_id->valuestring);
                    
                    if(0 == (strncmp((loop_id->valuestring), (id->valuestring), 16)))
                    {
                        //debug(1, "loop_id is %s", loop_id->valuestring);
                        switch_type += 1;
                        id_string = id->valuestring;
                        ol_int = ol->valueint;
                        
                        if(cJSON_GetObjectItem(loop_devices, "on"))
                        {
                            on = cJSON_GetObjectItem(loop_devices, "on");
                            if(0 == strncmp(cJSON_Print(on), "true", 4))
                            {
                                on = cJSON_Parse("1");
                                //debug(1, "loop_devices---->    %s    %d", cJSON_Print(on), on->valueint);
                            }
                            if(0 == strncmp(cJSON_Print(on), "false", 5))
                            {
                                on = cJSON_Parse("0");
                                //debug(1, "loop_devices---->    %s    %d", cJSON_Print(on), on->valueint);
                            }
                        }
                        else
                        {
                            char on_str[4];
                            sprintf(on_str, "%d", device_info[0].deviceState.lightState.on);
                            on = cJSON_Parse(on_str);
                            //debug(1, "%s    %d", cJSON_Print(on), on->valueint);
                        }
                        
                        cJSON_AddNumberToObject(sta_info, "switch", on->valueint);//maybe is none
                        cJSON_AddNumberToObject(sta_info, "ep", ep_now->valueint);
                        //debug(1, "%s", cJSON_Print(sta_info));
                        cJSON_AddItemToArray(status_info, sta_info);
                        sta_info = NULL;
                    }
                }
                
                //debug(1, "%s", cJSON_Print(status_info));
                cJSON_AddItemToArray(sub_bind, sub_device = cJSON_CreateObject());
                cJSON_AddItemToObject(sub_device, "sta", status_info);
                sprintf(type_sub, "%s", "10010-01-22001");
                cJSON_AddStringToObject(sub_device, "type", type_sub);
                cJSON_AddNumberToObject(sub_device, "switch_type", switch_type);
                cJSON_AddStringToObject(sub_device, "dev_id", id_string);
                cJSON_AddNumberToObject(sub_device, "ol", ol_int);
                
                //debug(DEBUG_DEBUG, "%s", cJSON_Print(sub_device));
                switch_type = 0;
            }
            
            //*. record the ID num and continue it
            sprintf(id_Addr, "%s", id->valuestring);
            debug(1, "id_Addr is %s", id_Addr);
        }
        
        memset(id_Addr, 0, sizeof(id_Addr));
        
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
    
    is_subdevice_data_upload = false;
    is_subdevice_data_download = false;
    *r_result_s =  root_req;
    
    return 0;
}



/**
** MQTT message 
**/
//2.1.1 device online req
void device_online_req(char **send_str, char *channel_no)
{
    char *device_time = get_timestamp();
    static char *out;
    cJSON *root_req;
    cJSON *online_req;
    
    root_req = cJSON_CreateObject();
    cJSON_AddItemToObject(root_req, "online_req", online_req = cJSON_CreateObject());
    cJSON_AddStringToObject(online_req, "channel_no", channel_no);
    cJSON_AddStringToObject(online_req, "device_time", device_time);
    out = cJSON_Print(root_req);
    
    *send_str = out;
    //cJSON_Delete(root_req);
    //cJSON_Delete(online_req);
    //root_req = NULL;
    //online_req = NULL;
}

//2.1.2
void device_online_rep(char *rec_str)
{
    char *error_code_str;
    char *error_info_str;
    char *cloud_time_str;
    
    //char *rec_str = "{\
    \"online_rep\":    {\
            \"error_code\":    \"0\",\
            \"error_info\":    \"NULL\",\
            \"cloud_time\":    \"timestamp\"\
        }\
    }";
    
    cJSON *root_rep = cJSON_Parse(rec_str);
    cJSON *online_rep = cJSON_GetObjectItem(root_rep, "online_rep");
    cJSON *error_code = cJSON_GetObjectItem(online_rep, "error_code");
    cJSON *error_info = cJSON_GetObjectItem(online_rep, "error_info");
    cJSON *cloud_time = cJSON_GetObjectItem(online_rep, "cloud_time");
    error_code_str = error_code->valuestring;
    error_info_str = error_info->valuestring;
    cloud_time_str = cloud_time->valuestring;
    
    printf("[device_json] %s\n", error_code_str);
    printf("[device_json] %s\n", error_info_str);
    printf("[device_json] %s\n", cloud_time_str);
    
}


//2.2.1 device send heartbeat
void device_hb_req(char **send_str)
{
    char *firmware_version = read_version();
    char *device_time = get_timestamp();
    static char *out;
    cJSON *root_req;
    cJSON *device_hb_req;
    
    root_req = cJSON_CreateObject();
    cJSON_AddItemToObject(root_req, "device_hb_req", device_hb_req = cJSON_CreateObject());
    cJSON_AddStringToObject(device_hb_req, "firmware_version", firmware_version);
    cJSON_AddStringToObject(device_hb_req, "device_time", device_time);
    out = cJSON_Print(root_req);
    
    //debug(DEBUG_DEBUG, "[device_json] %s\n", out);
    *send_str = out;
}

//2.2.2
void device_hb_rep(char *rec_str)
{
    char *error_code_str;
    char *error_info_str;
    char *cloud_time_str;
    
    //char *rec_str = "{\
    \"device_hb_rep\":    {\
        \"error_code\":    \"0\",\
        \"error_info\":    \"NULL\",\
        \"cloud_time\":    \"timestamp\"\
    }\
}";
    
    cJSON *root_rep = cJSON_Parse(rec_str);
    cJSON *device_hb_rep = cJSON_GetObjectItem(root_rep, "device_hb_rep");
    cJSON *error_code = cJSON_GetObjectItem(device_hb_rep, "error_code");
    cJSON *error_info = cJSON_GetObjectItem(device_hb_rep, "error_info");
    cJSON *cloud_time = cJSON_GetObjectItem(device_hb_rep, "cloud_time");
    error_code_str = error_code->valuestring;
    error_info_str = error_info->valuestring;
    cloud_time_str = cloud_time->valuestring;
    
    printf("[device_json] %s\n", error_code_str);
    printf("[device_json] %s\n", error_info_str);
    printf("[device_json] %s\n", cloud_time_str);
    
    if(0 == strcmp(error_code_str, "0"))
    {
        debug(1, "---- heartbeat is ok ----");
    }
    else if(0 == strcmp(error_code_str, "-1"))
    {
        is_heartbeat_normal = false;
        debug(1, "---- heartbeat is error : %s----", error_info_str);
    }
    else if(0 == strcmp(error_info_str, "NULL"))
    {
        debug(1, "---- heartbeat is ok ----");
    }
    
}


//2.3.1 Equipment information initiative reported
void device_data_upload_req()
{
    char *device_time = get_timestamp();
    char *ip_addr = "10.0.0.103";///////////////////////Need to get IP
    static char *out;
    cJSON *root_req;
    cJSON *device_data_upload_req;
    cJSON *device_data;
    
    root_req = cJSON_CreateObject();
    cJSON_AddItemToObject(root_req, "device_data_upload_req", device_data_upload_req = cJSON_CreateObject());
    cJSON_AddItemToObject(device_data_upload_req, "device_data", device_data = cJSON_CreateObject());
    cJSON_AddStringToObject(device_data, "ip_addr", ip_addr);
    //.....
    cJSON_AddStringToObject(device_data_upload_req, "device_time", device_time);
    out = cJSON_Print(root_req);
    
    printf("[device_json] %s\n", out);
    
}

//2.3.2
void device_data_upload_rep(char *rec_str)
{
    char *error_code_str;
    char *error_info_str;
    char *error_time_str;
    
    //char *rec_str = "{\
    \"device_data_upload_rep\":    {\
        \"error_code\":    \"返回码,0代表正常\",\
        \"error_info\":    \"错误信息，无错误返回NULL\",\
        \"cloud_time\":    \"云端时间timestamp\"\
    }\
}";
    
    cJSON *root_rep = cJSON_Parse(rec_str);
    cJSON *device_data_upload_rep = cJSON_GetObjectItem(root_rep, "device_data_upload_rep");
    cJSON *error_code = cJSON_GetObjectItem(device_data_upload_rep, "error_code");
    cJSON *error_info = cJSON_GetObjectItem(device_data_upload_rep, "error_info");
    cJSON *cloud_time = cJSON_GetObjectItem(device_data_upload_rep, "cloud_time");
    error_code_str = error_code->valuestring;
    error_info_str = error_info->valuestring;
    error_time_str = cloud_time->valuestring;
    
    printf("[device_json] %s\n", error_code_str);
    printf("[device_json] %s\n", error_info_str);
    printf("[device_json] %s\n", error_time_str);
}


//2.5.1
int device_data_download(char *rec_str)
{
    cJSON *root_reg;
    cJSON *device_data_download_req;
    cJSON *device_data;
    cJSON *cloud_time;
    cJSON *trxid_device;
    
    root_reg = cJSON_Parse(rec_str);
    if(cJSON_GetObjectItem(root_reg, "device_data_download_req"))
    {
        device_data_download_req = cJSON_GetObjectItem(root_reg, "device_data_download_req");
        device_data = cJSON_GetObjectItem(device_data_download_req, "device_data");
        trxid_device = cJSON_GetObjectItem(device_data_download_req, "trxid");
        trx_device = trxid_device->valuestring;
    }
    is_device_data_download = true;
    debug(1, "----> device_data_download_req <----");
    
    return 0;
}

//2.5.2
int device_data_download_rep(char **send_str)
{
    static char *out;
    char *firmware_version = read_version();
    char *device_time = get_timestamp();
    
    cJSON *root_rep;
    cJSON *device_data_download;
    cJSON *device_data;
    
    root_rep = cJSON_CreateObject();
    cJSON_AddItemToObject(root_rep, "device_data_download_rep", device_data_download = cJSON_CreateObject());
    
    if(trx_device != NULL)
    {
        cJSON_AddStringToObject(device_data_download, "trxid", trx_device);
    }

    cJSON_AddItemToObject(device_data_download, "device_data", device_data = cJSON_CreateObject());
    //cJSON_AddStringToObject(device_data, "ol", 1);
    cJSON_AddStringToObject(device_data_download, "device_time", device_time);
    cJSON_AddStringToObject(device_data, "firmware_version", firmware_version);
    out = cJSON_Print(root_rep);
    
    //printf("[device_json] %s\n", out);
    *send_str = out;
    
    return 0;
}


//2.6.1 OTA
int ota_request(char *rec_str)
{
    cJSON *root_reg;
    cJSON *ota_req;
    cJSON *version;
    cJSON *md5;
    cJSON *url;
    cJSON *cloud_time;
    cJSON *ota_trxid;
    char *version_str;
    char *md5_str;
    char *url_str;
    //char *cloud_time_str;
    
    root_reg = cJSON_Parse(rec_str);
    if(cJSON_GetObjectItem(root_reg, "ota_req"))
    {
        ota_req = cJSON_GetObjectItem(root_reg, "ota_req");
        if(cJSON_GetObjectItem(ota_req, "version"))
        {
            version = cJSON_GetObjectItem(ota_req, "version");
            if(cJSON_GetObjectItem(ota_req, "md5"))
            {
                md5 = cJSON_GetObjectItem(ota_req, "md5");
                if(cJSON_GetObjectItem(ota_req, "url"))
                {
                    url = cJSON_GetObjectItem(ota_req, "url");
                    if(cJSON_GetObjectItem(ota_req, "cloud_time"))
                    {
                        if(cJSON_GetObjectItem(ota_req, "trxid"))
                        {
                            ota_trxid = cJSON_GetObjectItem(ota_req, "trxid");
                            trxid_ota = ota_trxid->valuestring;
                            printf("trx_id = %s\n", trxid_ota);
                        }
                        cloud_time = cJSON_GetObjectItem(ota_req, "cloud_time");
                        url_str = url->valuestring;
                        md5_str = md5->valuestring;
                        version_str = version->valuestring;
                        debug(1, "%s", url_str);
                        debug(1, "%s", md5_str);
                        debug(1, "%s", version_str);
                        //1. 校验版本号
                        char *version_read = read_version();
                        debug(1, "now: %s    new:    %s    %ld : ", version_read, version_str, strlen(version_str));
                        if(strlen(version_str) >= 6)
                        {
                            if((md5_str == NULL) || (url_str == NULL) || (strlen(url_str) <= 10))
                            {
                                debug(1, "now: %s    new:    %s    %ld : ", version_read, version_str, strlen(version_str));
                                return 0;
                            }
                            if(strncmp(version_read, version_str, 16))
                            {
                                is_ota_req = true;
                                is_ota_download = false;
                                //httpclient_ota(url_str, md5_str, version_str);
                                /* Note:  开启新线程接收
                                */
                                char url_buff[256];
                                sprintf(url_buff, "wget -P /mnt/UDISK/misc-upgrade %s --no-check-certificate", url_str);
                                debug(1, "%s", url_buff);
                                led_driver(MUL_COLOR_RE);
                                azplay(mode_1, "/home/ota_downloading.mp3");
                                system(url_buff);
                                sleep(1);
                                debug(1, "--------------------------------");
                                debug(1, "Download OK!");
                                debug(1, "--------------------------------");
                                printf("--------Download is Ok --------\n");
                                printf("--------Download is Ok --------\n");
                                printf("--------Download is Ok --------\n");
                                write_version(version_str);
                                led_driver(MUL_COLOR_RE);
                                system("tar -zxvf /mnt/UDISK/misc-upgrade/*.tar.gz -C /tmp");
                                system("rm /mnt/UDISK/misc-upgrade/*.tar.gz");
                                led_driver(ALL_LEDS_RED);
                                azplay(mode_1, "/home/ota_update_ok.mp3");
                                system("aw_upgrade_normal.sh -f");
                                //do cp here , there is an other way to touch it
                                //system("rm -rf /home/*");
                                //system("cp -r /rom/home/* /home/");
                                //system("reboot -f");
                            }
                        }
                    }
                }
            }
        }
    }
    
    return 0;
}

//2.6.2
int ota_request_rep(char **send_str)
{
    static char *out;
    char *error_code = "0";
    char *error_info = "NULL";
    char *device_time = get_timestamp();
    
    cJSON *root_rep;
    cJSON *ota_sub_rep;
    
    root_rep = cJSON_CreateObject();
    cJSON_AddItemToObject(root_rep, "ota_rep", ota_sub_rep = cJSON_CreateObject());
    
    if(trxid_ota != NULL)
    {
        cJSON_AddStringToObject(ota_sub_rep, "trxid", trxid_ota);
    }
    cJSON_AddStringToObject(ota_sub_rep, "error_code", error_code);
    cJSON_AddStringToObject(ota_sub_rep, "error_info", error_info);
    cJSON_AddStringToObject(ota_sub_rep, "device_time", device_time);
    out = cJSON_Print(root_rep);
    
    //printf("[device_json] %s\n", out);
    *send_str = out;
    is_ota_status = OTA_START;
    
    return 0;
}

//2.6.3
int ota_status_report_req(char **send_str)
{
    static char *out;
    char *status = "1";
    char *progress = "100";
    char *device_time = get_timestamp();
    
    cJSON *root_rep;
    cJSON *ota_status_report;
    
    root_rep = cJSON_CreateObject();
    cJSON_AddItemToObject(root_rep, "ota_status_report_req", ota_status_report = cJSON_CreateObject());
    
    if(trxid_ota != NULL)
    {
        cJSON_AddStringToObject(ota_status_report, "trxid", trxid_ota);
    }
    cJSON_AddStringToObject(ota_status_report, "status", status);
    cJSON_AddStringToObject(ota_status_report, "progress", progress);
    cJSON_AddStringToObject(ota_status_report, "device_time", device_time);
    out = cJSON_Print(root_rep);
    
    printf("[device_json] %s\n", out);
    *send_str = out;
    
    return 0;
}

//2.6.4
int ota_status_report_rep(char *rec_str)
{
    cJSON *root_req;
    cJSON *ota_status_report;
    cJSON *error_code;
    cJSON *error_info;
    cJSON *cloud_time;
    
    root_req = cJSON_Parse(rec_str);
    
    if(cJSON_GetObjectItem(root_req, "ota_status_report_rep"))
    {
        ota_status_report = cJSON_GetObjectItem(root_req, "ota_status_report_rep");
        error_code = cJSON_GetObjectItem(ota_status_report, "error_code");
        error_info = cJSON_GetObjectItem(ota_status_report, "error_info");
        cloud_time = cJSON_GetObjectItem(ota_status_report, "cloud_time");
        
        if(0 == strncmp((error_code->valuestring), "0", 1))
        {
            debug(1, "%s", error_code->valuestring);
            system("reboot");
        }
    }
    
    return 0;
}


//2.8.1 Sub_device bind
int sub_device_bind_req(char *rec_str)
{
    int ret;
    char *bind_sub_str;
    char *cloud_time_str;
    
    cJSON *root_req = cJSON_Parse(rec_str);
    cJSON *bind_sub_req;
    cJSON *bind_sub;
    cJSON *cloud_time;
    cJSON *trx_id;
    
    if(cJSON_GetObjectItem(root_req, "subdevice_bind_req"))
    {
        bind_sub_req = cJSON_GetObjectItem(root_req, "subdevice_bind_req");
        if(cJSON_GetObjectItem(bind_sub_req, "sub_bind"))
        {
            bind_sub = cJSON_GetObjectItem(bind_sub_req, "sub_bind");
            if(cJSON_GetObjectItem(bind_sub_req, "cloud_time"))
            {
                cloud_time = cJSON_GetObjectItem(bind_sub_req, "cloud_time");
                if(cJSON_GetObjectItem(bind_sub_req, "trxid"))
                {
                    trx_id = cJSON_GetObjectItem(bind_sub_req, "trxid");
                    trxid = trx_id->valuestring;
                    debug(1, "%s", trxid);
                    //bind_sub_str = bind_sub->valuestring;
                    //cloud_time_str = cloud_time->valuestring;
                    
                    //printf("[device_json] %s\n", bind_sub->valuestring);
                    //printf("[device_json] %s\n", cloud_time->valuestring);
                    
                    if(0 == strncmp("true", bind_sub->valuestring, 4))
                    {
                        //Start binding the sub devices
                        if(!is_onbind)
                        {
                            ret = bind_subdevices();
                            is_subdevice_bind_rep = true;
                        }
                        else
                        {
                            device_ai_speaker("设备搜索中，请稍后。");
                        }
                    }
                }
                
            }
        }
    }
    
    if(ret >= 0)
        return 0;
    else 
        return -1;
}

//2.8.2
void sub_device_bind_rep(char **send_str)
{
    static char *out;
    char *error_code = "0";
    char *error_info = "NULL";
    char *device_time = get_timestamp();
    
    cJSON *root_rep;
    cJSON *bind_sub_rep;
    
    root_rep = cJSON_CreateObject();
    cJSON_AddItemToObject(root_rep, "subdevice_bind_rep", bind_sub_rep = cJSON_CreateObject());
    
    if(trxid != NULL)
    {
        cJSON_AddStringToObject(bind_sub_rep, "trxid", trxid);
    }
    cJSON_AddStringToObject(bind_sub_rep, "error_code", error_code);
    cJSON_AddStringToObject(bind_sub_rep, "error_info", error_info);
    cJSON_AddStringToObject(bind_sub_rep, "device_time", device_time);
    out = cJSON_Print(root_rep);
    
    //printf("[device_json] %s\n", out);
    *send_str = out;
}

//2.8.3
char *report_str;

void sub_device_bind_report_req(cJSON *send_data)
{
    static cJSON *r_result = NULL;
    report_str = NULL;
    //debug(DEBUG_DEBUG, "[device_json] %s\n", report_str);
    
    debug(1, "-------------sub_device_bind_report_req-------------");
    get_subdevices_status(&r_result, send_data, "subdevice_bind_report_req", "sub_bind");
    
    report_str = cJSON_Print(r_result);
    //debug(DEBUG_DEBUG, "[device_json] \n%s\n", report_str);
    is_device_bind_report_req = true;
    
    if(bind_sumdevices >= 1)
    {
        static char speak_str[256];
        memset(speak_str, 0, sizeof(speak_str));
        //sprintf(speak_str, "搜索到 %d 个新设备，并且绑定成功。", bind_sumdevices);
        sprintf(speak_str, "搜索到新设备，并且绑定成功。");
        debug(1, "%s", speak_str);
        is_bind_speech = true;
        device_ai_speaker(speak_str);
        bind_sumdevices = -1;
    }
    else if(0 == bind_sumdevices)
    {
        bind_sumdevices = -1;
        is_bind_speech = true;
        device_ai_speaker("搜索设备完成，没有绑定到设备。");
    }
}

void sub_device_bind_report_req_send(char **send_data)
{
    if(NULL != report_str)
    {
        *send_data = report_str;
        report_str = NULL;
    }
}

//2.8.4
void sub_device_bind_report_rep(char *root_str)
{
    
}


//2.9.1 Sub-device data is automatically reported
int sub_device_data_req(deviceInfo_t *deviceInfo, char **str_send)
{
    char ieeeaddr_string[17] = {0};
    sprintf(ieeeaddr_string,"%02x%02x%02x%02x%02x%02x%02x%02x",\
        deviceInfo->deviceBasic.ieeeAddr[0],\
        deviceInfo->deviceBasic.ieeeAddr[1],\
        deviceInfo->deviceBasic.ieeeAddr[2],\
        deviceInfo->deviceBasic.ieeeAddr[3],\
        deviceInfo->deviceBasic.ieeeAddr[4],\
        deviceInfo->deviceBasic.ieeeAddr[5],\
        deviceInfo->deviceBasic.ieeeAddr[6],\
        deviceInfo->deviceBasic.ieeeAddr[7]);
    debug(1, "-------------->%s deviceStateChange", ieeeaddr_string);
    
    is_subdevice_data_upload = true;
    devices_Info = deviceInfo;
    
    static cJSON *r_result;
    cJSON *result;
    char trx_devid[16];
    sprintf(trx_devid, "dev-%s", get_timestamp());
    trxid_dev = trx_devid;
    debug(1, "trxid_dev = %s", trxid_dev);
    
    local_list_ieeeAddr(&result, ieeeaddr_string);
    //local_list_new(&result);
    //debug(1, "%s\n", cJSON_Print(result));
    get_subdevices_status(&r_result, result, "subdevice_data_upload_req", "sub_device_data");
    //debug(DEBUG_DEBUG, "[device_json] %s\n", cJSON_Print(r_result));
    *str_send = cJSON_Print(r_result);
    is_subdevice_data_upload_req = true;
    
    //cap subdevice statu is change
    /*
    cJSON *sub_device_data;
    cJSON *type_id;
    if(cJSON_GetObjectItem(r_result, "subdevice_data_upload_req"))
    {
        sub_device_data = cJSON_GetObjectItem(r_result, "subdevice_data_upload_req");
        if(cJSON_GetObjectItem(sub_device_data, "type_id"))
        {
            type_id = cJSON_GetObjectItem(sub_device_data, "type_id");
            debug(1, "type_id = %d", type_id->valueint);
            debug(1, "------------------------");
        }
    }
    */
    
    return 0;
}

//2.9.2 Sub-device data is automatically reported
int sub_device_data_upload_rep(char *root_str)
{
    
    printf("-----------------------------------------------\n");
    
    
    return 0;
}


//2.10.1
int sub_device_download_req(char *root_str)
{
    is_subdevice_download_rep = true;
    is_subdevice_data_download = true;
    
    int num, i;
    cJSON *root_req;
    cJSON *subdevice_data_download_req;
    cJSON *sub_device_data;
    cJSON *sub_device;
    cJSON *trx_id;
    
    root_req = cJSON_Parse(root_str);
    subdevice_data_download_req = cJSON_GetObjectItem(root_req, "subdevice_data_download_req");
    sub_device_data = cJSON_GetObjectItem(subdevice_data_download_req, "sub_device_data");
    trx_id = cJSON_GetObjectItem(subdevice_data_download_req, "trxid");
    trxid_down = trx_id->valuestring;
    debug(1, "%s", trxid_down);
    
    //num = cJSON_GetArraySize(sub_device_data);
    /*
    for(i = 0; i < num; i++)
    {
        sub_device = cJSON_GetArrayItem(sub_device_data, num);
        //type    switch    ol    
    }
    */
    
    return 0;
}

//2.10.2
int sub_device_download_rep(char **str)
{
    cJSON *r_result;
    cJSON *result = NULL;
    static char *out;
    
    local_list_new(&result);
    //debug(1, "%s", cJSON_Print(result));
    get_subdevices_status(&r_result, result, "subdevice_data_download_rep", "sub_device_data");
    out = cJSON_Print(r_result);
    //debug(DEBUG_DEBUG, "[device_json] %s\n", out);
    *str = out;
    
    return 0;
}

cJSON *sub_actions_str;

//2.11.1 
int sub_device_action_req(cJSON *r_result)
{
    //1. get cJSON Item
    int num, num_list;
    cJSON *sub_action_req;
    cJSON *sub_actions_list;
    cJSON *sub_actions;
    cJSON *trx_id;
    
    sub_action_req = cJSON_GetObjectItem(r_result, "subdevice_action_req");
    sub_actions_list = cJSON_GetObjectItem(sub_action_req, "sub_actions");
    //debug(1, "--------sub_actions_str--------");
    sub_actions_str = NULL;
    sub_actions_str = cJSON_GetObjectItem(sub_action_req, "sub_actions");
    //debug(1, "%s", cJSON_Print(sub_actions_str));
    if(cJSON_GetObjectItem(sub_action_req, "trxid"))
    {
        trx_id = cJSON_GetObjectItem(sub_action_req, "trxid");
        trxid = trx_id->valuestring;
        debug(1, "%s", trxid);
    }
    
    num_list = cJSON_GetArraySize(sub_actions_list);
    
    for(num = 0; num < num_list; num++)
    {
        sub_actions = cJSON_GetArrayItem(sub_actions_list, num);
        //debug(DEBUG_DEBUG, "%s", cJSON_Print(sub_actions));
        //......
        set_device_status(sub_actions);
    }
    
    return 0;
}

//2.11.2
int sub_device_action_rep(char **send_str)
{
    cJSON *root_reg;
    cJSON *subdevice_action_rep;
    cJSON *sub_actions;
    //cJSON *trx_id;
    char *error_code = "0";
    char *error_info = "NULL";
    char *device_time = get_timestamp();
    static char *out;
    
    root_reg = cJSON_CreateObject();
    cJSON_AddItemToObject(root_reg, "subdevice_action_rep", subdevice_action_rep = cJSON_CreateObject());
    cJSON_AddItemToObject(subdevice_action_rep, "sub_actions", sub_actions = cJSON_CreateObject());
    cJSON_AddStringToObject(sub_actions, "error_code", error_code);
    cJSON_AddStringToObject(sub_actions, "error_info", error_info);
    cJSON_AddStringToObject(subdevice_action_rep, "device_time", device_time);
    if(NULL != trxid)
    {
        cJSON_AddStringToObject(subdevice_action_rep, "trxid", trxid);
    }
    
    out = cJSON_Print(root_reg);
    //debug(1, "%s", out);
    *send_str = out;
    
    return 0;
}

//2.11.3
int sub_device_action_exec_report_req(char **send_str)
{
    //1. Get error code
    SUBDEVICE_ACTION_EXEC ret = ACTION_EXEC_OK;
    
    //2. json format
    int num, num_list;
    cJSON *root_reg;
    cJSON *subdevice_action_exec;
    cJSON *sub_actions;
    cJSON *sub_action;
    cJSON *sub_num;
    
    cJSON *type;
    cJSON *dev_id;
    cJSON *sub_list;
    //cJSON *trx_id;
    char *error_code = "0";
    char *error_info = "NULL";
    char *ip_addr = "10.0.1.144";
    char *device_time = get_timestamp();
    static char *out;
    
    root_reg = cJSON_CreateObject();
    cJSON_AddItemToObject(root_reg, "subdevice_action_exec_report_req", subdevice_action_exec = cJSON_CreateObject());
    //cJSON_AddItemToObject(subdevice_action_exec, "sub_actions", sub_actions = cJSON_CreateObject());
    
    cJSON_AddStringToObject(subdevice_action_exec, "ip_addr", ip_addr);
    cJSON_AddStringToObject(subdevice_action_exec, "device_time", device_time);
    if(NULL != trxid)
    {
        cJSON_AddStringToObject(subdevice_action_exec, "trxid", trxid);
    }
    cJSON_AddItemToObject(subdevice_action_exec, "sub_actions", sub_action = cJSON_CreateArray());
    //debug(1, "%s", cJSON_Print(sub_actions_str));
    num_list = cJSON_GetArraySize(sub_actions_str);
    for(num = 0; num < num_list; num++)
    {
        sub_num = cJSON_CreateObject();
        sub_list = cJSON_GetArrayItem(sub_actions_str, num);
        if(cJSON_GetObjectItem(sub_list, "type"))
        {
            type = cJSON_GetObjectItem(sub_list, "type");
            cJSON_AddStringToObject(sub_num, "type", type->valuestring);
            debug(1, "%s", cJSON_Print(type));
        }
        if(cJSON_GetObjectItem(sub_list, "dev_id"))
        {
            dev_id = cJSON_GetObjectItem(sub_list, "dev_id");
            cJSON_AddStringToObject(sub_num, "dev_id", dev_id->valuestring);
            debug(1, "%s", cJSON_Print(dev_id));
        }
        cJSON_AddStringToObject(sub_num, "error_code", error_code);
        cJSON_AddStringToObject(sub_num, "error_code", error_code);
        cJSON_AddStringToObject(sub_num, "error_info", error_info);
        cJSON_AddItemToArray(sub_action, sub_num);
        //debug(1, "%s", cJSON_Print(sub_num));
    }
    
    out = cJSON_Print(root_reg);
    debug(1, "%s", out);
    *send_str = out;
    sub_num = NULL;
    
    return 0;
}


//2.11.4
int sub_device_action_exec_report()
{
    debug(1, "%s", "-----------sub_device_action_exec_report-----------");
    
    return 0;
}


//2.13.1 Sub devices unbind   coord ieee addr: 00  12  4b  00  0e  4f  bf  94 
int unbind_sub_req(char *rec_str)
{
    int ret;
    cJSON *root_req;
    cJSON *unbind_sub_req;
    cJSON *unbind_sub;
    cJSON *cloud_time;
    
    root_req = cJSON_Parse(rec_str);
    unbind_sub_req = cJSON_GetObjectItem(root_req, "subdevice_unbind_req");
    unbind_sub = cJSON_GetObjectItem(unbind_sub_req, "unbind_sub");
    cloud_time = cJSON_GetObjectItem(unbind_sub_req, "cloud_time");
    
    printf("[device_json] %s\n", unbind_sub->valuestring);
    printf("[device_json] %s\n", cloud_time->valuestring);
    
    if(0 == strncmp("true", unbind_sub->valuestring, 4))
    {
        //Start binding the sub devices
        ret = unbind_subdevices();
    }
    
    if(ret >= 0)
        return 0;
    else 
        return -1;
}

//2.13.2
void unbind_sub_rep(char **send_str)
{
    static char *out;
    char *error_code = "0";
    char *error_info = "NULL";
    char *ip_addr = "10.0.0.103"; //get ipaddress
    char *device_time = get_timestamp();
    
    cJSON *root_rep;
    cJSON *bind_sub_rep;
    
    root_rep = cJSON_CreateObject();
    cJSON_AddItemToObject(root_rep, "subdevice_unbind_rep", bind_sub_rep = cJSON_CreateObject());
    cJSON_AddStringToObject(bind_sub_rep, "error_code", error_code);
    cJSON_AddStringToObject(bind_sub_rep, "error_info", error_info);
    cJSON_AddStringToObject(bind_sub_rep, "ip_addr", ip_addr);
    cJSON_AddStringToObject(bind_sub_rep, "device_time", device_time);
    out = cJSON_Print(root_rep);
    
    printf("[device_json] %s\n", out);
    *send_str = out;
}


//2.14.1 
int device_avs_activation(char *rec_str)
{
    cJSON *root_req;
    cJSON *device_avs_activation;
    cJSON *txid;
    cJSON *activate;
    cJSON *cloud_time;
    
    root_req = cJSON_Parse(rec_str);
    device_avs_activation = cJSON_GetObjectItem(root_req, "device_avs_activation");
    txid = cJSON_GetObjectItem(device_avs_activation, "txid");
    activate = cJSON_GetObjectItem(device_avs_activation, "activate");
    cloud_time = cJSON_GetObjectItem(device_avs_activation, "cloud_time");
    
    trxid_avs = txid->valuestring;
    printf("[device_json] %s\n", trxid_avs);
    printf("[device_json] %s\n", cloud_time->valuestring);
    
    if(0 == strncmp("true", activate->valuestring, 4))
    {
        //Start avs 
        is_avs_activation = true;
    }
    
    return 0;
}

//2.14.2
int device_avs_activation_rep(char **send_str)
{
    cJSON *root_rep;
    cJSON *bind_sub_rep;
    static char *out;
    char *sn = "2e4bf13c4570cc4a4b92da28fc6a62";
    char *code_verifier;
    char *code_challenge;
    char *error_code = "0";
    char *error_info = "NULL";
    char *device_time = get_timestamp();
    
    avs_code_verifier(&code_verifier, &code_challenge);
    debug(1, "%s %s", code_verifier, code_challenge);
    
    root_rep = cJSON_CreateObject();
    cJSON_AddItemToObject(root_rep, "device_avs_activation_rep", bind_sub_rep = cJSON_CreateObject());
    cJSON_AddStringToObject(bind_sub_rep, "code_verifier", code_verifier);
    cJSON_AddStringToObject(bind_sub_rep, "code_challenge", code_challenge);
    cJSON_AddStringToObject(bind_sub_rep, "sn", sn);
    if(NULL != trxid_avs)
    {
        cJSON_AddStringToObject(bind_sub_rep, "txid", trxid_avs);
    }
    cJSON_AddStringToObject(bind_sub_rep, "error_code", error_code);
    cJSON_AddStringToObject(bind_sub_rep, "error_info", error_info);
    cJSON_AddStringToObject(bind_sub_rep, "device_time", device_time);
    out = cJSON_Print(root_rep);
    
    debug(1, "[device_json] %s\n", out);
    *send_str = out;
    
    return 0;
}

//2.14.3
int device_avs_auth()
{
    
    
    return 0;
}

//2.14.4
int device_avs_auth_rep()
{
    
    
    return 0;
}


/*
{
    "device_ai_req": {
         "trxid": "AI-201711132110asc9saf399dsf8401",
         "user_id": "372a48db86f4ed0c3db33b0c3bb0553620482e441cedc2151af029fd40a0c5a3",
         "query": "上海天气"
    }
}
*/

char *ai_string;

//2.15.1 AI Servie
int ai_service_req(char *query_string)
{
    cJSON *root_req;
    cJSON *device_ai_req;
    char *user_id;
    char trxid_ai[32];
    
    sprintf(trxid_ai, "AI-%s%s", get_timestamp(), get_random_string(16));
    debug(1, "trxid_ai : %s", trxid_ai);
    debug(1, "query_string : %s", query_string);
    
    if(NULL != read_userid())
    {
        user_id = read_userid();
        debug(1, "%s len = %d", user_id, strlen(user_id));
    }
    else
    {
        debug(1, "<------------------------------------------------>");
        debug(1, "--  The user_id is NULL!  Please reload the network! --");
        debug(1, "<------------------------------------------------>");
    }
    
    root_req = cJSON_CreateObject();
    cJSON_AddItemToObject(root_req, "device_ai_req", device_ai_req = cJSON_CreateObject());
    cJSON_AddStringToObject(device_ai_req, "trxid", trxid_ai);
    cJSON_AddStringToObject(device_ai_req, "user_id", user_id);
    cJSON_AddStringToObject(device_ai_req, "query", query_string);
    ai_string = cJSON_Print(root_req);
    //debug(1, "%s", ai_string);
    
    is_ai_service_req = true;
    
    return 0;
}

void ai_service_req_send(char **send_data)
{
    if(NULL != ai_string)
    {
        *send_data = ai_string;
        ai_string = NULL;
    }
}

/*
{
    "device_ai_rep": {
         "trxid": "AI-xxxxxxxx", 
         "intention_id":  <intention的ID，数值型>,    # 具体intention所对应值见下表
         "domain":  <领域表示>,    # 表示AI解析出的语音指令所属领域方向，如音乐查询则该字段内容为"music"，天气查询则该字段内容为"weather"，等。该字段仅作参考，响应的具体逻辑如何处理由intention_id字段决定
         "speech": "<云端执行完后所作回答的文本，必须由音箱合成语音并播放>",
         "results":  [],     # 若指令被AI解析为设备控制或闲聊，则该列表为空(长度0)
         "error_code": "返回码， 0 代表无错误"，非0时，具体错误信息由speech返回。
    }
}
*/

//2.15.2
int ai_service_rep(cJSON *root_rep)
{
    cJSON *device_ai_rep;
    cJSON *trxid;
    cJSON *intention_id;
    cJSON *domain;
    cJSON *speech;
    cJSON *results;
    cJSON *error_code;
    cJSON *result;
    
    int results_len, num;
    int intention_value;
    char *domain_str;
    char *speech_str;
    char *error_value;
    
    device_ai_rep = cJSON_GetObjectItem(root_rep, "device_ai_rep");
    trxid = cJSON_GetObjectItem(device_ai_rep, "trxid");
    intention_id = cJSON_GetObjectItem(device_ai_rep, "intention_id");
    domain = cJSON_GetObjectItem(device_ai_rep, "domain");
    speech = cJSON_GetObjectItem(device_ai_rep, "speech");
    results = cJSON_GetObjectItem(device_ai_rep, "results");
    error_code = cJSON_GetObjectItem(device_ai_rep, "error_code");
    
    debug(1, "trxid : %s", trxid->valuestring);
    debug(1, "intention_id : %d", intention_id->valueint);
    debug(1, "domain : %s", domain->valuestring);
    debug(1, "speech : %s", speech->valuestring);
    debug(1, "error_code : %d", domain->valueint);
    
    if(intention_id->valueint)
    {
        intention_value = intention_id->valueint;
    }
    
    led_driver(ALL_LEDS_BLUE);
    is_reply = true;
    
    //device_ai_speaker(speech->valuestring);
    //intention_id
    switch(intention_value)
    {
        case -10000:
        {
            cJSON *res_results;
            debug(1, "result ----->  -10000");
            
            results_len = cJSON_GetArraySize(results);
            if(results_len > 0)
            {
                res_results = cJSON_GetArrayItem(results, 0);
                
                if(cJSON_Print(res_results))
                {
                    debug(1, "result %d : %s", 0, cJSON_Print(res_results));
                    char speech_buf[2048];
                    memset(speech_buf, 0, sizeof(speech_buf));
                    
                    if(!strncmp(domain->valuestring, "time", 4))
                    {
                        debug(1, "time");
                        sprintf(speech_buf, "%s", speech->valuestring);
                    }
                    else if(!strncmp(domain->valuestring, "people", 6))
                    {
                        debug(1, "people");
                        if(cJSON_GetObjectItem(res_results, "persons"))
                        {
                            cJSON *persons = cJSON_GetObjectItem(res_results, "persons");
                            int persons_len = cJSON_GetArraySize(persons);
                            debug(1, "%d", persons_len);
                            if(persons_len > 0)
                            {
                                cJSON *res_persons = cJSON_GetArrayItem(persons, 0);
                                if(cJSON_GetObjectItem(res_persons, "brief"))
                                {
                                    cJSON *brief = cJSON_GetObjectItem(res_persons, "brief");
                                    //debug(1, "%s", brief->valuestring);
                                    const char *brief_str = brief->valuestring;
                                    int count = 0;
                                    bool is_end = false;
                                    char brief_string[2048];
                                    memset(brief_string, 0, 2048);
                                    while((*brief_str) && (count >= 1900))
                                    {
                                        count++;
                                        if(!strncmp("。", brief_str, 2))
                                        {
                                            is_end = true;
                                            memcpy(brief_string, brief->valuestring, count+2);
                                            debug(1, "%d  %s", count, brief_string);
                                            break;
                                        }
                                        brief_str++;
                                    }
                                    brief_str = NULL;
                                    if(!is_end)
                                    {
                                        memcpy(brief_string, brief->valuestring, 2048);
                                    }
                                    sprintf(speech_buf, "%s", brief_string);
                                    //sprintf(speech_buf, "%s", brief->valuestring);
                                    is_people = true;
                                }
                            }
                        }
                    }
                    else if(!strncmp(domain->valuestring, "news", 4))
                    {
                        sprintf(speech_buf, "%s", speech->valuestring);
                    }
                    else if(!strncmp(domain->valuestring, "story", 5))
                    {
                        sprintf(speech_buf, "%s", res_results->valuestring);
                    }
                    else if(!strncmp(domain->valuestring, "radio", 5))
                    {
                        sprintf(speech_buf, "%s", res_results->valuestring);
                    }
                    else
                    {
                        sprintf(speech_buf, "%s", speech->valuestring);
                    }
                    debug(1, "%s", speech_buf);
                    
                    device_ai_speaker(speech_buf);
                }
                else
                {
                    debug(1, "result %d : %s", 0, res_results->valuestring);
                    device_ai_speaker(res_results->valuestring);
                }
                
                results_len = 0;
            }
            else
            {
                debug(1, "speech %s", speech->valuestring);
                device_ai_speaker(speech->valuestring);
            }
            
            break;
        }
        case 10000: //chatting    chatting
        {
            device_ai_speaker(speech->valuestring);
            break;
        }
        case 10100: //searching    music
        {
            cJSON *song_one;
            cJSON *song_url;
            cJSON *song_name;
            cJSON *song_singer;
            cJSON *song_singer_name;
            
            results_len = cJSON_GetArraySize(results);
            debug(1, "results_len = %d", results_len);
            for(num = 0; num < results_len; num++)
            {
                result = cJSON_GetArrayItem(results, num);
                debug(1, "result %d : %s", num, cJSON_Print(result));
            }
            if(results_len > 0)
            {
                int flag_num;
                do
                {
                    flag_num = rand() % 10;
                }
                while(flag_num >= results_len);
                debug(1, "flag_num %d ", flag_num);
                if(flag_num <= results_len)
                {
                    song_one = cJSON_GetArrayItem(results, flag_num);
                }
                else
                {
                    song_one = cJSON_GetArrayItem(results, 0);
                }
                song_url = cJSON_GetObjectItem(song_one, "song_url");
                song_name = cJSON_GetObjectItem(song_one, "name");
                song_singer = cJSON_GetObjectItem(song_one, "singer");
                song_singer_name = cJSON_GetArrayItem(song_singer, 0);
                char song_detail[128];
                sprintf(song_detail, "为您播放%s的歌曲%s", song_singer_name->valuestring, song_name->valuestring);
                device_ai_speaker(song_detail);
                debug(1, "%s", song_detail);
                sleep(5);
                
                char song_buf[256];
                sprintf(song_buf, "%s", song_url->valuestring);
                azplay(mode_3, song_buf);
                results_len = 0;
            }
            else
            {
                device_ai_speaker(speech->valuestring);
            }
            break;
        }
        case 10101: //searching    movie
        {
            device_ai_speaker(speech->valuestring);
            break;
        }
        case 10102: //searching    weather
        {
            device_ai_speaker(speech->valuestring);
            break;
        }
        case 10200: //instructing    light-control
        {
            device_ai_speaker(speech->valuestring);
            printf("----> light-control\n");
            break;
        }
        case 10201: //instructing    curtain-control
        {
            device_ai_speaker(speech->valuestring);
            printf("----> curtain-control\n");
            break;
        }
        case 10202: //instructing    door-control
        {
            device_ai_speaker(speech->valuestring);
            break;
        }
        case 10203: //sound
        {
            device_ai_speaker(speech->valuestring);
            break;
        }
        case 10210: //bind
        {
            if(!is_onbind)
            {
                if(is_device_ready)
                {
                    
                }
                device_ai_speaker(speech->valuestring);
                printf("----> bind-control\n");
            }
            break;
        }
        case 10211: //volumn
        {
            cJSON *res_one;
            
            debug(1, "----> volumn-control");
            //device_ai_speaker(speech->valuestring);
            results_len = cJSON_GetArraySize(results);
            
            if(results_len > 0)
            {
                debug(1, "----> results_len %d ", results_len);
                res_one = cJSON_GetArrayItem(results, 0);
                if((res_one->valueint) || (0 == res_one->valueint))
                {
                    debug(1, "result  %d", res_one->valueint);
                    if(-100 == res_one->valueint)
                    {
                        debug(1, "----> results_len %d ", results_len);
                        control_volume(VOLUME_CLOUD_UP);
                    }
                    else if(-101 == res_one->valueint)
                    {
                        debug(1, "----> results_len %d ", results_len);
                        control_volume(VOLUME_CLOUD_DOWN);
                    }
                    else if(0 == res_one->valueint)
                    {
                        debug(1, "----> results_len %d ", results_len);
                        button_set_mute();
                    }
                    else
                    {
                        if(res_one->valueint)
                        {
                            debug(1, "buf_volumn  %d", res_one->valueint);
                            if(100 == res_one->valueint)
                            {
                                set_volume(100);
                            }
                            else
                            {
                                set_volume(res_one->valueint);
                            }
                        }
                    }
                    results_len = 0;
                }
            }
            sleep(1);
            device_ai_speaker(speech->valuestring);
            break;
        }
        case 10500: //calculating    tool
        {
            device_ai_speaker(speech->valuestring);
            break;
        }
        case 10600: //traffic    map
        {
            device_ai_speaker(speech->valuestring);
            break;
        }
        case 10700: //routing    map
        {
            device_ai_speaker(speech->valuestring);
            break;
        }
        default:
        {
            device_ai_speaker(speech->valuestring);
            break;
        }
    }
    
    return 0;
}


char *ota_str = NULL;

void *device_th(void *p)
{
    if(!is_ota_download)
    {
        is_ota_download = true;
        ota_request(ota_str);
        debug(1, "----------------------------------------------");
        ota_str = NULL;
    }
    
    pthread_exit(0);
}


/**Note: 
**Handle mqtt received the message
**/
int device_cjson(char *rec_str)
{
    //debug(1, "----------------------------------------------\n%s", rec_str);
    
    if(cJSON_Parse(rec_str))
    {
        int ret;
        char *root_str;
        cJSON *root_req;
        cJSON *recv_req;
        
        root_req = cJSON_Parse(rec_str);
        root_str = cJSON_Print(root_req);
        if(NULL == root_req)
        {
            printf("error:%s\n", cJSON_GetErrorPtr());
            cJSON_Delete(root_req);
            return -1;
        }
        
        
        //Parse the received json, see what data is received
        //2.1.2 cloud on-line response
        recv_req = cJSON_GetObjectItem(root_req, "online_rep");
        if (recv_req != NULL)
        {
            //debug(DEBUG_DEBUG, "%s", cJSON_Print(recv_req));
            device_online_rep(root_str);
            recv_req = NULL;
        }
        
        
        //2.2.2 Device heartbeat
        recv_req = cJSON_GetObjectItem(root_req, "device_hb_rep");
        if (recv_req != NULL)
        {
            //debug(DEBUG_DEBUG, "%s", cJSON_Print(recv_req));
            device_hb_rep(root_str);
            recv_req = NULL;
        }
        
        
        //2.3.2
        recv_req = cJSON_GetObjectItem(root_req, "device_data_upload_rep");
        if (recv_req != NULL)
        {
            //debug(DEBUG_DEBUG, "%s", cJSON_Print(recv_req));
            device_data_upload_rep(root_str);
            recv_req = NULL;
        }
        
        
        //2.5.1 Device data
        if(cJSON_GetObjectItem(root_req, "device_data_download_req"))
        {
            recv_req = cJSON_GetObjectItem(root_req, "device_data_download_req");
            if (recv_req != NULL)
            {
                //debug(DEBUG_DEBUG, "%s", cJSON_Print(recv_req));
                device_data_download(root_str);
                recv_req = NULL;
            }
        }
        
        
        //2.6.1 OTA
        if(cJSON_GetObjectItem(root_req, "ota_req"))
        {
            recv_req = cJSON_GetObjectItem(root_req, "ota_req");
            if (recv_req != NULL)
            {
                //debug(DEBUG_DEBUG, "%s", cJSON_Print(recv_req));
                //ota_request(root_str);
                ota_str = root_str;
                pthread_t p_device;
                pthread_create(&p_device, NULL, (void *)device_th, NULL);
                recv_req = NULL;
            }
        }
        
        //2.6.4
        if(cJSON_GetObjectItem(root_req, "ota_status_report_rep"))
        {
            recv_req = cJSON_GetObjectItem(root_req, "ota_status_report_rep");
            if (recv_req != NULL)
            {
                //debug(DEBUG_DEBUG, "%s", cJSON_Print(recv_req));
                ota_status_report_rep(root_str);
                recv_req = NULL;
            }
        }
        
        
        //2.8.1 Sub device binding
        recv_req = cJSON_GetObjectItem(root_req, "subdevice_bind_req");
        if (recv_req != NULL)
        {
            //debug(DEBUG_DEBUG, "%s", cJSON_Print(recv_req));
            ret = sub_device_bind_req(root_str);
            //2.8.2
            recv_req = NULL;
        }
        
        //2.8.3 subdevice_bind_report() Not here, when gateway scan is end & Debug here //TEST
        recv_req = cJSON_GetObjectItem(root_req, "subdevice_bind_report_req_test");
        if (recv_req != NULL)
        {
            //debug(DEBUG_DEBUG, "%s", cJSON_Print(recv_req));
            subdevice_bind_report();
            recv_req = NULL;
        }
        
        //2.8.4
        recv_req = cJSON_GetObjectItem(root_req, "subdevice_bind_report_rep");
        if (recv_req != NULL)
        {
            //debug(DEBUG_DEBUG, "%s", cJSON_Print(recv_req));
            sub_device_bind_report_rep(root_str);
            recv_req = NULL;
        }
        
        
        //2.9.1  subdevice_data_upload_req                                                 //TEST
        recv_req = cJSON_GetObjectItem(root_req, "subdevice_data_upload_req_test");
        if (recv_req != NULL)
        {
            //debug(DEBUG_DEBUG, "%s", cJSON_Print(recv_req));
            char *str;
            char *ieeeAddr = "00124b0007fde916";
            //sub_device_data_req();
            debug(1, "%s", str);
            recv_req = NULL;
        }
        
        //2.9.2
        recv_req = cJSON_GetObjectItem(root_req, "subdevice_data_upload_rep");
        if (recv_req != NULL)
        {
            //debug(DEBUG_DEBUG, "%s", cJSON_Print(recv_req));
            sub_device_data_upload_rep(root_str);
            recv_req = NULL;
        }
        
        
        //2.10.1
        recv_req = cJSON_GetObjectItem(root_req, "subdevice_data_download_req");
        if (recv_req != NULL)
        {
            //debug(DEBUG_DEBUG, "%s", cJSON_Print(recv_req));
            sub_device_download_req(root_str);
            recv_req = NULL;
        }
        
        //2.10.2  It should be seperation json string and create a new string //TEST
        recv_req = cJSON_GetObjectItem(root_req, "subdevice_data_download_rep_test");
        if (recv_req != NULL)
        {
            //debug(DEBUG_DEBUG, "%s", cJSON_Print(recv_req));
            char *out_str;
            sub_device_download_rep(&out_str);
            debug(1, "%s", out_str);
            recv_req = NULL;
        }
        
        
        //2.11.1
        recv_req = cJSON_GetObjectItem(root_req, "subdevice_action_req");
        if (recv_req != NULL)
        {
            is_sub_device_action_rep = true;
            //debug(DEBUG_DEBUG, "%s", cJSON_Print(recv_req));
            sub_device_action_req(root_req);
            recv_req = NULL;
        }
        
        //2.11.2  Test
        
        //2.11.3  Test
        
        //2.11.4  
        recv_req = cJSON_GetObjectItem(root_req, "subdevice_action_exec_report_req");
        if (recv_req != NULL)
        {
            //debug(DEBUG_DEBUG, "%s", cJSON_Print(recv_req));
            sub_device_action_exec_report(root_req);
            recv_req = NULL;
        }
        
        
        //2.13.1
        recv_req = cJSON_GetObjectItem(root_req, "subdevice_unbind_req");
        if (recv_req != NULL)
        {
            debug(DEBUG_DEBUG, "%s", cJSON_Print(recv_req));
            ret = unbind_sub_req(root_str);
            if(ret >= 0)
            {
                is_device_unbind_report_req = true;
            }
            recv_req = NULL;
        }
        
        /*
        //2.14.1
        recv_req = cJSON_GetObjectItem(root_req, "device_avs_activation_req");
        if (recv_req != NULL)
        {
            debug(DEBUG_DEBUG, "%s", cJSON_Print(recv_req));
            device_avs_activation(root_str);
            recv_req = NULL;
        }
        */
        
        //2.15 
        recv_req = cJSON_GetObjectItem(root_req, "device_ai_rep");
        if (recv_req != NULL)
        {
            //debug(DEBUG_DEBUG, "%s", cJSON_Print(recv_req));
            ai_service_rep(root_req);
            recv_req = NULL;
        }
        
        
    }
    else
    {
        //Get json format error!
        debug(1, "----------------------------------------------");
    }
    
    
    return 0;
}
