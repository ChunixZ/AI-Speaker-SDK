#CC = arm-openwrt-linux-gcc
CC = arm-openwrt-linux-muslgnueabi-gcc

DEFS = -I$(TOPDIR)/out/astar-parrot/staging_dir/target/usr/include

#=================================================================================
#								Main entrance
#=================================================================================
APP_NAME = socket_get_currency

all: $(APP_NAME)

APP_OBJECTS = socket_get_currency.o 

$(APP_OBJECTS) : %.o:%.c
	$(CC) -c $(DEFS) $< -o $@ 


$(APP_NAME): $(APP_OBJECTS) 
	$(CC) -o $(APP_NAME) *.o 


clean:
	rm $(APP_NAME) *.o

.PHONY: clean

