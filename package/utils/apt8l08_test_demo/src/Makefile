BUILDPATH=.

CFLAGS=-Wall
CFLAGS+=-I./
CFLAGS+=-O0 -g
LOCAL_SRC = apt8l08_test.c 

all:
	$(CC) $(CFLAGS) $(EXT_CFLAGS)  $(LOCAL_SRC) $(LDFLAGS)  -o apt8l08_test_demo 
clean:
	rm -f $(LIB) $(LIBOBJ)
