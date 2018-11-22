#include "include/mqtt_cloud.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "mosquitto.h"
#include "include/device_json.h"

#define IMIO_CRT "/etc/config/imio"


//imio.crt
void write_imio(char *imio_crt)
{
    FILE *fp;
    
    fp = fopen(IMIO_CRT, "w+");
    fwrite(imio_crt, strlen(imio_crt), 1, fp);
    fclose(fp);
}

char* read_imio()
{
    static char buffer[2048];
    FILE *fp;
    
    if((fp = fopen(IMIO_CRT, "r")) != NULL)
    {
        memset(buffer, 0, 2048);
        fread(buffer, 1, 2048, fp);
        debug(1, "%s", buffer);
    }
    fclose(fp);
    
    return buffer;
}


/*
void my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    int i;
    if(!result)
    {
        // Subscribe to broker information topics on successful connect. $SYS/#
        mosquitto_subscribe(mosq, NULL, TOPIC, Q0S2);
    }
    else
    {
        fprintf(stderr, "Connect failed\n");
    }
}
*/

void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    if(message->payloadlen)
    {
        printf("----------------------------------------------\n");
        printf("%s\n %s\n", (char *)message->topic, (char *)message->payload);
        printf("----------------------------------------------\n");
        /**Note: Receive the callback message
        **Call device_json to resolve
        **/
        device_cjson((char *)message->payload);
    }
    else
    {
        printf("%s (null)\n", message->topic);
    }
    fflush(stdout);
}

void my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
    int i;

    printf("Subscribed (mid: %d): %d", mid, granted_qos[Q0S2]);
    for(i = 1; i < qos_count; i++)
    {
        printf(", %d", granted_qos[i]);
    }
    printf("\n");
}


struct mosquitto *mosq = NULL;

int mqtt_sub(char *connect_main, char *channel_no)
{
    bool clean_session = true;
    
    mosquitto_lib_init();
    mosq = mosquitto_new(NULL, clean_session, NULL);
    if(!mosq)
    {
        fprintf(stderr, "Error: Out of memory.\n");
    }
    
    mosquitto_username_pw_set(mosq, "", "");
    //mosquitto_connect_callback_set(mosq, my_connect_callback);
    mosquitto_message_callback_set(mosq, my_message_callback);
    mosquitto_subscribe_callback_set(mosq, my_subscribe_callback);

    if(mosquitto_connect(mosq, connect_main, PORT, KEEPALIVE))
    {
        fprintf(stderr, "Unable to connect.\n");
    }
    
    mosquitto_subscribe(mosq, NULL, channel_no, Q0S2);
    printf("mosquitto_subscribe finish\n");
    
    mosquitto_loop_forever(mosq, -1, 1);

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    
    return 0;
}

int mqtt_pub(char *channel_no, char *pub_str)
{
    mosquitto_publish(mosq, NULL, channel_no, strlen(pub_str), pub_str, Q0S2, true);
    
    return 0;
}

int mqtt_ssl_sub(char *connect_main, char *channel_no)
{
    int rc;
    bool clean_session = true;
    
    mosquitto_lib_init();
    mosq = mosquitto_new(NULL, clean_session, NULL);//String to use as the client id.  
    if(!mosq)
    {
        fprintf(stderr, "Error: Out of memory.\n");
    }
    
    write_imio(CERTIFICATE);
    mosquitto_username_pw_set(mosq, "", "");
    mosquitto_tls_set(mosq, IMIO_CRT, NULL, NULL, NULL, NULL);
    mosquitto_tls_insecure_set(mosq, 1);
    mosquitto_tls_opts_set(mosq, 1, NULL, NULL);
    
    rc = mosquitto_connect(mosq, connect_main, PORT_SSL, KEEPALIVE);
    printf("connect returned %d\n", rc);
    
    //mosquitto_connect_callback_set(mosq, my_connect_callback);
    mosquitto_message_callback_set(mosq, my_message_callback);
    mosquitto_subscribe_callback_set(mosq, my_subscribe_callback);
    
    mosquitto_subscribe(mosq, NULL, channel_no, Q0S2);
    write_imio("");
    printf("mosquitto_subscribe finish\n");
    mosquitto_loop_forever(mosq, -1, 1);
    
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    
    return rc;
}

int mqtt_ssl_pub(char *channel_no, char *pub_str)
{
    printf("----------------------------------------------------------------------------\n");
    mosquitto_publish(mosq, NULL, channel_no, strlen(pub_str), pub_str, Q0S2, true);
    
    return 0;
}

int mqtt_cleanup()
{
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    
    return 0;
}

