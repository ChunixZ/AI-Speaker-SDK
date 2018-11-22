#ifndef _DEVICE_JSON_H_
#define _DEVICE_JSON_H_


#include <stdbool.h>
#include "zigbee_device.h"
#include "user_types.h"
#include "ubus.h"



#define SHA256_ROTL(a,b) (((a>>(32-b))&(0x7fffffff>>(31-b)))|(a<<b))
#define SHA256_SR(a,b) ((a>>b)&(0x7fffffff>>(b-1)))
#define SHA256_Ch(x,y,z) ((x&y)^((~x)&z))
#define SHA256_Maj(x,y,z) ((x&y)^(x&z)^(y&z))
#define SHA256_E0(x) (SHA256_ROTL(x,30)^SHA256_ROTL(x,19)^SHA256_ROTL(x,10))
#define SHA256_E1(x) (SHA256_ROTL(x,26)^SHA256_ROTL(x,21)^SHA256_ROTL(x,7))
#define SHA256_O0(x) (SHA256_ROTL(x,25)^SHA256_ROTL(x,14)^SHA256_SR(x,3))
#define SHA256_O1(x) (SHA256_ROTL(x,15)^SHA256_ROTL(x,13)^SHA256_SR(x,10))

#define MAX_PATH 256


//2.6
typedef enum{
    OTA_ERROR = -1,
    OTA_NORMAL = 0,
    OTA_START,
    OTA_SEND,
    OTA_DOWNLOAD,
    OTA_MD5,
    OTA_UPDATE,
    OTA_SUCCESS,
}OTA_STATUS;

//2.11.3
typedef enum{
    ACTION_EXEC_ERROR = -1,
    ACTION_EXEC_OK = 0
}SUBDEVICE_ACTION_EXEC;


typedef struct
{
    bool is_subBind;
    bool is_subBind_res;
}mqtt_pubulish;

typedef struct 
{
    bool online;
    char ip_addr[12];
    char product_id[17];
    char product_sercet[32];
    char dev_id[32];
    char dev_sercet[32];
}deviceUpload_data;

typedef struct
{
    bool online;
    int status;
    char sub_product_id[32];
}subDevice_data;


extern int read_system(char *);
extern char *get_timestamp();
extern char* get_random_string(int);
extern int get_sub_devices_status(deviceInfo_t **, char *);

//2.1
extern void device_online_req(char **, char *);
extern void device_online_rep(char *);

//2.2
extern void device_hb_req(char **);
extern void device_hb_rep(char *);

//2.5
extern int device_data_download_rep(char **);

//2.6
extern int ota_request_rep(char **);
extern int ota_status_report(char **);

//2.8
extern void sub_device_bind_rep(char **);
extern void sub_device_bind_report_req(cJSON *);
extern void sub_device_bind_report_req_send(char **);

//2.9.1
extern int sub_device_data_req(deviceInfo_t *, char **);

//2.10.1
extern int sub_device_download_req(char *);

//2.10.2
extern int sub_device_download_rep(char **);

//2.11.2
extern int sub_device_action_rep(char **);

//2.11.3
extern int sub_device_action_exec_report_req(char **);

//2.12

extern int device_cjson(char *);

//2.13.2
extern void unbind_sub_rep(char **);

//2.14.2
extern int device_avs_activation_rep(char **);

//2.15.1
extern int ai_service_req(char *);
extern void ai_service_req_send(char **);



//test switch
extern int device_test_switch(int, int);
extern int device_test_bind(char *);
extern int raspberry_test_bind(char *);


#endif
