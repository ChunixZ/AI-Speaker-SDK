/*************************************************************************
    > File Name: cap.h
    > Author: zyc
    > Mail: zyc@imio.ai 
    > Created Time: 2017/12/12
*************************************************************************/

#ifndef __CAP_H__


#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <alsa/asoundlib.h>

#include <ad_type.h>
#include <factory_api.h>
#include <common_def.h>

#include "aozhou.h"
#include "cJSON.h"
#include "cae_intf.h"
#include "cae_lib.h"
#include "cae_errors.h"
#include "formats.h"
#include "gettext.h"
#include "msp_cmn.h"
#include "msp_errors.h"
#include "qisr.h"


#define    disable    0
#define    enable      1


enum  play_sta{
    mode_0 = 0,    //初始状态
    mode_1   ,       //唤醒状态
    mode_2   ,      //提示状态
    mode_3   ,      //播放状态
    mode_4   ,
    mode_5   ,
};


extern void adplayer_set_file_context(s_file_context *fc);
extern void adplayer_get_file_context(s_file_context *fc);
extern int adplayer(int , void *);
extern int adplayer_init(void);
extern int  ctrl_getstate(void);
extern void ctrl_setstate(int);
extern void adstop();
//extern void adplayer_exit(void);
extern int key_wakeup();
extern int adplay(void *, int );
extern void azplay(int , char *);

extern int device_ai_speaker(char *);
//extern int cap_main();
extern int zigbee_main();


#endif
