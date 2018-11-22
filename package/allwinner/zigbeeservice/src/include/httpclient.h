#ifndef __HTTPCLIENT_H__
#define __HTTPCLIENT_H__


#define     BUFF_SIZE           256
#define     BUFF_DEVICE       32

#define BUILD_RELEASE 0
#define BUILD_TEST       1
#define BUILD_BUILD     2

#define HTTPS_R_POST_REGISTER_URL    "http://192.168.1.28/imio-cloud/dev_reg_v1"
#define HTTPS_R_POST_LOGIN_URL          "http://192.168.1.28/imio-cloud/dev_online_v1"
#define HTTPS_D_POST_REGISTER_URL    "http://192.168.1.28/imio-cloud/dev_reg_v1"
#define HTTPS_D_POST_LOGIN_URL          "http://192.168.1.28/imio-cloud/dev_online_v1"
#define HTTPS_B_POST_REGISTER_URL    "http://192.168.1.28/imio-cloud/dev_reg_v1"
#define HTTPS_B_POST_LOGIN_URL          "http://192.168.1.28/imio-cloud/dev_online_v1"

#define FILE_PATH                               "/etc/config"
#define FILE_UPDATE_PATH                "/mnt/UDISK/misc-upgrade"
#define FILE_DEVICE_ID                     "/etc/config/dev_id"
#define FILE_ONLINE_ID                    "/etc/config/dev_ol"
#define FILE_USER_ID                         "/etc/config/user_id"
#define FILE_PRODUCT_ID                  "/etc/config/product_id"
#define FILE_PRODUCT_SECRET          "/etc/config/product_secret"
#define FILE_DEV_ID                           "/etc/config/device_id"

#define PRODUCT_ID              "IMIO-Circle_01"
#define PRODUCT_SECRET      "imio_circle_0120171222d3mPN1Ypy8d6t2F240" //40
#define DEV_ID                       "R16-"
#define DEV_TYPE                    "10020-01"

#define DEV_KEY                     "8ac21755dad4e5ebde1b5ac7d10dd476"


//#define USER_ID "09e209df546eacc0c9f19e3a3c5a323be5f9a32537b90e06e880aa4bc0609aa8"


/** @brief   This enumeration defines the API return type.  */
typedef enum {
    HTTPCLIENT_ERROR_PARSE = -6,           /**< A URL parse error occurred. */
    HTTPCLIENT_UNRESOLVED_DNS = -5,        /**< Could not resolve the hostname. */
    HTTPCLIENT_ERROR_PRTCL = -4,           /**< A protocol error occurred. */
    HTTPCLIENT_ERROR = -3,                 /**< An unknown error occurred. */
    HTTPCLIENT_CLOSED = -2,                /**< Connection was closed by a remote host. */
    HTTPCLIENT_ERROR_CONN = -1,            /**< Connection failed. */
    HTTPCLIENT_OK = 0,                     /**< The operation was successful. */
    HTTPCLIENT_RETRIEVE_MORE_DATA = 1      /**< More data needs to be retrieved. */
} HTTPCLIENT_RESULT;

typedef enum {
    HTTPCLIENT_JSON_UNKNOW = -3,
    HTTPCLIENT_JSON_ERROR = -2,
    HTTPCLIENT_JSON_ERRCODE = -1,
    HTTPCLIENT_JSON_OK = 0
} HTTPCLIENT_JSON;

typedef enum{
    FILE_ERROR = -5,
    FILE_NOT_EXIST = -4,
    FILE_NOT_OPEN = -3,
    FILE_NOT_REG = -2,
    FILE_NOT_ONLINE = -1,
    FILE_OK = 0,
    FILE_RELOAD = 1
} FILE_RESULT;


extern int read_register(char **);
extern int read_online(char **, char **);
extern char* read_userid();

extern HTTPCLIENT_RESULT httpclient_register();
extern HTTPCLIENT_RESULT httpclient_login(char *);
extern HTTPCLIENT_JSON httpclient_login_prepare(char *);
extern HTTPCLIENT_RESULT httpclient_ota(char *, char *, char *);
extern void httpclient_relaod();

extern char *read_version();
extern int write_version(char *);
extern int baidu_tts_init();
extern int baidu_wav(char *, int , int , int , int );


#endif /* __HTTPCLIENT_H__ */
