/*************************************************************************
    > File Name: cap.c
    > Author: zyc
    > Mail: zyc@imio.ai 
    > Created Time: 2017/12/12
*************************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include "cap.h"
#include "wireless.h"
#include "httpclient.h"
#include "device_json.h"
#include "driver_led_key.h"


#define WAV_FILE       "/tmp/baidu_tts.mp3"


volatile sig_atomic_t in_aborting = enable;
static u_char *audiobuf = NULL;
const u_char *audio_buf;
static snd_pcm_uframes_t chunk_size;
static snd_pcm_uframes_t period_frames = 0, buffer_frames = 0;
static int start_delay = 1, stop_delay = 0;
static size_t bits_per_sample, bits_per_frame, chunk_bytes;
static snd_output_t *log;
static unsigned period_time = 0, buffer_time = 0;
static int waitsig = disable;
static int waitcmd = disable;
unsigned int writelen;
volatile static int recycle_capture_file = enable;
static snd_pcm_stream_t stream = SND_PCM_STREAM_CAPTURE;

char play_buf[1024];
s_file_context  r_play;
static int play_save_s = disable;
static int play_time, play_status = mode_0, play_mode = mode_0, save_status, player = disable;
extern int set_dec_para, av_opened, play_over, decode_over;

pthread_cond_t condwakeup, condplay_az;
pthread_mutex_t mutexwakeup, mutexplay_az;

//zyc
#define    Version          "01.00.171221_r01"

bool is_led_speak = false;
bool is_recognize = false;
bool is_baidu_tts = false;
int build_relese = BUILD_TEST;
extern bool is_wireless_ready;
extern bool is_ap_mode;
extern bool is_mute;
extern bool is_reply;
extern bool is_people;
extern bool is_bind_speech;
extern bool is_device_data_download;
extern unsigned int wfcount;

//end zyc


static snd_pcm_t *handleaz;

static struct {
    snd_pcm_format_t format;
    unsigned int channels;
    unsigned int rate;
} hwparams, rhwparams;

typedef struct _UserData {
    int     build_fini; //标识语法构建是否完成
    int     update_fini; //标识更新词典是否完成
    int     errcode; //记录语法构建或更新词典回调错误码
    char    grammar_id[MAX_GRAMMARID_LEN]; //保存语法构建返回的语法ID
} UserData;

typedef struct _CAEUserData{
    FILE *fp_out;
} CAEUserData;

CAE_HANDLE cae = NULL;
static Proc_CAENew              api_cae_new;
static Proc_CAEAudioWrite   api_cae_audio_write;
static Proc_CAEResetEng       api_cae_reset_eng;
static Proc_CAESetRealBeam  api_cae_set_real_beam;
static Proc_CAESetWParam   api_cae_set_wparam;
static Proc_CAEGetWParam   api_cae_get_wparam;
static Proc_CAEGetVersion    api_cae_get_version;
static Proc_CAEGetChannel    api_cae_get_channel;
static Proc_CAESetShowLog  api_cae_set_show_log;
static Proc_CAEDestroy        api_cae_destroy;
//end 科大定义



static void CAEIvwCb(short angle, short channel, float power, short CMScore,\
        short beam, char *param1, void *param2, void *userData)
{
    if(!is_mute)
    {
        waitsig = disable;
        
        //is_wireless_ready = true;
        if(is_wireless_ready)
        {
            if(is_ap_mode)
            {
                led_driver(ALL_LEDS_ORANGE); //orange light 
                azplay(mode_2, "/home/ap_mode.mp3");
            }
            else
            {
                if((angle >= 0) && (angle < 30))
                {
                    led_driver(THIRD_BLUE_LED); //13  202
                }
                else if((angle >= 30) && (angle < 90))
                {
                    led_driver(FOURTH_BLUE_LED); //14
                }
                else if((angle >= 90) && (angle < 150))
                {
                    led_driver(SIXTH_BLUE_LED); //16
                }
                else if((angle >= 150) && (angle < 210))
                {
                    led_driver(NINTH_BLUE_LED); //19
                }
                else if((angle >= 210) && (angle < 270))
                {
                    led_driver(ELEVENTH_BLUE_LED); //21
                }
                else if((angle >= 270) && (angle < 330))
                {
                    led_driver(FIRST_BLUE_LED); //11
                }
                else if((angle >= 330) && (angle <= 360))
                {
                    led_driver(THIRD_BLUE_LED);//13
                }
                is_led_speak = true;
                azplay(mode_1, "/home/Iam.mp3");
                sleep(1);//end to atalk need the time
                
                usleep(1200*1000);//end to atalk need the time
                waitsig = enable;
                pthread_cond_signal(&condwakeup);
            }
        }
        else
        {
            is_led_speak = true;
            led_driver(RED_SLOWER_TURN);//13
            azplay(mode_2, "/home/wifi_offline.mp3");//iamn.mp3
            sleep(1);
        }
    }
    
    printf("---------------------\n");
    printf("\nCAEIvwCb ....\nangle:%d\n  param1:%s\n", angle, param1);
    printf("---------------------\n");
}

static void CAEAudioCb(const void *audioData, unsigned int audioLen, int param1, const void *param2, void *userData)
{
    CAEUserData *usDta = (CAEUserData*)userData;
    audio_buf = audioData;//6麦合成返回的单麦数据，通过此数据进行语音识别
    writelen = audioLen;
    waitcmd = enable;
}

static int initFuncs()
{
    int ret = MSP_SUCCESS;
    const char* libname = "/usr/lib/libcae.so";
    void* hInstance = cae_LoadLibrary(libname);

    if(hInstance == NULL)
    {
        printf("Can not open library!\n");
        return MSP_ERROR_OPEN_FILE;
    }
    
    api_cae_new = (Proc_CAENew)cae_GetProcAddress(hInstance, "CAENew");
    api_cae_audio_write = (Proc_CAEAudioWrite)cae_GetProcAddress(hInstance, "CAEAudioWrite");
    api_cae_reset_eng = (Proc_CAEResetEng)cae_GetProcAddress(hInstance, "CAEResetEng");
    api_cae_set_real_beam = (Proc_CAESetRealBeam)cae_GetProcAddress(hInstance, "CAESetRealBeam");
    api_cae_set_wparam = (Proc_CAESetWParam)cae_GetProcAddress(hInstance, "CAESetWParam");
    api_cae_get_wparam = (Proc_CAEGetWParam)cae_GetProcAddress(hInstance, "CAEGetWParam");
    api_cae_get_version = (Proc_CAEGetVersion)cae_GetProcAddress(hInstance, "CAEGetVersion");
    api_cae_get_channel= (Proc_CAEGetChannel)cae_GetProcAddress(hInstance, "CAEGetChannel");
    api_cae_destroy = (Proc_CAEDestroy)cae_GetProcAddress(hInstance, "CAEDestroy");
    api_cae_set_show_log = (Proc_CAESetShowLog)cae_GetProcAddress(hInstance, "CAESetShowLog");
    
    return ret;
}

static void prg_exit(int code) 
{
    if (handleaz)
    {
        snd_pcm_drain(handleaz);
        snd_pcm_close(handleaz);
    }
    exit(code);
}

static void set_params(void)
{
    int err;
    size_t n;
    unsigned int rate;
    snd_pcm_hw_params_t *params;
    snd_pcm_sw_params_t *swparams;
    snd_pcm_uframes_t buffer_size;
    snd_pcm_uframes_t start_threshold, stop_threshold;
    
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_sw_params_alloca(&swparams);
    err = snd_pcm_hw_params_any(handleaz, params);
    
    if (err < 0) {
        error(_("Broken configuration for this PCM: no configurations available"));
        prg_exit(EXIT_FAILURE);
    }
    err = snd_pcm_hw_params_set_access(handleaz, params,
            SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0) {
        error(_("Access type not available"));
        prg_exit(EXIT_FAILURE);
    }
    err = snd_pcm_hw_params_set_format(handleaz, params, hwparams.format);
    if (err < 0) {
        error(_("Sample format non available"));
        prg_exit(EXIT_FAILURE);
    }
    err = snd_pcm_hw_params_set_channels(handleaz, params, hwparams.channels);
    if (err < 0) {
        error(_("Channels count non available"));
        prg_exit(EXIT_FAILURE);
    }

    rate = hwparams.rate;
    err = snd_pcm_hw_params_set_rate_near(handleaz, params, &hwparams.rate, 0);
    assert(err >= 0);

    if (buffer_time == 0 && buffer_frames == 0) {
        err = snd_pcm_hw_params_get_buffer_time_max(params,
                &buffer_time, 0);
        assert(err >= 0);
        if (buffer_time > 500000)
            buffer_time = 500000;
    }
    if (period_time == 0 && period_frames == 0) {
        if (buffer_time > 0)
            period_time = buffer_time / 4;
        else
            period_frames = buffer_frames / 4;
    }
    if (period_time > 0)
        err = snd_pcm_hw_params_set_period_time_near(handleaz, params,
                &period_time, 0);
    else
        err = snd_pcm_hw_params_set_period_size_near(handleaz, params,
                &period_frames, 0);
    assert(err >= 0);
    if (buffer_time > 0) {
        err = snd_pcm_hw_params_set_buffer_time_near(handleaz, params,
                &buffer_time, 0);
    } else {
        err = snd_pcm_hw_params_set_buffer_size_near(handleaz, params,
                &buffer_frames);
    }
    assert(err >= 0);
    err = snd_pcm_hw_params(handleaz, params);
    if (err < 0) {
        error(_("Unable to install hw params:"));
        snd_pcm_hw_params_dump(params, log);
        prg_exit(EXIT_FAILURE);
    }
    snd_pcm_hw_params_get_period_size(params, &chunk_size, 0);
    snd_pcm_hw_params_get_buffer_size(params, &buffer_size);
    if (chunk_size == buffer_size) {
        error(_("Can't use period equal to buffer size (%lu == %lu)"),
                chunk_size, buffer_size);
        prg_exit(EXIT_FAILURE);
    }
    snd_pcm_sw_params_current(handleaz, swparams);
    n = chunk_size;
    err = snd_pcm_sw_params_set_avail_min(handleaz, swparams, n);

    n = buffer_size;
    start_threshold = (double) rate * start_delay / 1000000;
    if (start_threshold < 1)
        start_threshold = 1;
    if (start_threshold > n)
        start_threshold = n;
    err = snd_pcm_sw_params_set_start_threshold(handleaz, swparams, start_threshold);
    assert(err >= 0);
    stop_threshold = buffer_size + (double) rate * stop_delay / 1000000;
    err = snd_pcm_sw_params_set_stop_threshold(handleaz, swparams, stop_threshold);
    assert(err >= 0);

    if (snd_pcm_sw_params(handleaz, swparams) < 0) {
        error(_("unable to install sw params:"));
        snd_pcm_sw_params_dump(swparams, log);
        prg_exit(EXIT_FAILURE);
    }

    bits_per_sample = snd_pcm_format_physical_width(hwparams.format);//16
    bits_per_frame = bits_per_sample * hwparams.channels;//16*8=128
    chunk_bytes = chunk_size * bits_per_frame / 8;//2000*128/8=3200
    audiobuf = realloc(audiobuf, chunk_bytes);
    if (audiobuf == NULL) {
        error(_("not enough memory"));
        prg_exit(EXIT_FAILURE);
    }
    buffer_frames = buffer_size;    /* for position test */
}

static int capture(void)
{
    int err, errcount = 0;
    snd_pcm_info_t *info;
    
    snd_pcm_info_alloca(&info);
    err = snd_output_stdio_attach(&log, stderr, 0);
    assert(err >= 0);

    chunk_size = 1024;
    rhwparams.channels = 8;
    rhwparams.format = SND_PCM_FORMAT_S24_LE;
    rhwparams.rate = 16000;

    hwparams = rhwparams;
    err = snd_pcm_open(&handleaz, "hw:1,0", stream, 0);
    if (err < 0) 
    {
        error(_("audio open error: %s"), snd_strerror(err));
        goto exit;
    }
    if ((err = snd_pcm_info(handleaz, info)) < 0) 
    {
        error(_("info error: %s"), snd_strerror(err));
        return -1;
    }

    audiobuf = (u_char *)malloc(1024);
    if (audiobuf == NULL) {
        error(_("not enough memory"));
        return -1;
    }

    int ret = MSP_SUCCESS;
    const char *resPath = "fo|/home/ivw_resource.jet|0|1024";
    static CAEUserData userData;

    if(initFuncs() != MSP_SUCCESS)
    {
        printf("load cae library failed\n");
        return -1;
    }

    ret = api_cae_new(&cae, resPath, CAEIvwCb, CAEAudioCb, NULL, &userData);
    if (MSP_SUCCESS != ret)
    {
        printf("CAENew ....failed\n");
        ret = MSP_ERROR_FAIL;
        goto error;
    }

    set_params();//init 
    
    int switchcmd = enable, first_pcm = disable;
    
    while(recycle_capture_file && in_aborting)
    {
        size_t c = chunk_bytes;//64000
        u_char *data = audiobuf;
        u_char *audiop = NULL;
        ssize_t r;
        ssize_t count = chunk_size;//2000
        
        while(count > 0 && in_aborting)
        {
            r = snd_pcm_readi(handleaz, data, count);//6麦原始数据
            if (r == -EAGAIN || (r >= 0 && r < count))
            {
                snd_pcm_wait(handleaz, 100);
                error(_("read error: %s,%d"), snd_strerror(r),errcount);
                errcount ++;
            } 
            else if (r < 0)
            {
                snd_pcm_prepare(handleaz);
                error(_("read error: %s,%d"), snd_strerror(r),errcount);
                if(errcount > 4)
                    goto exit;
                errcount ++;
            }
            if (r > 0)
            {
                count -= r;
                data += r * bits_per_frame / 8;
                errcount = 0;
            }
        }
        if(r > 0 && in_aborting)
        {
            if(first_pcm)
            {
                audiop = change_buf_channel(audiobuf,&switchcmd,&c);
                api_cae_audio_write(cae, audiop, c);
            }
            else
                first_pcm = enable;
            
            audiop = NULL;
        }
    }
    
    return 0;

error:
exit:
    if(cae)
    {
        api_cae_destroy(cae);
        cae = NULL;
    }

    if(handleaz)
    {
        snd_pcm_drain(handleaz);
        snd_pcm_close(handleaz);
        handleaz = NULL ;
        free(audiobuf);
        audiobuf = NULL ;
        printf("snd pcm close handleaz\n");
    }
    snd_output_close(log);
    snd_config_update_free_global();
    
    return -1;
}

void *wake_up(void* data)
{
    sleep(1);
    while(in_aborting)
    {
        if(-1 == capture())
        {
            printf("error to the end \n");
            system("killall -q wpa_supplicant udhcpc udhcpd hostapd bsa_server");
            sleep(5);
            continue;
        }
        else
            break;
    }
    
    printf("snd_pcm_close handld\n");
    
    if(handleaz)
    {
        snd_pcm_drain(handleaz);
        snd_pcm_close(handleaz);
        handleaz = NULL;
    }
    free(audiobuf);
    audiobuf = NULL ;

__end:
    snd_output_close(log);
    snd_config_update_free_global();
    prg_exit(EXIT_SUCCESS);
    
    return EXIT_SUCCESS;
}


int run_asr(UserData *udata)
{
    const char *rec_rslt     = NULL, *bufrec=NULL;
    const char *session_id  = NULL;
    int aud_stat                 = MSP_AUDIO_SAMPLE_CONTINUE;
    int ep_status                = MSP_EP_LOOKING_FOR_SPEECH;
    int rec_status               = MSP_REC_STATUS_INCOMPLETE;
    int rss_status               = MSP_REC_STATUS_INCOMPLETE;
    int errcode                   = -1;
    int count                       = 0;
    
    //离线语法识别参数设置
    char *asr_param = "nlp_version = 3.0,scene = main,sch = 1,sub = iat,domain = fariat,\
                       language = zh_cn, accent = mandarin,aue = speex-wb;7,sample_rate = 16000,\
                       result_type = plain, result_encoding = utf8,vad_enable = 1,vad_eos = 1000";

    session_id = QISRSessionBegin(NULL, asr_param, &errcode);
    
    if (NULL == session_id)
        return 0;
    usleep(500*1000);
    printf("开始识别... \n");
    led_driver(RE_BLUE);
    
    while(waitsig)
    {
        if(waitcmd)
        {
            if(ep_status != 3)
            {
                QISRAudioWrite(session_id, (const void *)audio_buf,writelen,\
                        aud_stat,&ep_status, &rec_status);
                if(ep_status == 3)
                {
                    //printf("audio_buf: %s\n", audio_buf);
                    QISRAudioWrite(session_id, (const void *)NULL,0,\
                            MSP_AUDIO_SAMPLE_LAST, &ep_status, &rec_status);
                }
            }
            waitcmd = disable;
        }
        count++;
        if(!waitsig)
        {
            rec_rslt = NULL;
            QISRSessionEnd(session_id, NULL);
            return 0;
        } 
        usleep(10*1000);//must
#if 1
        if(ep_status == 3)
            rec_rslt = QISRGetResult(session_id, &rss_status, 0, &errcode);
        //printf("ep_status=%d\n",ep_status);
        if(rec_rslt)
        {
            printf("rec_rslt = %s\n",rec_rslt);
            bufrec = rec_rslt;
            break;
        }
#endif
        if(count > 600)
        {
            break;
        }
    }
    
    //led_driver(OFF_ON_LED);
    if(rec_rslt)
    {
        cJSON *root_data;
        cJSON *text_data;
        const char *buf_rec;
        
        if(cJSON_Parse(rec_rslt))
        {
            root_data = cJSON_Parse(rec_rslt);
            if(cJSON_GetObjectItem(root_data, "text"))
            {
                text_data = cJSON_GetObjectItem(root_data, "text");
                debug(1, "text : %d %s\n", sizeof(text_data->valuestring), text_data->valuestring);
                buf_rec = text_data->valuestring;
                debug(1, "%s\n", buf_rec);
                ai_service_req(text_data->valuestring);
            }
        }
        else
        {
            printf("JSON format is error!\n");
        }
    }
    else if(waitsig)
    {
        azplay(mode_2, "/home/nounder.mp3");
    }
    
    rec_rslt = NULL;
    bufrec = NULL;
    QISRSessionEnd(session_id, NULL);

    return 0;
}

void *a_talk(void*data)
{
    sleep(1);
    const char *login_config = "appid = "; //登录参数
    UserData asr_data;
    int ret = 0;
    char c;
    
    ret = MSPLogin(NULL, NULL, login_config); 
    //第一个参数为用户名，第二个参数为密码，传NULL即可，第三个参数是登录参数
    if (MSP_SUCCESS != ret) 
    {
        printf("登录失败：%d\n", ret);
        goto exit;
    }
    
    azplay(mode_1, "/home/kaiji.mp3");
    
    while(in_aborting)
    {
        printf("atalk is ready\n");
        pthread_cond_wait(&condwakeup, &mutexwakeup);
        ret = run_asr(&asr_data);
    }
    
    pthread_exit(NULL);
    
exit:
    MSPLogout();
    printf("请按任意键退出...\n");
    getchar();
    
    return 0;
}


void azplay(int i_mode, char *path)
{
    if(ctrl_getstate() == IS_BUFFERING)
    {
        decode_over = 1;
        ctrl_setstate(IS_STOP);
    }
    
    if(ctrl_getstate() == IS_DOWNLOAD)
    {
        adstop();
    }
    
    if(play_status)
    {
        save_status = i_mode ;
        if(path)
        {
            memset(play_buf, 0, 1024);
            strcpy(play_buf, path);
        }
        else
        {
            memset(play_buf, 0, 1024);
        }
        if(is_people)
        {
            is_people = false;
            //sleep(5);
        }
        pthread_cond_signal(&condplay_az);
    }
}

void *zigbee_device_ai(void *speak_str)
{
    char *speak_string = (char *)speak_str;
    printf("--------> speech: %s\n", speak_string);
    //text_to_speech(speak_string, "/tmp/imio_ai.mp3");
    baidu_wav(speak_string, 0, 4, 12, 5);
    while(is_baidu_tts);
    debug(1, "is_baidu_tts = %d", is_baidu_tts);
    
    if(is_bind_speech)
    {
        is_bind_speech = false;
        azplay(mode_1, "/tmp/imio_ai.mp3");
    }
    else
    {
        azplay(mode_3, "/tmp/imio_ai.mp3");
    }
}

int device_ai_speaker(char *speak_string)
{
    pthread_t th_device_ai;
    pthread_create(&th_device_ai, NULL, (void *)zigbee_device_ai, speak_string);
    
    return 0;
}

void save_play_status(void)
{
    play_save_s = enable;
    adplayer_get_file_context(&r_play);
    play_time = r_play.postion;
}

void get_play_status(void)
{
    if(play_save_s)
    {
        r_play.seek_time = play_time;
        adplayer_set_file_context(&r_play);
        memset(play_buf, 0, 1024);
        strcpy(play_buf, r_play.url);
        play_save_s = disable;
    }
}

void end_play(void)
{
    if(play_save_s)
    {
        printf("play_save_s is enable\n");
        if(play_mode == mode_2)
        {
            play_mode = mode_3;
            get_play_status();
        }
        else
        {
            player = disable;
            play_mode = mode_0;
        }
    }
    else
    {
        player = disable;
        play_mode = mode_0;
    }
    
    is_reply = false;
    if(wfcount >= 4)
    {
        led_driver(OFF_ON_LED);
    }
}

void* play_to_signal(void *p)
{
    int time_out;
    
    debug(1, "--------play_to_signal--------");
    while(in_aborting)
    {
        pthread_cond_wait(&condplay_az, &mutexplay_az);
        time_out = 0;
        debug(1, "start play to signal \n");
        
        if(ctrl_getstate() == IS_PLAY)
        {
            if(play_mode == mode_3)
                save_play_status();
            adstop();
            play_mode = save_status;
            player = enable;
        }
        else if(ctrl_getstate() == IS_DOWNLOAD)
        {
            if( set_dec_para)
            {
                if(play_mode == mode_3)
                    save_play_status();
                adstop();
                play_mode = save_status;
                player = enable;
            } 
            else if(!av_opened)
            {
                if(play_mode == mode_3)
                {
                    while(!av_opened)
                    {
                        if(time_out > 3000)
                        {
                            adstop();
                            break;
                        }
                        if(time_out > 2000)
                            waitsig = disable;
                        usleep(1000);
                        time_out ++;
                    }
                }
                if(av_opened)
                {
                    if(play_mode == mode_3)
                        save_play_status();
                    adstop();
                    play_mode = save_status;
                    player = enable;
                }
                else
                {
                    if(play_mode == mode_3)
                    {
                        adstop();
                        play_save_s = enable;
                    }
                    else
                        adstop();
#if  0 
                    adplayer_exit();
                
                    if(adplayer_init())
                    {
                        play_status = disable;
                        printf("init player fail\n");
                    }
                    else
                        play_status = enable;
#endif
                    waitsig = disable;
                }
            }
        }

        else
        {
        //adstop();
            play_mode = save_status;
            player = enable;
        }

        debug(1, "play_mode = %d, play_status = %d\n",play_mode,play_status);
        is_reply = false;
    }
    
    pthread_exit(0);
}

int init_mp3()
{
    sleep(10);
/*
    text_to_speech("进入网络设置模式，请配合app进行网络设置。","/home/mp3/ap_mode.mp3");
    text_to_speech("网络连接中。","/home/mp3/wifi_connecting.mp3");
    text_to_speech("网络连接成功。","/home/mp3/wifi_connected.mp3");
    text_to_speech("妙琦已经准备就绪，试着说，你好妙琦，唤醒我。","/home/mp3/miokey_is_ready.mp3");
    text_to_speech("网络连接失败，请重试。","/home/mp3/wifi_connect_fail.mp3");
    text_to_speech("网络连接超时。","/home/mp3/wifi_connect_timeout.mp3");
    
*/
    text_to_speech("妙棋现在处于离线状态，需前往app 联网设置解决。","/home/mp3/wifi_offline.mp3");
    
    return 0;
}

void *zigbee_th(void *p)
{
    zigbee_main();
    
    //init_mp3();
    
    pthread_exit(0);
}

int key_wakeup()
{
    waitsig = disable;
    
    if(!is_mute)
    {
        if(is_wireless_ready)
        {
            led_driver(VOL_TEN_STEP);
            is_led_speak = true;
            azplay(mode_1, "/home/Iam.mp3");
            sleep(1);//end to atalk need the time
            waitsig = enable;
            pthread_cond_signal(&condwakeup);
        }
        else
        {
            azplay(mode_2, "/home/wifi_offline.mp3");//iamn.mp3
        }
    }
}

void signal_handler(int sig)
{
    in_aborting = disable;
    recycle_capture_file = disable;
    
    fprintf(stderr, _("Aborted by signal %s...\n"), strsignal(sig));
    
    if(handleaz)
    {
        snd_pcm_drain(handleaz);
        snd_pcm_close(handleaz);
        handleaz = NULL;
    }
    
    signal(sig, SIG_DFL);
    usleep(500*1000);
    system("killall -q  udhcpc  udhcpd wpa_supplicant hostapd ");
}

int cap_init()
{
    printf("Start cap...\n");
    //0. fushikang 
    /*
    */
    system("insmod /home/apt8l08.ko");
    system("insmod /home/led_dev.ko");
    system("insmod /home/led_drv.ko");
    
    led_driver(MUL_COLOR_RE);
    init_volume();
    
    if(adplayer_init())
    {
        play_status = disable;
        printf("init adplayer error \n");
    }
    else
    {
        play_status = enable;
        player = enable;
        adstop();
    }
    
    //get version
    if(access("/etc/config/version", F_OK) == -1)
    {
        is_device_data_download = true;
        debug(1, "Version : %s", Version);
        write_version(Version);
    }
    else
    {
        char *version_read = read_version();
        if(strncmp(version_read, Version, sizeof(Version)))
        {
            write_version(Version);
        }
        else
        {
            debug(1, "Version : %s %s", version_read, Version);
        }
    }
    
    return 0;
}

int main(int argc,char **argv)
{
    cap_init();
    
    /*
    if(argc >= 2)
    {
        build_relese = atoi(argv[1]);
    }
    debug(1, "build_relese = %d", build_relese);
    */
    
    pthread_t th_zigbee, th_wakeup, th_atalk, th_play_signal;
    
    pthread_cond_init(&condwakeup, NULL);
    pthread_mutex_init(&mutexwakeup, NULL);
    pthread_cond_init(&condplay_az,NULL);
    pthread_mutex_init(&mutexplay_az, NULL);
    
    pthread_create(&th_zigbee,NULL,(void *)zigbee_th, NULL);//zigbee线程 
    pthread_create(&th_wakeup, NULL, wake_up, NULL);//唤醒线程
    pthread_create(&th_atalk, NULL, a_talk, NULL);//识别线程
    usleep(100 * 1000);
    pthread_create(&th_play_signal, NULL, play_to_signal, NULL);
    usleep(100 * 1000);
    
    pthread_detach(th_zigbee);
    pthread_detach(th_wakeup);
    pthread_detach(th_atalk);
    pthread_detach(th_play_signal);
    
    signal(SIGINT, signal_handler);
    
    while(in_aborting)
    {
        if(player == enable)
        {
            debug(1, "********adplay start********");
            adplay(play_buf, CONTENT_TYPE_URL);
            end_play();
        }
        else
        {
            usleep(100*1000);
        }
    }
    
    pthread_cond_destroy(&condwakeup);
    pthread_cond_destroy(&condplay_az);
    pthread_mutex_destroy(&mutexwakeup);
    pthread_mutex_destroy(&mutexplay_az);
    printf("main program is exit \n");
    
    return 0;
}

