#ifndef _DATA_STRUCTURE_H_
#define _DATA_STRUCTURE_H_

//#include "g_para.h"
#include <sys/time.h>
#include <inttypes.h>

enum {
    STREAM_MUSIC = 0,
    STREAM_TONE,
    STREAM_3,
    STREAM_4,    
    STREAM_5,
    MAX_STREAM_NUM
};

typedef struct list_packet{
    unsigned char *data;    //解码后的数据
    int size;               //解码后的数据长度
    int p_time;             //播放时间
#ifdef __MULTI_ROOM__    
    int num;                //同一秒内包序号
#endif    
	int dec;                //解码标志位
    struct list_packet *next;
}S_LIST_PACKET,*P_LIST_PACKET;

#define AVCODEC_MAX_AUDIO_FRAME_SIZE 16*1024

typedef struct linein_packet{
    unsigned char data[AVCODEC_MAX_AUDIO_FRAME_SIZE];//解码后的数据
	int dec;
    struct linein_packet *next;
}S_LINEIN_PACKET,*P_LINEIN_PACKET;

typedef struct buff_stream{
    char *buff;
	int size;
}S_BUFF_STREAM,*P_BUFF_STREAM;

extern void set_head_list(P_LIST_PACKET list, int stream);
extern P_LIST_PACKET get_head_list(int stream);
extern P_LIST_PACKET create_packet_list(int n);
extern void delete_packet_list(int stream);
extern void clear_packet_list(int stream);
extern void add_packet_data(P_LIST_PACKET tmp_pk, unsigned char *frame_data, int data_size, int time);
extern void delete_packet_data(P_LIST_PACKET tmp_pk);
extern void print_packet_dec(int stream);

#endif
