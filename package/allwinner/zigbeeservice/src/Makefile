#CC = arm-openwrt-linux-gcc
#CC = arm-openwrt-linux-muslgnueabi-gcc

MY_LIBS = -L./ -lshuncomzigbee

MY_INCLUDES = -I$(CURDIR)/include/ -I$(CURDIR)/ -I./alsa/install/include 

DEFS = -I$(TOPDIR)/out/astar-parrot/staging_dir/target/usr/include

TINCLUDES = 

CFLAGS = $(DEFS) $(TINCLUDES)

DEFS_LIB = -L$(TOPDIR)/out/astar-parrot/staging_dir/target/usr/lib

TLIB = -L./alsa/install/lib -L./lib 

MY_CFLAGS += $(DEFS_LIB) $(TLIB) -ladplayer -lsalsa -laozhou -lasound -lcae -lcae -lmsc  -lbsa -pthread -lmosquitto -lcares -lcrypto -lcyassl -lpolarssl -lssl -lnghttp2 -lcurl -lpthread -lubus -lubox -llua -lm  -luci -ljson-c -lsqlite3 -lblobmsg_json -ldl ${CFLAGS} -fPIC ${MY_INCLUDES} -g 


#=================================================================================
#								Main entrance
#=================================================================================
APP_NAME = zigbeeService

all: $(APP_NAME)


APP_OBJECTS =  zigbeeservice.o ubus.o zha_strategy.o cJSON.o httpclient.o mqtt_cloud.o  zigbee_device.o wireless.o device_json.o cae_lib.o cap.o driver_led_key.o pcm_mixer.o 

$(APP_OBJECTS) : %.o:%.c
	$(CC) -c $(MY_CFLAGS) $(MY_INCLUDES) $(DEFS) $< -o $@ 


$(APP_NAME): $(APP_OBJECTS) 
	$(CC) -o $(APP_NAME)  $(MY_CFLAGS) *.o $(MY_LIBS)


clean:
	rm $(APP_NAME) *.o

.PHONY: clean

