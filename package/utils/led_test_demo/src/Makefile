BUILDPATH=.

CFLAGS=-Wall
CFLAGS+=-I./
CFLAGS+=-O0 -g
LOCAL_SRC = test_led_demo.c 

all:
	$(CC) $(CFLAGS) $(EXT_CFLAGS)  $(LOCAL_SRC) $(LDFLAGS)  -o test_led_demo 
clean:
	rm -f $(LIB) $(LIBOBJ)
