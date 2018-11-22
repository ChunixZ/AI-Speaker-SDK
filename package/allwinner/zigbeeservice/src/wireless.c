/*************************************************************************
    > File Name: wireless.c
    > Author: zyc
    > Mail: zyc@imio.ai 
    > Created Time: 2017/09/10
 ************************************************************************/
#include "wireless.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <errno.h>
#include <net/if.h>
#include "cJSON.h"
#include "cap.h"
#include "driver_led_key.h"
//#include "device_json.h"



bool is_wireless_ready = false;
bool is_ap_mode = false;
bool is_ap_connect = false;
bool is_station = false;
bool is_connected = false;
bool is_reply = false;
int ap_mode = 0;
int station_mode = 0;
int wifi_count = 0;
int connected_count = 0;
unsigned int wfcount = 0;
int reply_count = 0;
int online_count = 0;
int wifi_status = WIFI_DISCONNECTED;

extern bool is_volume_led;
extern bool is_led_speak;
extern bool is_mute;
extern bool is_device_online;
extern bool is_heartbeat_normal;
extern bool is_httpclient_ok;
extern volatile sig_atomic_t in_aborting;


tWIFI_STATE wifi_connect_status()
{
    tWIFI_STATE ret = WIFI_DISCONNECTED;
    
    //debug(1, "Start ping ...\n");
    ret = system("ping 115.239.211.112 -c 2"); //www.baidu.com
    //debug(1, "ret = %d\n", ret);
    
    if(WIFI_CONNECTED == ret)
    {
        //debug(1, "Ping success!\n");
        ret = WIFI_CONNECTED;
    }
    else
    {
        //debug(1, "Ping failed!\n");
    }
    
    return ret;
}

char* read_user_id()
{
    static char buffer[128];
    FILE *fp;
    
    if((fp = fopen("/etc/config/user_id", "r")) != NULL)
    {
        memset(buffer, 0, 128);
        fread(buffer, 1, 128, fp);
        debug(1, "%s", buffer);
    }
    fclose(fp);
    
    return buffer;
}

int write_userid(char *user_id)
{
    FILE *fp;
    
    fp = fopen("/etc/config/user_id", "w");
    fwrite(user_id, strlen(user_id), 1, fp);
    fclose(fp);
    
    return 0;
}

char* read_wifi()
{
    static char buffer[16];
    FILE *fp;
    
    if((fp = fopen("/etc/config/ap_name", "r")) != NULL)
    {
        memset(buffer, 0, 16);
        fread(buffer, 1, 16, fp);
        //debug(1, "%s", buffer);
    }
    fclose(fp);
    
    return buffer;
}

int write_wifi(char *ssid)
{
    FILE *fp;
    
    fp = fopen("/etc/config/ap_name", "w");
    fwrite(ssid, strlen(ssid), 1, fp);
    fclose(fp);
    
    return 0;
}

int udp_server(char *msg)
{
    //char msg[128] = "I am broadCast message from server!";
    int brdcFd;
    
    if((brdcFd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        debug(1, "socket fail\n");
        return -1;
    }
    
    int optval = 1;
    int sendBytes;
    struct sockaddr_in theirAddr;
    
    setsockopt(brdcFd, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &optval, sizeof(int));
    memset(&theirAddr, 0, sizeof(struct sockaddr_in));
    
    theirAddr.sin_family = AF_INET;
    theirAddr.sin_addr.s_addr = inet_addr("192.168.5.255");
    theirAddr.sin_port = htons(UDP_SEND);
    
    if((sendBytes = sendto(brdcFd, msg, strlen(msg), 0,
            (struct sockaddr *)&theirAddr, sizeof(struct sockaddr))) == -1)
    {
        debug(1, "sendto fail, errno=%d\n", errno);
        return -1;
    }
    
    debug(1, "msg=%s, msgLen=%ld, sendBytes=%d\n", msg, strlen(msg), sendBytes);
    close(brdcFd);
    
    return 0;
}

int client_udp(char **recieve_date)
{
    int sockListen;
    int set = 1;
    struct sockaddr_in recvAddr;
    int recvbytes;
    static char recvbuf[BUFFER];
    int addrLen = sizeof(struct sockaddr_in);
    
    if((sockListen = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        debug(1, "socket fail\n");
        return -1;
    }
    
    setsockopt(sockListen, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(int));
    memset(&recvAddr, 0, sizeof(struct sockaddr_in));
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_port = htons(UDP_PORT);
    recvAddr.sin_addr.s_addr = INADDR_ANY;
    
    if(bind(sockListen, (struct sockaddr *)&recvAddr, sizeof(struct sockaddr)) == -1)
    {
        debug(1, "bind fail\n");
        return -1;
    }
    
    if((recvbytes = recvfrom(sockListen, recvbuf, BUFFER, 0,(struct sockaddr *)&recvAddr, &addrLen)) != -1)
    {
        recvbuf[recvbytes] = '\0';
        debug(1, "receive a broadCast messgse:%s\n", recvbuf);
        *recieve_date = recvbuf;
    }
    else
    {
        debug(1, "recvfrom fail\n");
        return -1;
    }
    
    close(sockListen);
    
    return 0;
}

int start_ap()
{
    char wifiq[64]={0};
    char wifi_ap_name[8]={0};
    
    is_station = false;
    station_mode = 0;
    srand(time(NULL));
    get_random_string(8);
    //sprintf(wifiq,"echo -e ssid=IMIO_Circle_%s >> /etc/hostapd.conf", get_random_string(8));
    if(access("/etc/config/ap_name",F_OK) == -1)
    {
        sprintf(wifi_ap_name, "%06s", get_random_string(7));
        //sprintf(wifi_ap_name, "%06d", rand()%10000);
        sprintf(wifiq,"echo -e ssid=IMIO_Circle_%s >> /etc/hostapd.conf", wifi_ap_name);
        write_wifi(wifi_ap_name);
    }
    else
    {
        char *read_ap_name = read_wifi();
        sprintf(wifiq,"echo -e ssid=IMIO_Circle_%s >> /etc/hostapd.conf", read_ap_name);
    }
    system(wifiq);
    system("start_ap");
    
    return 0;
}

int start_station()
{
    azplay(mode_3, "/home/wifi_connecting.mp3");
    is_station = true;
    system("start_station");
    
    return 0;
}

int network_ok(int status)
{
    debug(1, "Network is OK!\n");
    is_wireless_ready = true;
    is_station = false;
    wifi_status = WIFI_CONNECTED;
    
    if(NETWORK_START == status)
    {
        led_driver(RE_ORANGE); //re_green
    }
    else
    {
        led_driver(BLUE_SLOW_TURN); //
    }
    
    azplay(mode_3, "/home/wifi_connected.mp3");
    azplay(mode_3, "/home/miokey_is_ready.mp3");
    //device online right now
    //is_device_online = true;
    //is_heartbeat_normal = false;
    sleep(1);
    led_driver(RE_BLUE); //OFF_ON_LED
    is_connected = true;
    
    return 0;
}

int network_error()
{
    debug(1, "Network is error!\n");
    wifi_status = WIFI_CONNECTFAIL;
    led_driver(ALL_LEDS_RED); //red
    azplay(mode_2, "/home/wifi_connect_fail.mp3");
}

int check_wifi()
{
    int ret;
    char buf[32] = {0};
    
    FILE *wf_p = popen("cat /etc/wifi/wpa_supplicant.conf | grep ssid","r");
    
    if(fgets(buf, 32, wf_p) != NULL)
    {
        start_station();
        ret = 0;
    }
    else
    {
        ret = -1;
    }
    
    return ret;
}

int network_connect(int status)
{
    int ret = -1;
    
    debug(1, "Begin network connect() \n");
    wifi_status == WIFI_CONNECTING;
    led_driver(OFF_ON_LED);
    
    if(is_ap_connect)
    {
        is_ap_connect = false;
    }
    else
    {
        if(0 == check_wifi())
        {
            int i = 0;
            led_driver(RE_ORANGE); //re_orange
            for(i = 0; i < 3; i++)
            {
                ret = wifi_connect_status();
                
                if(0 == ret)
                {
                    if(NETWORK_START == status)
                    {
                        network_ok(status);
                    }
                    return 0;
                }
                return ret;
            }
        }
        else
        {
            return ret;
        }
    }
    
    if(0 == ret)
    {
        if(NETWORK_START == status)
        {
            network_ok(status);
        }
        return 0;
    }
    else
    {
        network_error();
    }
    
    return ret;
}

int network_ap()
{
    char send_buff[128];
    char *recieve_date;
    cJSON *root;
    cJSON *ssid;
    cJSON *key;
    cJSON *user_id;
    
    //3. Get UDP info
    debug(1, "AP Mode start...\n");
    is_ap_mode = true;
    is_httpclient_ok = false;
    led_driver(ALL_LEDS_ORANGE); //orange light 
    start_ap();
    azplay(mode_3, "/home/ap_mode.mp3");
    
    debug(1, "Waiting for Recieve Date...\n");
    client_udp(&recieve_date);
    
    if(NULL != recieve_date)
    {
        if(cJSON_Parse(recieve_date))
        {
            root = cJSON_Parse(recieve_date);
            if(cJSON_GetObjectItem(root, "ssid"))
            {
                ssid = cJSON_GetObjectItem(root, "ssid");
                if(cJSON_GetObjectItem(root, "key"))
                {
                    key = cJSON_GetObjectItem(root, "key");
                    char ssid_str[128];
                    sprintf(ssid_str, "\"%s\"", ssid->valuestring);
                    char passwd_str[128];
                    sprintf(passwd_str, "\"%s\"", key->valuestring);
                    if(cJSON_GetObjectItem(root, "user_id"))
                    {
                        user_id = cJSON_GetObjectItem(root, "user_id");
                        char user_id_str[256];
                        sprintf(user_id_str, "%s", user_id->valuestring);
                        debug(1, "user_id : %s\n", user_id_str);
                        if(access("/etc/config/user_id", F_OK) == -1)
                        {
                            write_userid(user_id_str);
                        }
                        else
                        {
                            char *user_string = read_user_id();
                            if(user_string != user_id_str)
                            {
                                debug(1, "rm /etc/config/dev_id");
                                system("rm /etc/config/dev_id");
                                write_userid(user_id_str);
                            }
                        }
                    }
                    led_driver(RE_ORANGE);
                    debug(1, "ssid : %s\n", ssid_str);
                    debug(1, "key : %s\n", passwd_str);
                    udp_server(SEND_SUCCESS);
                    
                    char buf[512] = {0};
                    char commset[] = {"ctrl_interface=/var/run/wpa_supplicant\nupdate_config=1\neapol_version=1\nfast_reauth=1\n"};
                    char WPAstr[] = {"ap_scan=1\nnetwork={\n\tssid=%s\n\tscan_ssid=1\n\tpsk=%s\n}\n"};
                    sprintf(buf, WPAstr, ssid_str, passwd_str);
                    system("rm /etc/wifi/wpa_supplicant.conf");
                    debug(1, "%s", buf);
                    
                    FILE *fp;
                    if((fp = fopen("/etc/wifi/wpa_supplicant.conf", "w+"))==NULL)
                    {
                        printf("open file failed %s\n", strerror(errno));
                        exit(1);
                    }
                    fprintf(fp,"%s", commset);
                    fprintf(fp,"%s", buf);
                    fclose(fp);
                    
                    //sleep(1);
                    udp_server(SEND_SUCCESS);
                    network_connect(NETWORK_START);
                }
                else
                {
                    udp_server(FAIL);
                    //network_error();
                    debug(1, "JSON key is error! \n");
                }
            }
            else
            {
                udp_server(FAIL);
                //network_error();
                debug(1, "JSON ssid is error! \n");
            }
        }
        else
        {
            udp_server(FAIL);
            //network_error();
            debug(1, "JSON format is error! \n");
        }
    }
    else
    {
        udp_server(FAIL);
        //network_error();
        debug(1, "Date format is error! \n");
    }
    
    debug(1, "STA Mode start...\n");
    is_ap_mode = false;
    ap_mode = 0;
    
    return 0;
}


void *thread_udp_main(void *p)
{
    while(1)
    {
        if((!is_wireless_ready) && (!is_ap_mode))
        {
            debug(1, "thread_udp_main orange\n");
            led_driver(ALL_LEDS_ORANGE); //
            sleep(6);//60
            debug(1, "thread_udp_main off\n");
            led_driver(OFF_ON_LED); //
            sleep(60);//600
            //azplay(mode_3, "/home/mp3/wifi_offline.mp3");
        }
    }
    
    pthread_exit(0);
}

//Listening the wifi status
void *thread_wifi_main(void *p)
{
    int ret, ret_wifi, i, net_num = 0;
    pthread_t id_udp;
    
    //Start new pthread
    sleep(5);
    debug(1, "thread_wifi_main is start() \n");
    //pthread_create(&id_udp, NULL, (void *)thread_udp_main, NULL);
    
    while(in_aborting)
    {
        if(is_wireless_ready)
        {
            sleep(60);
            
            debug(1, "thread_wifi_main is runing!\n");
            if(!is_ap_mode)
            {
                for(i = 0; i < 5; i++)
                {
                    ret = wifi_connect_status();
                    if(0 == ret)
                    {
                        debug(1, "Network is ok!\n");
                    }
                    else
                    {
                        net_num++;
                    }
                }
                debug(1, "net_num = %d\n", net_num);
                if(net_num >= 4)
                {
                    //Miokey lost the network connect
                    ret_wifi = network_connect(NETWORK_CONNECT);
                    if(0 == ret_wifi)
                    {
                        led_driver(RE_GREEN); //green run
                        sleep(2);
                        led_driver(OFF_ON_LED); //
                    }
                    else
                    {
                        led_driver(RE_RED); //red run
                        sleep(2);
                        led_driver(OFF_ON_LED); //
                    }
                }
                else
                {
                    led_driver(OFF_ON_LED); //
                }
                net_num = 0;
            }
        }
        else
        {
            sleep(1);
        }
    }
    
    pthread_exit(0);
}


void *wifi_pthread(void *p)
{
    int ret_net;
    
    ret_net = network_connect(NETWORK_START);
    
    if(0 == ret_net)
    {
        debug(1, "Wifi connected !\n");
    }
    else
    {
        wifi_status = WIFI_CONNECTFAIL;
    }
    
    pthread_exit(0);
}

void *ap_pthread(void *p)
{
    network_ap();//First time to setup
    
    pthread_exit(0);
}

void *thread_key_main(void *p)
{
    key_driver();
    
    pthread_exit(0);
}

int wifi_main()
{
    int led_count = 0;
    int led_speak = 0;
    int fd_rds = 1;
    fd_set rds;
    struct timeval del_time;	
    pthread_t id_key, id_wireless;
    
    //1. Listening wifi status
    pthread_create(&id_key, NULL, (void *)thread_key_main, NULL);
    pthread_create(&id_wireless, NULL, (void *)thread_wifi_main, NULL);
    
    sleep(2);
    
    while(in_aborting)
    {
        FD_ZERO(&rds);
        FD_SET(fd_rds, &rds);
        del_time.tv_sec = 1;
        del_time.tv_usec = 0;
        int ret = select(fd_rds + 1, &rds, NULL, NULL, &del_time);
        
        if(ret <= 1)
        {
            debug(1, "wfcount = %d\n", wfcount);
            
            if(ret >= 1)
            {
                debug(1, "ret = %d\n", ret);
                sleep(1);
            }
            
            if(wfcount == 4)
            {
                led_driver(OFF_ON_LED);
            }
            
            if(wifi_status == WIFI_DISCONNECTED)
            {
                //2. Get wifi status
                wifi_status = WIFI_CONNECTING;
                wifi_count = 5;
                pthread_t th_wifi;
                pthread_create(&th_wifi, NULL, wifi_pthread, NULL);
            }
            
            if(wifi_status == WIFI_CONNECTFAIL)
            {
                wifi_count++;
                debug(1, "wifi_count = %d", wifi_count);
                if(wifi_count >= 5)
                {
                    wifi_count = 0;
                    wifi_status = WIFI_AP_MODE;
                    pthread_t ap_wifi;
                    pthread_create(&ap_wifi, NULL, ap_pthread, NULL);
                }
            }
            
            if(is_volume_led)
            {
                led_count++;
                if(led_count > 4)
                {
                    led_count = 0;
                    is_volume_led = false;
                    led_driver(OFF_ON_LED);
                }
            }
            
            if(is_led_speak)
            {
                led_speak++;
                if(led_speak > 5)
                {
                    led_speak = 0;
                    is_led_speak = false;
                    led_driver(OFF_ON_LED);
                }
            }
            
            if(is_station)
            {
                station_mode++;
                debug(1, "station_mode = %d\n", station_mode);
                if(station_mode >= 30)
                {
                    station_mode = 0;
                    is_station = false;
                    system("killall -q wpa_supplicant udhcpc udhcpd start_station");
                    debug(1, "kill start_station \n");
                    wifi_status = WIFI_CONNECTFAIL;
                    network_error();
                }
            }
            
            if(is_connected)
            {
                connected_count++;
                if(connected_count >= 3)
                {
                    connected_count = 0;
                    is_connected = false;
                    led_driver(OFF_ON_LED);
                }
            }
            
            if(is_ap_mode)
            {
                ap_mode++;
                if(ap_mode >= 300)
                {
                    ap_mode = 0;
                    led_driver(OFF_ON_LED);
                }
                else
                {
                    led_driver(ALL_LEDS_ORANGE);
                }
            }
            
            if(is_mute)
            {
                led_driver(ALL_LEDS_RED);//RED 
            }
            
            if(is_reply)
            {
                reply_count++;
                if(reply_count >= 3)
                {
                    led_driver(BLUE_SLOW_TURN);
                }
                else if(reply_count >= 4)
                {
                    reply_count = 0;
                    led_driver(ALL_LEDS_BLUE);
                }
            }
            
            if(is_device_online)
            {
                online_count++;
                if(online_count >= 4)
                {
                    online_count = 0;
                    led_driver(OFF_ON_LED);
                }
            }
            
            if(wfcount < 0)
                wfcount = 0;
            wfcount ++;
            
            continue;
        }
        else
        {
            debug(1, "ret = %d\n", ret);
            sleep(1);
        }
    }
    
    return 0;
}
