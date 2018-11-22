/*************************************************************************
    > File Name: driver_led_key.c
    > Author: zyc
    > Mail: zyc@imio.ai 
    > Created Time: 2017/10/10
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <linux/input.h>
#include <ctype.h>
#include <stdbool.h>
#include "cap.h"
#include "wireless.h"
#include "httpclient.h"
#include "include/asoundlib.h"
#include "driver_led_key.h"


bool is_mute = false;
bool is_volume_led = false;
bool is_end_press = true;
bool is_begin_press = false;
static int volume_sum;
extern int wifi_status;
extern bool is_heartbeat_normal;
extern bool is_wireless_ready;
extern int wifi_count;
extern int build_relese;


static void tinymix_list_controls(struct mixer *mixer);
static int tinymix_detail_control(struct mixer *mixer, const char *control, int print_all);
static int tinymix_set_value(struct mixer *mixer, const char *control, char **values, unsigned int num_values);


//func
char* read_volume()
{
    static char buffer[6];
    FILE *fp;
    
    if((fp = fopen("/etc/config/volume", "r")) != NULL)
    {
        memset(buffer, 0, 6);
        fread(buffer, 1, 6, fp);
        debug(1, "%s", buffer);
    }
    fclose(fp);
    
    return buffer;
}

int write_volume(char *volume)
{
    FILE *fp;
    
    fp = fopen("/etc/config/volume", "w");
    fwrite(volume, strlen(volume), 1, fp);
    fclose(fp);
    
    return 0;
}

int led_driver(int number)
{
    int fs;
    int val;
    unsigned char buf[1];
    
    fs = open("/dev/led1_dev", O_RDWR);
    if (fs < 0)
    {
        printf("can't open /dev/led1_dev\n");
        return -1;
    }
    
    if (number)
    {
        ioctl(fs, number);    //使用值传入设置参数
    }
    else
    {
        printf("Usage: eg. arg num is 100-111 \n");
        return -1;
    }
    close(fs);
    
    //debug(1, "led_driver : %d ", number);
    
    return 0;
}


static void tinymix_list_controls(struct mixer *mixer)
{
    struct mixer_ctl *ctl;
    const char *name, *type;
    unsigned int num_ctls, num_values;
    unsigned int i;

    num_ctls = mixer_get_num_ctls(mixer);

    printf("Number of controls: %ul\n", num_ctls);

    printf("ctl\ttype\tnum\t%-40s value\n", "name");
    for (i = 0; i < num_ctls; i++) {
        ctl = mixer_get_ctl(mixer, i);

        name = mixer_ctl_get_name(ctl);
        type = mixer_ctl_get_type_string(ctl);
        num_values = mixer_ctl_get_num_values(ctl);
        printf("%ul\t%s\t%ul\t%-40s", i, type, num_values, name);
        tinymix_detail_control(mixer, name, 0);
    }
}

static void tinymix_print_enum(struct mixer_ctl *ctl, int print_all)
{
    unsigned int num_enums;
    unsigned int i;
    const char *string;

    num_enums = mixer_ctl_get_num_enums(ctl);

    for (i = 0; i < num_enums; i++) {
        string = mixer_ctl_get_enum_string(ctl, i);
        if (print_all)
        {
            printf("\t%s%s", mixer_ctl_get_value(ctl, 0) == (int)i ? ">" : "", string);
        }
        else if (mixer_ctl_get_value(ctl, 0) == (int)i)
            printf(" %-s", string);
    }
}

static int tinymix_detail_control(struct mixer *mixer, const char *control, int print_all)
{
    struct mixer_ctl *ctl;
    enum mixer_ctl_type type;
    unsigned int num_values;
    unsigned int i;
    int min, max;
    int volume;

    if (isdigit(control[0]))
        ctl = mixer_get_ctl(mixer, atoi(control));
    else
        ctl = mixer_get_ctl_by_name(mixer, control);

    if (!ctl) {
        fprintf(stderr, "Invalid mixer control\n");
        return -1;
    }

    type = mixer_ctl_get_type(ctl);
    num_values = mixer_ctl_get_num_values(ctl);

    if (print_all)
        printf("%s:", mixer_ctl_get_name(ctl));

    for (i = 0; i < num_values; i++) {
        switch (type)
        {
        case MIXER_CTL_TYPE_INT:
        {
            volume = mixer_ctl_get_value(ctl, i);
            printf(" %d", mixer_ctl_get_value(ctl, i));
            break;
        }
        case MIXER_CTL_TYPE_BOOL:
            printf(" %s", mixer_ctl_get_value(ctl, i) ? "On" : "Off");
            break;
        case MIXER_CTL_TYPE_ENUM:
            tinymix_print_enum(ctl, print_all);
            break;
         case MIXER_CTL_TYPE_BYTE:
            printf(" 0x%02x", mixer_ctl_get_value(ctl, i));
            break;
        default:
            printf(" unknown");
            break;
        };
    }

    if (print_all) {
        if (type == MIXER_CTL_TYPE_INT) {
            min = mixer_ctl_get_range_min(ctl);
            max = mixer_ctl_get_range_max(ctl);
            printf(" (range %d->%d)", min, max);
        }
    }
    printf("\n");
    
    return volume;
}

static int tinymix_set_value(struct mixer *mixer, const char *control, char **values, unsigned int num_values)
{
    struct mixer_ctl *ctl;
    enum mixer_ctl_type type;
    unsigned int num_ctl_values;
    unsigned int i;

    if (isdigit(control[0]))
        ctl = mixer_get_ctl(mixer, atoi(control));
    else
        ctl = mixer_get_ctl_by_name(mixer, control);

    if (!ctl) {
        fprintf(stderr, "Invalid mixer control\n");
        return -1;
    }

    type = mixer_ctl_get_type(ctl);
    num_ctl_values = mixer_ctl_get_num_values(ctl);

    if (isdigit(values[0][0])) {
        if (num_values == 1) {
            /* Set all values the same */
            int value = atoi(values[0]);

            for (i = 0; i < num_ctl_values; i++) {
                if (mixer_ctl_set_value(ctl, i, value)) {
                    fprintf(stderr, "Error: invalid value\n");
                    return -1;
                }
            }
        } else {
            /* Set multiple values */
            if (num_values > num_ctl_values) {
                fprintf(stderr,
                        "Error: %ul values given, but control only takes %ul\n",
                        num_values, num_ctl_values);
                return -1;
            }
            for (i = 0; i < num_values; i++) {
                if (mixer_ctl_set_value(ctl, i, atoi(values[i]))) {
                    fprintf(stderr, "Error: invalid value for index %ul\n", i);
                    return -1;
                }
            }
        }
    } else {
        if (type == MIXER_CTL_TYPE_ENUM) {
            if (num_values != 1) {
                fprintf(stderr, "Enclose strings in quotes and try again\n");
                return -1;
            }
            if (mixer_ctl_set_enum_by_string(ctl, values[0]))
                fprintf(stderr, "Error: invalid enum value\n");
        } else {
            fprintf(stderr, "Error: only enum types can be set with strings\n");
        }
    }
    
    return 0;
}

int volume_tinymix(int volume_num, char *volume_str)
{
    int volume_now = -1;
    struct mixer *mixer;
    int card = 0;
    
    mixer = mixer_open(card);
    if (!mixer) 
    {
        fprintf(stderr, "Failed to open mixer\n");
        return EXIT_FAILURE;
    }
    if(volume_num == 0)
    {
        volume_now = tinymix_detail_control(mixer, "2", 1);
    }
    else
    {
        volume_now = tinymix_set_value(mixer, "2", &volume_str, 1);
        write_volume(volume_str);
    }
    
    mixer_close(mixer);
    
    return volume_now;
}


int control_volume(int kind)
{
    int led_volume;
    int ge;
    
    if((volume_sum < HIGHT_VOLUME) && (volume_sum > LOW_VOLUME))
    {
        if((kind == VOLUME_UP) || (kind == VOLUME_CLOUD_UP))
        {
            if(volume_sum <= 20)
            {
                volume_sum += 5;
            }
            else
            {
                if(kind == VOLUME_UP)
                {
                    volume_sum += volume_sum * 0.1;
                }
                else
                {
                    volume_sum += volume_sum * 0.2;
                }
            }
            if(volume_sum > HIGHT_VOLUME)
            {
                volume_sum = HIGHT_VOLUME;
            }
        }
        else if((kind == VOLUME_DOWN) || (kind == VOLUME_CLOUD_DOWN))
        {
            if(volume_sum <= 20)
            {
                volume_sum -= 5;
            }
            else
            {
                if(kind == VOLUME_DOWN)
                {
                    volume_sum -= volume_sum * 0.1;
                }
                else
                {
                    volume_sum -= volume_sum * 0.2;
                }
            }
            if(volume_sum < LOW_VOLUME)
            {
                volume_sum = LOW_VOLUME;
            }
        }
        
        ge = volume_sum % 10;
        if((ge >= 0) && (ge < 4))
        {
            led_volume = (volume_sum / 10) * 10;
        }
        else if((ge >= 4) && (ge < 7))
        {
            led_volume = (volume_sum / 10) * 10 + 5;
        }
        else if((ge >= 7) && (ge <= 9))
        {
            led_volume = (volume_sum / 10) * 10 + 10;
        }
        debug(1, "volume_sum = %d, led_volume = %d\n", volume_sum, led_volume);
        is_volume_led = true;
        
        switch(led_volume)
        {
            case 0:
            {
                led_driver(VOL_ZER_STEP);
                break;
            }
            case 5:
            {
                led_driver(VOL_ONE_STEP);
                break;
            }
            case 10:
            {
                led_driver(VOL_TWO_STEP);
                break;
            }
            case 15:
            {
                led_driver(VOL_THR_STEP);
                break;
            }
            case 20:
            {
                led_driver(VOL_FOU_STEP);
                break;
            }
            case 25:
            {
                led_driver(VOL_FIV_STEP);
                break;
            }
            case 30:
            {
                led_driver(VOL_SIX_STEP);
                break;
            }
            case 35:
            {
                led_driver(VOL_SEV_STEP);
                break;
            }
            case 40:
            {
                led_driver(VOL_EIG_STEP);
                break;
            }
            case 45:
            {
                led_driver(VOL_NIN_STEP);
                break;
            }
            case 50:
            {
                led_driver(VOL_TEN_STEP);
                break;
            }
            default:
            {
                led_driver(VOL_TEN_STEP);
                break;
            }
        }
        char* szBuffer = (char *)malloc(sizeof(int) + 1);
        memset(szBuffer, 0, sizeof(int) + 1);
        sprintf(szBuffer, "%d", volume_sum);
        volume_tinymix(1, szBuffer);
        free(szBuffer);
    }
    else
    {
        char* szBuffer = (char *)malloc(sizeof(int) + 1);
        memset(szBuffer, 0, sizeof(int) + 1);
        
        if(volume_sum >= HIGHT_VOLUME)
        {
            led_driver(VOL_TEN_STEP);
            volume_sum = HIGHT_VOLUME - 1;
        }
        else if(volume_sum <= LOW_VOLUME)
        {
            led_driver(VOL_ZER_STEP);
            volume_sum = LOW_VOLUME + 1;
        }
        sprintf(szBuffer, "%d", volume_sum);
        volume_tinymix(1, szBuffer);
        free(szBuffer);
    }
    
    volume_sum = volume_tinymix(0, "50");
    azplay(mode_2, "/home/dong.mp3");
    
    return 0;
}

int set_volume(int percent)
{
    int volume_set;
    char volume_str[4];

    if(100 == percent)
    {
        volume_set = HIGHT_VOLUME;
    }
    else
    {
        volume_set = HIGHT_VOLUME * percent / 100;
    }
    
    if((volume_set<= 100) || (volume_set >= 0))
    {
        sprintf(volume_str, "%d", volume_set);
        //debug(1, "volume_set = %d  volume = %s", volume_set, volume_str);
        volume_tinymix(1, volume_str);
        volume_sum = volume_tinymix(0, "50");
        debug(1, "volume = %d", volume_sum);
    }
    
    return 0;
}

void *button_up(void *p)
{
    do
    {
        control_volume(VOLUME_UP);
        sleep(1);
    }
    while(is_end_press);
    
    debug(1, "----Button_up end----\n");
    
    pthread_exit(0);
}

void *button_down(void *p)
{
    do
    {
        control_volume(VOLUME_DOWN);
        sleep(1);
    }
    while(is_end_press);
    
    debug(1, "----Button_down end----\n");
    
    pthread_exit(0);
}

void button_set_mute()
{
    is_mute = !is_mute;
    debug(1, "is_mute = %d\n", is_mute);
    azplay(mode_2, "/home/dong.mp3");
    //build_relese = BUILD_TEST;//test///////////////////////////////////
    
    if(is_mute)
    {
        led_driver(ALL_LEDS_RED);//RED 
        sleep(1);
        char* szBuffer = (char *)malloc(sizeof(int) + 1);
        memset(szBuffer, 0, sizeof(int) + 1);
        sprintf(szBuffer, "%d", LOW_VOLUME);
        volume_tinymix(1, szBuffer);
        free(szBuffer);
    }
    else
    {
        led_driver(OFF_ON_LED);
        char* szBuffer = (char *)malloc(sizeof(int) + 1);
        memset(szBuffer, 0, sizeof(int) + 1);
        sprintf(szBuffer, "%d", volume_sum);
        volume_tinymix(1, szBuffer);
        free(szBuffer);
    }
}

int init_volume()
{
    //1. read volume file 
    if(access("/etc/config/volume",F_OK) == -1)
    {
        volume_sum = volume_tinymix(0, "50");
        debug(1, "volume_sum = %d\n", volume_sum);
        
        if(volume_sum >= HIGHT_VOLUME)
        {
            volume_sum = HIGHT_VOLUME;// * 0.8;
            char* szBuffer = (char *)malloc(sizeof(int) + 1);
            memset(szBuffer, 0, sizeof(int) + 1);
            sprintf(szBuffer, "%d", volume_sum);
            volume_tinymix(1, szBuffer);
            free(szBuffer);
        }
        else
        {
            char volume_str[4];
            sprintf(volume_str, "%d", volume_sum);
            write_volume(volume_str);
        }
    }
    else
    {
        char *volume_now = read_volume();
        debug(1, "volume_now = %s\n", volume_now);
        if((atoi(volume_now)) || (0 == atoi(volume_now)))
        {
            volume_sum = atoi(volume_now);
            debug(1, "volume_sum = %d\n", volume_sum);
            volume_tinymix(1, volume_now);
            if(volume_sum <= LOW_VOLUME)
            {
                volume_sum = LOW_VOLUME;;
                led_driver(ALL_LEDS_RED);//RED 
                char* szBuffer = (char *)malloc(sizeof(int) + 1);
                memset(szBuffer, 0, sizeof(int) + 1);
                sprintf(szBuffer, "%d", volume_sum);
                volume_tinymix(1, szBuffer);
                free(szBuffer);
            }
            if(LOW_VOLUME == volume_sum)
            {
                button_set_mute();
            }
        }
    }
    
    volume_sum = volume_tinymix(0, "50");
    debug(1, "volume_sum = %d\n", volume_sum);
    
    return 0;
}

int key_driver()
{
    int buttons_fd;
    struct input_event ev_key;
    pthread_t button_press;
    
    buttons_fd = open("/dev/input/event2", O_RDWR);//event2  event5
    
    if (buttons_fd < 0)
    {
        perror("open key event2 err!");
        exit(1);
    }

    while(1)
    {
        read(buttons_fd, &ev_key, sizeof(ev_key));
        //debug(1, "type = %d, value = %d, code = %d\n", ev_key.type, ev_key.value, ev_key.code);
        
        //press a long time
        if((ev_key.type == 1) || (ev_key.type == 0))
        {
            if(!is_mute)
            {
                if(ev_key.code == BUTTON_MUTE_LONG)
                {
                    if(0 == ev_key.value)
                    {
                        
                    }
                    else if(1 == ev_key.value)
                    {
                        //build_relese = BUILD_BUILD;//test///////////////////////////////////
                        //device_ai_speaker("妙琦已进入开发模式。");
                    }
                }
                else if(ev_key.code == BUTTON_DOWN_LONG)
                {
                    if(0 == ev_key.value)
                    {
                        is_end_press = false;
                    }
                    else if(1 == ev_key.value)
                    {
                        if(!is_begin_press)
                        {
                            is_begin_press = true;
                            pthread_create(&button_press, NULL, button_down, NULL);
                        }
                    }
                }
                else if(ev_key.code == BUTTON_UP_LONG)
                {
                    if(0 == ev_key.value)
                    {
                        is_end_press = false;
                    }
                    else if(1 == ev_key.value)
                    {
                        if(!is_begin_press)
                        {
                            is_begin_press = true;
                            pthread_create(&button_press, NULL, button_up, NULL);
                        }
                    }
                }
                else if(ev_key.type && ev_key.code == BUTTON_MENU_LONG)
                {
                    if(0 == ev_key.value)
                    {
                        
                    }
                    else if(1 == ev_key.value)
                    {
                        system("echo > /etc/wifi/wpa_supplicant.conf");
                        wifi_status = WIFI_CONNECTFAIL;
                        wifi_count = 5;
                        azplay(mode_2, "/home/dong.mp3");
                        led_driver(ALL_LEDS_ORANGE);
                        azplay(mode_3, "/home/rmwifi.mp3");
                        is_heartbeat_normal = false;
                        is_wireless_ready = false;
                    }
                }
            }
        }
        
        //press a short time
        if(ev_key.value)
        {
            //Mute
            if(ev_key.type && ev_key.code == BUTTON_MUTE)
            {
                button_set_mute();
            }
            //Down
            if(!is_mute)
            {
                if(ev_key.type && ev_key.code == BUTTON_DOWN)
                {
                    control_volume(VOLUME_DOWN);
                }
                //Up
                if(ev_key.type && ev_key.code == BUTTON_UP)
                {
                    control_volume(VOLUME_UP);
                }
                //O
                if(ev_key.type && ev_key.code == BUTTON_MENU)
                {
                    key_wakeup();
                }
            }
        }
    }

    close(buttons_fd);
    
    return 0;
}


#if 0
int main(int argc, char *argv[])
{
    int ret;
    
    led_driver(106);
    ret = key_driver();
    printf("ret = %d\n", ret);
    
    return 0;
}
#endif

