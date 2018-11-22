/*************************************************************************
    > File Name: driver_led_key.h
    > Author: zyc
    > Mail: zyc@imio.ai 
    > Created Time: 2017/10/10
 ************************************************************************/
#ifndef _DRIVER_LED_KEY_H_
#define _DRIVER_LED_KEY_H_


#define    HIGHT_VOLUME     50 //63
#define    LOW_VOLUME        10


enum VOLUME_VALUE{
    VOLUME_DOWN = 1,
    VOLUME_UP = 2,
    VOLUME_CLOUD_DOWN,
    VOLUME_CLOUD_UP,
};

enum BUTTON_VALUE {
    BUTTON_MUTE = 1,
    BUTTON_MUTE_LONG,
    BUTTON_DOWN = 3,
    BUTTON_DOWN_LONG,
    BUTTON_UP = 5,
    BUTTON_UP_LONG,
    BUTTON_MENU = 7,
    BUTTON_MENU_LONG
};

enum LED_VALUE {
    OFF_ON_LED = 23,
    ALL_LEDS_BLUE,
    RED_SLOWER_TURN,
    ALL_LEDS_RED = 26,
    
    RE_ORANGE = 100,
    ORANGE_TO_RED,
    ORANGE_TO_GREEN,
    RED_FAST_TURN,
    RED_TWO_TIMES_TURN,
    RED_SLOW_TURN,
    PROGRESS_BAR,
    ON_AND_LISTERNING,
    RE_BLUE,
    BLUE_SLOW_TURN,
    VOL_ONE_TO_TEN,
    MUL_COLOR_RE,
    
    ALL_LEDS_ORANGE = 112,
    RE_GREEN,
    RE_RED,
    ONE_BLUE_BAR = 115,
    TWO_BLUE_BAR,
    THR_BLUE_BAR,
    FOU_BLUE_BAR,
    FIV_BLUE_BAR,
    SIX_BLUE_BAR,
    SEV_BLUE_BAR,
    EIG_BLUE_BAR,
    NIN_BLUE_BAR,
    TEN_BLUE_BAR,
    ELE_BLUE_BAR,
    TWE_BLUE_BAR,
    
    FIRST_LED = 11,
    SECOND_LED,
    THIRD_LED,
    FOURTH_LED,
    FIFTH_LED,
    SIXTH_LED,
    SEVENTH_LED,
    EIGHTH_LED,
    NINTH_LED,
    TENTH_LED,
    ELEVENTH_LED,
    TWELFTH_LED,
    
    FIRST_BLUE_LED = 200,
    SECOND_BLUE_LED,
    THIRD_BLUE_LED,
    FOURTH_BLUE_LED,
    FIFTH_BLUE_LED,
    SIXTH_BLUE_LED,
    SEVENTH_BLUE_LED,
    EIGHTH_BLUE_LED,
    NINTH_BLUE_LED,
    TENTH_BLUE_LED,
    ELEVENTH_BLUE_LED,
    TWELFTH_BLUE_LED,
    ALL_BLUE_LED = 212,
    
    VOL_ZER_STEP = 1100,
    VOL_ONE_STEP,
    VOL_TWO_STEP,
    VOL_THR_STEP,
    VOL_FOU_STEP,
    VOL_FIV_STEP,
    VOL_SIX_STEP,
    VOL_SEV_STEP,
    VOL_EIG_STEP,
    VOL_NIN_STEP,
    VOL_TEN_STEP = 11010,
};

extern int init_volume();
extern int led_driver(int );
extern int key_driver();
extern int control_volume(int );
extern void button_set_mute();
extern int set_volume(int );


#endif
