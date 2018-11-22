
#ifndef _MULTIROOM_H_
#define _MULTIROOM_H_

/*********************
    for multi-room 
***********************/
#ifdef __MULTI_ROOM__
//#define __UDP_SEARCH_DEV__
#ifndef __UDP_SEARCH_DEV__
#define __ANROID_APP_LINK__
#endif
#endif


struct group_udp{    
	int ip_port;    
    char ip_addr[16];
};

struct tcp_msg_head{
    uint8_t type;
    uint8_t activity;
};

typedef struct tcp_timesamp_msg{
    int position; //s
    int num;    //pk num of 1s
    struct timeval server_time;
}*P_TCP_TIMESAMP_MSG, S_TCP_TIMESAMP_MSG;

#define TCP_PORT 1125

#define APP_PORT 1229

//active
#define NOTICE_MSG  1
#define REQUEST_MSG 0
//type
enum {
    ACT_SYNC_PLAY_TIME = 11,
    ACT_ADD_IN_PLAYING,
    ACT_PLAYER_START,
    ACT_PLAYER_STOP,
    ACT_PLAYER_PAUSE,
    ACT_PLAYER_BUFFERING,
    ACT_PLAYER_SEEK,
    ACT_PLAYER_UPDATE_VOLUME,
    ACT_PTHREAD_START,

    ACT_SYNC_OVER,
    
    ACT_SYNC_END
};

#define STA_WAIT  0
#define STA_READY 1

#define MAX_DEV 8

#define TO_ALL 0
#define TO_SYNC_ONE 1


extern int tcp_client_enable;
extern int tcp_serv_enable;
extern int sync_serv_enable;
extern int sync_cli_enable;

//#define RET_SUCCESS 0xFF

typedef struct  
{  
    void (*open_service)(void);
    void (*close_service)(void);
    int  (*get_volume)(void);  
    void (*set_volume)(int vol);
    void (*stop)(void);
    
}multi_callback;

extern void* app_connection_creat_pthread(void);

#endif

