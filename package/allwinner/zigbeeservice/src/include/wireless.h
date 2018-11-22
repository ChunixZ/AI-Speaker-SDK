#ifndef _WIRELESS_H_
#define _WIRELESS_H_


#define UDP_PORT 9090
#define UDP_SEND 9091
#define BUFFER 1024
#define UDP_SUCCESS "Success"
#define FAIL "Fail"
#define NETWORK_START         1
#define NETWORK_CONNECT    2

#define SEND_SUCCESS "{\"udpsuccess\":\"true\"}"


#define debug(level,format,...)        do{    \
                                        printf("[%s][%d]", __FILE__, __LINE__);    \
                                        printf(format, ##__VA_ARGS__);    \
                                        printf("\n");    \
                                    }while(0)


typedef enum {
    WIFI_DISCONNECTED = -1,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    WIFI_CONNECTFAIL,
    WIFI_RECONNECT,
    WIFI_AP_MODE,
}tWIFI_STATE;


typedef enum {
    NETWORK_DISCONNECTED = 1,
    NETWORK_CONNECTED = 2,
}tNETWORK_STATE;


extern int wifi_main();
extern int network_ap();


#endif
