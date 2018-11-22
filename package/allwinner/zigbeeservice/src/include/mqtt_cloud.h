#ifndef _MQTT_CLOUD_H_
#define _MQTT_CLOUD_H_

#include <stdbool.h>


#define  HOST  ""
#define  PORT  1883
#define  PORT_SSL  8883
#define  KEEPALIVE  60
#define  CLEAN_SESSION  true
#define  Q0S2   2


#define    CERTIFICATE    ""


extern int mqtt_sub(char *, char *);
extern int mqtt_pub(char *, char *);
extern int mqtt_ssl_sub(char *, char *);
extern int mqtt_ssl_pub(char *, char *);
extern int mqtt_cleanup();


#endif
