#include <linux/kernel.h>  /**/ 
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#include <linux/workqueue.h>
#include <mach/sys_config.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/delay.h>

#include <linux/leds.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/device.h>

#include <linux/workqueue.h>
#include <linux/kthread.h>

#include "fl3236.h"
#include "rgb_val.h"

#define LED_DBG  1

#if (LED_DBG)
#define LED_DBG(format,args...)  printk("[LED_DBG] "format,##args)
#else
#define LED_DBG(...)
#endif

static int major;
static struct class *class = NULL;
static struct i2c_client *led1_client = NULL; 
static struct device *led1_dev = NULL;
static struct gpio_config fl3236_led_shut;
u32 g_cmd = 0;
u8 flag = 0;
char g_data[4]={0};
u8 g_num = 0;
u32 g_cmd_array[1] = {0};
u8 g_switch_status = 1;

static struct task_struct *led_light_task = NULL;
static struct task_struct *led_ctrl_task = NULL;
static int light_one_led( u8 pwm_reg, u8 pwm_val,
                            bool oo, u8 colour, u32 mtime );
#if 1 
#define     re_orange               100
#define     orange_to_red           101
#define     orange_to_green         102
#define     red_fast_turn           103
#define     red_2_times_turn        104
#define     red_slow_turn           105
#define     progress_bar            106
#define     on_and_listening        107
#define     re_blue                 108
#define     blue_slow_turn          109
#define     vol_one_to_ten          110
#define     mul_color_re            111

#define     all_leds_orange         112
#define     re_green                113
#define     re_red                  114

#define     one_white_blue                  200
#define     two_white_blue                  201
#define     thr_white_blue                  202
#define     fou_white_blue                  203
#define     fiv_white_blue                  204
#define     six_white_blue                  205
#define     sev_white_blue                  206
#define     eig_white_blue                  207
#define     nin_white_blue                  208
#define     ten_white_blue                  209
#define     ele_white_blue                  210
#define     twl_white_blue                  211
#define     all_blue                        212
#define     all_green                        213
#define     all_white                        214


#define     one_blue_bar                115
#define     two_blue_bar                116
#define     thr_blue_bar                117
#define     fou_blue_bar                118
#define     fiv_blue_bar                119
#define     six_blue_bar                120
#define     sev_blue_bar                121
#define     eig_blue_bar                122
#define     nin_blue_bar                123
#define     ten_blue_bar                124
#define     ele_blue_bar                125
#define     twe_blue_bar                126

#define     vol_zer_step          1100
#define     vol_one_step          1101
#define     vol_two_step          1102
#define     vol_thr_step          1103
#define     vol_fou_step          1104
#define     vol_fiv_step          1105
#define     vol_six_step          1106
#define     vol_sev_step          1107
#define     vol_eig_step          1108
#define     vol_nin_step          1109
#define     vol_ten_step          11010


#endif

#define     first_led            11
#define     second_led            12
#define     third_led            13
#define     fourth_led            14
#define     fifth_led            15
#define     sixth_led            16
#define     seventh_led            17
#define     eighth_led            18
#define     ninth_led            19
#define     tenth_led            20
#define     eleventh_led            21
#define     twelfth_led            22
#define     off_all_led            23

#define     start_up            24
#define     all_led_red            25
#define     all_leds_long_time_red  26

  /*
  *  看见智能灯效
  */

/* 不亮灯, 电源关闭 , 可直接使用参数23*/
/* 蓝色直接用参数 212 */
#define start_white_turn_white_around  30 /* 启动蓝光变白光 */
#define pulse_white 31
#define little_blue_around      32  /* 淡蓝色跑马灯 */
#define keep_little_blue 33 //淡蓝色待调试 
#define pulse_orange 34 // 
#define keep_orange 35
#define blue_around 36
#define pulse_red 37
#define keep_red 38
//#define key                     39  /* 按键灯 */

#define     led_drv_bus_num	1	

/* 蓝慢??? */
static void blue_slow_turn_fun(void)
{
    int i, pwm_reg=0,pwm_val = 240;

    for (pwm_reg=1; pwm_reg<8; pwm_reg +=1){
		if (g_cmd_array[0] != g_cmd){
			goto fun_end2;
			}
        switch (pwm_reg){
            case 1: 
                pwm_val = 240;
        break;
            case 2: 
                pwm_val = 200;
        break;
            case 3:
                pwm_val = 160;
        break;
            case 4: 
                pwm_val = 120;
        break;
            case 5: 
                pwm_val = 80;
        break;
            case 6: 
                pwm_val = 40;
        break;
            case 7: 
                pwm_val = 0;
        break;
            }

    for (i=1;i<37; i+=3){
		if (g_cmd_array[0] != g_cmd){
			goto fun_end2;
			}
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[i-1], led1_pwm_val_array[0]);//G
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[i], led1_pwm_val_array[pwm_val]);//B
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[i+1], led1_pwm_val_array[0]);//R
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[i-1], Led_on);//G
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[i], Led_on);//B
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[i+1], Led_on); //R
        }
            i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
            mdelay(300);
        }

       for (pwm_reg=7; pwm_reg>0; pwm_reg --){
		   if (g_cmd_array[0] != g_cmd){
			   goto fun_end2;
			   }
           switch (pwm_reg){
               case 1: 
                   pwm_val = 240;
           break;
               case 2: 
                   pwm_val = 200;
           break;
               case 3:
                   pwm_val = 160;
           break;
               case 4: 
                   pwm_val = 120;
           break;
               case 5: 
                   pwm_val = 80;
           break;
               case 6: 
                   pwm_val = 40;
           break;
               case 7: 
                   pwm_val = 0;
           break;
               }
       for (i=1;i<37; i+=3){
		   if (g_cmd_array[0] != g_cmd){
			   goto fun_end2;
			   }
           i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[i-1], led1_pwm_val_array[0]);//G
           i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[i], led1_pwm_val_array[pwm_val]);//B
           i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[i+1], led1_pwm_val_array[0]);//R
           i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[i-1], Led_on);//G
           i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[i], Led_on);//B
           i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[i+1], Led_on); //R
           }
               i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
               mdelay(300);
           }

       for (i=1;i<37; i+=3){
		   if (g_cmd_array[0] != g_cmd){
			   goto fun_end2;
			   }
           i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[i-1], led1_pwm_val_array[0]);//G
           i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[i], led1_pwm_val_array[0]);//B
           i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[i+1], led1_pwm_val_array[0]);//R
           i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[i-1], Led_on);//G
           i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[i], Led_on);//B
           i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[i+1], Led_on); //R
           }
               i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
               mdelay(300);

           for (i=1;i<37; i+=3){
			   if (g_cmd_array[0] != g_cmd){
				   goto fun_end2;
				   }
               i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[i-1], led1_pwm_val_array[0]);//G
               i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[i], led1_pwm_val_array[pwm_val]);//B
               i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[i+1], led1_pwm_val_array[0]);//R
               i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[i-1], Led_on);//G
               i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[i], Led_on);//B
               i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[i+1], Led_on); //R
               }
                   i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
                   mdelay(300);
fun_end2:
g_switch_status = 0;		  
       
}

static int mul_color_re_fun(void)
{
    int i;  
#if 1
    for (i=0;i<156;i +=12){
		if (g_cmd_array[0] != g_cmd){
			goto fun_end5;
			}
    mdelay(200);
    
    /* LED0 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[0], led1_pwm_val_array[rgb_val[i][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[1], led1_pwm_val_array[rgb_val[i][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[2], led1_pwm_val_array[rgb_val[i][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[0], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[1], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[2], Led_on); 

    /* LED1 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[3], led1_pwm_val_array[rgb_val[i+1][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[4], led1_pwm_val_array[rgb_val[i+1][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[5], led1_pwm_val_array[rgb_val[i+1][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[3], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[4], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[5], Led_on); 
    
    /* LED2 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[6], led1_pwm_val_array[rgb_val[i+2][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[7], led1_pwm_val_array[rgb_val[i+2][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[8], led1_pwm_val_array[rgb_val[i+2][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[6], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[7], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[8], Led_on); 
	if (g_cmd_array[0] != g_cmd){
		goto fun_end5;
		}

    /* LED3 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[9], led1_pwm_val_array[rgb_val[i+3][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[10], led1_pwm_val_array[rgb_val[i+3][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[11], led1_pwm_val_array[rgb_val[i+3][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[9], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[10], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[11], Led_on); 

     /* LED4 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[12], led1_pwm_val_array[rgb_val[i+4][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[13], led1_pwm_val_array[rgb_val[i+4][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[14], led1_pwm_val_array[rgb_val[i+4][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[12], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[13], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[14], Led_on); 
	if (g_cmd_array[0] != g_cmd){
		goto fun_end5;
		}

    /* LED5 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[15], led1_pwm_val_array[rgb_val[i+5][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[16], led1_pwm_val_array[rgb_val[i+5][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[17], led1_pwm_val_array[rgb_val[i+5][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[15], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[16], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[17], Led_on); 

    /* LED6 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[18], led1_pwm_val_array[rgb_val[i+6][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[19], led1_pwm_val_array[rgb_val[i+6][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[20], led1_pwm_val_array[rgb_val[i+6][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[18], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[19], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[20], Led_on); 

    /* LED7 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[21], led1_pwm_val_array[rgb_val[i+7][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[22], led1_pwm_val_array[rgb_val[i+7][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[23], led1_pwm_val_array[rgb_val[i+7][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[21], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[22], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[23], Led_on); 

    /* LED8 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[24], led1_pwm_val_array[rgb_val[i+8][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[25], led1_pwm_val_array[rgb_val[i+8][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[26], led1_pwm_val_array[rgb_val[i+8][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[24], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[25], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[26], Led_on); 
	if (g_cmd_array[0] != g_cmd){
		goto fun_end5;
		}

    /* LED9 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[27], led1_pwm_val_array[rgb_val[i+9][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[28], led1_pwm_val_array[rgb_val[i+9][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[29], led1_pwm_val_array[rgb_val[i+9][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[27], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[28], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[29], Led_on); 

    /* LED10 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[30], led1_pwm_val_array[rgb_val[i+10][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[31], led1_pwm_val_array[rgb_val[i+10][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[32], led1_pwm_val_array[rgb_val[i+10][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[30], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[31], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[32], Led_on); 

    /* LED11 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[33], led1_pwm_val_array[rgb_val[i+11][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[34], led1_pwm_val_array[rgb_val[i+11][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[35], led1_pwm_val_array[rgb_val[i+11][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[33], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[34], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[35], Led_on); 
    i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
    }
#endif
fun_end5:
	g_switch_status = 0;
    return 0;
    }

static int ctl_first_led_fun(void)
{
    /* LED0 */
    int i;
    for (i=2;i<36;i +=3){
		if (i == 2){
			light_one_led(i-2, 255, 1, 0, 0);
			}else {
    light_one_led(i, 0, 0, 8, 0);
    }
    i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
    	}

fun_end6:
	g_switch_status = 0;
    return 0;
    }

static int ctl_secod_led_fun(void)
{
    /* LED1 */
    int i;
    for (i=2;i<36;i +=3){
		if (i == 5){
			light_one_led(i-2, 255, 1, 0, 0);
			}else {
    light_one_led(i, 0, 0, 8, 0);
    }
    i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
    	}

fun_end7:
	g_switch_status = 0;
    return 0;
    }

static int ctl_third_led_fun(void)
{
    /* LED2 */
    int i;
    for (i=2;i<36;i +=3){
		if (i == 8){
			light_one_led(i-2, 255, 1, 0, 0);
			}else {
    light_one_led(i, 0, 0, 8, 0);
    }
    i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
    	}

fun_end8:
	g_switch_status = 0;
    return 0;
    }

static int ctl_fourth_led_fun(void)
{
    /* LED3 */
    int i;
    for (i=2;i<36;i +=3){
		if (i == 11){
			light_one_led(i-2, 255, 1, 0, 0);
			}else {
    light_one_led(i, 0, 0, 8, 0);
    }
    i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
    	}
 
fun_end9:
	g_switch_status = 0;
    return 0;
    }

static int ctl_fifth_led_fun(void)
{
     /* LED4 */
    int i;
    for (i=2;i<36;i +=3){
		if (i == 14){
			light_one_led(i-2, 255, 1, 0, 0);
			}else {
    light_one_led(i, 0, 0, 8, 0);
    }
    i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
    	}

fun_end10:
	g_switch_status = 0;
    return 0;
    }

static int ctl_sixth_led_fun(void)
{
    /* LED5 */
    int i;
    for (i=2;i<36;i +=3){
		if (i == 17){
			light_one_led(i-2, 255, 1, 0, 0);
			}else {
    light_one_led(i, 0, 0, 8, 0);
    }
    i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
    	}

fun_end11:
	g_switch_status = 0;
    return 0;
    }

static int ctl_seventh_led_fun(void)
{
    /* LED6 */
    int i;
    for (i=2;i<36;i +=3){
		if (i == 20){
			light_one_led(i-2, 255, 1, 0, 0);
			}else {
    light_one_led(i, 0, 0, 8, 0);
    }
    i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
    	}

fun_end12:
	g_switch_status = 0;
    return 0;
    }

static int ctl_eighth_led_fun(void)
{
    /* LED7 */
    int i;
    for (i=2;i<36;i +=3){
		if (i == 23){
			light_one_led(i-2, 255, 1, 0, 0);
			}else {
    light_one_led(i, 0, 0, 8, 0);
    }
    i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
    	}

fun_end13:
	g_switch_status = 0;
    return 0;
    }

static int ctl_ninth_led_fun(void)
{
    /* LED8 */
    int i;
    for (i=2;i<36;i +=3){
		if (i == 26){
			light_one_led(i-2, 255, 1, 0, 0);
			}else {
    light_one_led(i, 0, 0, 8, 0);
    }
    i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
    	}

fun_end14:
	g_switch_status = 0;
    return 0;
    }

static int ctl_tenth_led_fun(void)
{
    /* LED9 */
    int i;
    for (i=2;i<36;i +=3){
		if (i == 29){
			light_one_led(i-2, 255, 1, 0, 0);
			}else {
    light_one_led(i, 0, 0, 8, 0);
    }
    i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
    	}
fun_end15:
	g_switch_status = 0;
    return 0;
}

static int ctl_eleventh_led_fun(void)
{
    /* LED10 */
    int i;
    for (i=2;i<36;i +=3){
		if (i == 32){
			light_one_led(i-2, 255, 1, 0, 0);
			}else {
    light_one_led(i, 0, 0, 8, 0);
    }
    i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
    	}
fun_end16:
	g_switch_status = 0;
    return 0;
}

static int ctl_twelfth_led_fun(void)
{
    /* LED11 */
    int i;
    for (i=2;i<36;i +=3){
		if (i == 35){
			light_one_led(i-2, 255, 1, 0, 0);
			}else {
    light_one_led(i, 0, 0, 8, 0);
    }
    i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
    	}
fun_end17:
	g_switch_status = 0;
    return 0;
}

static int ctl_off_all_led_fun(void)
{
    int i;
    for (i=2;i<36;i +=3){
    light_one_led(i, 0, 0, 8, 0);
    }
    i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
    
    return 0;
}

static int all_leds_long_time_red_fun(void)
{
    int i;
    for (i=2;i<36;i +=3){
		if (g_cmd_array[0] != g_cmd){
			goto fun_end18;
			}
    light_one_led(i, 255, 1, 2, 0);
    }
    i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
fun_end18:
	g_switch_status = 0;
    return 0;
}



static int red_color_fun(void)
{
    int i;  
#if 1
    for (i=0;i<156;i +=12){
		if (g_cmd_array[0] != g_cmd){
			goto fun_end2;
			}
    mdelay(200);
    
    /* LED0 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[0], led1_pwm_val_array[red_val[i][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[1], led1_pwm_val_array[red_val[i][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[2], led1_pwm_val_array[red_val[i][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[0], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[1], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[2], Led_on); 

    /* LED1 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[3], led1_pwm_val_array[red_val[i+1][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[4], led1_pwm_val_array[red_val[i+1][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[5], led1_pwm_val_array[red_val[i+1][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[3], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[4], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[5], Led_on); 
    
    /* LED2 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[6], led1_pwm_val_array[red_val[i+2][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[7], led1_pwm_val_array[red_val[i+2][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[8], led1_pwm_val_array[red_val[i+2][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[6], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[7], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[8], Led_on); 

    /* LED3 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[9], led1_pwm_val_array[red_val[i+3][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[10], led1_pwm_val_array[red_val[i+3][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[11], led1_pwm_val_array[red_val[i+3][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[9], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[10], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[11], Led_on); 

     /* LED4 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[12], led1_pwm_val_array[red_val[i+4][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[13], led1_pwm_val_array[red_val[i+4][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[14], led1_pwm_val_array[red_val[i+4][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[12], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[13], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[14], Led_on); 

    /* LED5 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[15], led1_pwm_val_array[red_val[i+5][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[16], led1_pwm_val_array[red_val[i+5][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[17], led1_pwm_val_array[red_val[i+5][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[15], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[16], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[17], Led_on); 

    /* LED6 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[18], led1_pwm_val_array[red_val[i+6][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[19], led1_pwm_val_array[red_val[i+6][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[20], led1_pwm_val_array[red_val[i+6][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[18], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[19], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[20], Led_on); 

    /* LED7 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[21], led1_pwm_val_array[red_val[i+7][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[22], led1_pwm_val_array[red_val[i+7][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[23], led1_pwm_val_array[red_val[i+7][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[21], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[22], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[23], Led_on); 

    /* LED8 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[24], led1_pwm_val_array[red_val[i+8][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[25], led1_pwm_val_array[red_val[i+8][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[26], led1_pwm_val_array[red_val[i+8][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[24], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[25], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[26], Led_on); 

    /* LED9 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[27], led1_pwm_val_array[red_val[i+9][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[28], led1_pwm_val_array[red_val[i+9][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[29], led1_pwm_val_array[red_val[i+9][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[27], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[28], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[29], Led_on); 

    /* LED10 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[30], led1_pwm_val_array[red_val[i+10][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[31], led1_pwm_val_array[red_val[i+10][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[32], led1_pwm_val_array[red_val[i+10][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[30], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[31], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[32], Led_on); 

    /* LED11 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[33], led1_pwm_val_array[red_val[i+11][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[34], led1_pwm_val_array[red_val[i+11][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[35], led1_pwm_val_array[red_val[i+11][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[33], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[34], Led_off);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[35], Led_on); 
    i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
    }
#endif
fun_end2:
	g_switch_status = 0;		  
    return 0;
    }

void all_leds_white(void)
{
		int pwm_reg=0, pwm_val = 240;
		
			/* 全亮 */
		for (pwm_reg=1;pwm_reg<37;pwm_reg +=3){
			if (g_cmd_array[0] != g_cmd){
				goto fun_end3;
				}
		i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg-1], led1_pwm_val_array[255]);
		i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg], led1_pwm_val_array[255]);
		i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg+1], led1_pwm_val_array[255]);
		i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg-1], Led_on);
		i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg], Led_on);
		i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg+1], Led_on); 
	//	  i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
			}
		i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
		mdelay(1000);
fun_end3:
	g_switch_status = 0;

}

/* 音量加减 */
static void mod_pwm_val(void/*u8 rgb, u8 pwm_reg, u8 pwm_val, bool oo, u8 colour, u32 mtime*/)
{

	if (g_cmd == vol_one_step){ /* 音量第一级 */
        /* LED0 */
	if (g_cmd_array[0] != g_cmd){
		goto fun_end4;
		}
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[0], led1_pwm_val_array[200]);//G
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[1], led1_pwm_val_array[255]);//B
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[2], led1_pwm_val_array[200]);//R
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[0], Led_on);//G
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[1], Led_on);//B
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[2], Led_on); //R
        
        /* LED1 */
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[3], led1_pwm_val_array[200]);//G
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[4], led1_pwm_val_array[255]);//B
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[5], led1_pwm_val_array[200]);//R
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[3], Led_on);//G
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[4], Led_on);//B
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[5], Led_on); //R
        
        /* LED2 */
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[6], led1_pwm_val_array[200]);//G
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[7], led1_pwm_val_array[255]);//B
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[8], led1_pwm_val_array[200]);//R
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[6], Led_on);//G
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[7], Led_on);//B
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[8], Led_on); //R
        
		if (g_cmd_array[0] != g_cmd){
			goto fun_end4;
			}
        /* LED3 */
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[9], led1_pwm_val_array[200]);//G
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[10], led1_pwm_val_array[255]);//B
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[11], led1_pwm_val_array[200]);//R
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[9], Led_on);//G
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[10], Led_on);//B
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[11], Led_on); //R
        
        /* LED4 */
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[12], led1_pwm_val_array[200]);//G
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[13], led1_pwm_val_array[255]);//B
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[14], led1_pwm_val_array[200]);//R
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[12], Led_on);//G
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[13], Led_on);//B
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[14], Led_on); //R
        
        /* LED5 */
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[15], led1_pwm_val_array[200]);//G
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[16], led1_pwm_val_array[255]);//B
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[17], led1_pwm_val_array[200]);//R
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[15], Led_on);//G
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[16], Led_on);//B
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[17], Led_on); //R
        
		if (g_cmd_array[0] != g_cmd){
			goto fun_end4;
			}
        /* LED6 */
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[18], led1_pwm_val_array[200]);//G
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[19], led1_pwm_val_array[255]);//B
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[20], led1_pwm_val_array[200]);//R
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[18], Led_on);//G
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[19], Led_on);//B
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[20], Led_on); //R
        
        /* LED7 */
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[21], led1_pwm_val_array[200]);//G
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[22], led1_pwm_val_array[255]);//B
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[23], led1_pwm_val_array[200]);//R
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[21], Led_on);//G
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[22], Led_on);//B
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[23], Led_on); //R
        
        /* LED8 */
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[24], led1_pwm_val_array[200]);//G
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[25], led1_pwm_val_array[255]);//B
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[26], led1_pwm_val_array[200]);//R
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[24], Led_on);//G
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[25], Led_on);//B
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[26], Led_on); //R
        
		if (g_cmd_array[0] != g_cmd){
			goto fun_end4;
			}
        /* LED9 */
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[27], led1_pwm_val_array[200]);//G
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[28], led1_pwm_val_array[255]);//B
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[29], led1_pwm_val_array[200]);//R
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[27], Led_on);//G
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[28], Led_on);//B
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[29], Led_on); //R
        
        /* LED10 */
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[30], led1_pwm_val_array[200]);//G
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[31], led1_pwm_val_array[255]);//B
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[32], led1_pwm_val_array[200]);//R
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[30], Led_on);//G
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[31], Led_on);//B
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[32], Led_on); //R
        
        /* LED11 */
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[33], led1_pwm_val_array[200]);//G
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[34], led1_pwm_val_array[255]);//B
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[35], led1_pwm_val_array[200]);//R
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[33], Led_on);//G
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[34], Led_on);//B
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[35], Led_on); //R
        i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
        mdelay(1000);
	}else if (g_cmd == vol_two_step){ /* 音量第二级 */
		if (g_cmd_array[0] != g_cmd){
			goto fun_end4;
			}
			/* LED0 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[0], led1_pwm_val_array[140]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[1], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[2], led1_pwm_val_array[140]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[0], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[1], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[2], Led_on); //R
			
			/* LED1 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[3], led1_pwm_val_array[140]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[4], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[5], led1_pwm_val_array[140]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[3], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[4], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[5], Led_on); //R
			
			/* LED2 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[6], led1_pwm_val_array[140]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[7], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[8], led1_pwm_val_array[140]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[6], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[7], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[8], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED3 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[9], led1_pwm_val_array[140]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[10], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[11], led1_pwm_val_array[140]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[9], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[10], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[11], Led_on); //R
			
			/* LED4 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[12], led1_pwm_val_array[140]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[13], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[14], led1_pwm_val_array[140]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[12], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[13], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[14], Led_on); //R
			
			/* LED5 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[15], led1_pwm_val_array[140]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[16], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[17], led1_pwm_val_array[140]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[15], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[16], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[17], Led_on); //R
			
			/* LED6 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[18], led1_pwm_val_array[140]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[19], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[20], led1_pwm_val_array[140]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[18], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[19], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[20], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED7 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[21], led1_pwm_val_array[140]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[22], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[23], led1_pwm_val_array[140]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[21], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[22], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[23], Led_on); //R
			
			/* LED8 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[24], led1_pwm_val_array[140]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[25], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[26], led1_pwm_val_array[140]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[24], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[25], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[26], Led_on); //R
			
			/* LED9 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[27], led1_pwm_val_array[140]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[28], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[29], led1_pwm_val_array[140]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[27], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[28], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[29], Led_on); //R
			
			/* LED10 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[30], led1_pwm_val_array[140]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[31], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[32], led1_pwm_val_array[140]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[30], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[31], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[32], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED11 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[33], led1_pwm_val_array[140]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[34], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[35], led1_pwm_val_array[140]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[33], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[34], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[35], Led_on); //R
			i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
			mdelay(1000);
			}else if (g_cmd == vol_thr_step){/* 音量第三级 */
				if (g_cmd_array[0] != g_cmd){
					goto fun_end4;
					}
			/* LED0 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[0], led1_pwm_val_array[100]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[1], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[2], led1_pwm_val_array[100]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[0], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[1], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[2], Led_on); //R
			
			/* LED1 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[3], led1_pwm_val_array[100]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[4], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[5], led1_pwm_val_array[100]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[3], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[4], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[5], Led_on); //R
			
			/* LED2 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[6], led1_pwm_val_array[100]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[7], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[8], led1_pwm_val_array[100]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[6], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[7], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[8], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED3 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[9], led1_pwm_val_array[100]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[10], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[11], led1_pwm_val_array[100]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[9], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[10], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[11], Led_on); //R
			
			/* LED4 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[12], led1_pwm_val_array[100]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[13], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[14], led1_pwm_val_array[100]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[12], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[13], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[14], Led_on); //R
			
			/* LED5 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[15], led1_pwm_val_array[100]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[16], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[17], led1_pwm_val_array[100]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[15], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[16], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[17], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED6 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[18], led1_pwm_val_array[100]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[19], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[20], led1_pwm_val_array[100]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[18], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[19], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[20], Led_on); //R
			
			/* LED7 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[21], led1_pwm_val_array[100]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[22], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[23], led1_pwm_val_array[100]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[21], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[22], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[23], Led_on); //R
			
			/* LED8 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[24], led1_pwm_val_array[100]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[25], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[26], led1_pwm_val_array[100]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[24], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[25], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[26], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED9 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[27], led1_pwm_val_array[100]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[28], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[29], led1_pwm_val_array[100]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[27], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[28], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[29], Led_on); //R
			
			/* LED10 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[30], led1_pwm_val_array[100]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[31], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[32], led1_pwm_val_array[100]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[30], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[31], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[32], Led_on); //R
			
			/* LED11 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[33], led1_pwm_val_array[100]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[34], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[35], led1_pwm_val_array[100]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[33], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[34], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[35], Led_on); //R
			i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
			mdelay(1000);
			}else if (g_cmd == vol_fou_step){/* 音量第四级 */
				if (g_cmd_array[0] != g_cmd){
					goto fun_end4;
					}
			/* LED0 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[0], led1_pwm_val_array[60]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[1], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[2], led1_pwm_val_array[60]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[0], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[1], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[2], Led_on); //R
			
			/* LED1 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[3], led1_pwm_val_array[60]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[4], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[5], led1_pwm_val_array[60]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[3], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[4], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[5], Led_on); //R
			
			/* LED2 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[6], led1_pwm_val_array[60]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[7], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[8], led1_pwm_val_array[60]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[6], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[7], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[8], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED3 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[9], led1_pwm_val_array[60]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[10], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[11], led1_pwm_val_array[60]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[9], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[10], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[11], Led_on); //R
			
			/* LED4 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[12], led1_pwm_val_array[60]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[13], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[14], led1_pwm_val_array[60]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[12], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[13], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[14], Led_on); //R
			
			/* LED5 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[15], led1_pwm_val_array[60]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[16], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[17], led1_pwm_val_array[60]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[15], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[16], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[17], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED6 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[18], led1_pwm_val_array[60]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[19], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[20], led1_pwm_val_array[60]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[18], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[19], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[20], Led_on); //R
			
			/* LED7 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[21], led1_pwm_val_array[60]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[22], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[23], led1_pwm_val_array[60]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[21], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[22], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[23], Led_on); //R
			
			/* LED8 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[24], led1_pwm_val_array[60]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[25], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[26], led1_pwm_val_array[60]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[24], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[25], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[26], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED9 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[27], led1_pwm_val_array[60]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[28], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[29], led1_pwm_val_array[60]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[27], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[28], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[29], Led_on); //R
			
			/* LED10 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[30], led1_pwm_val_array[60]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[31], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[32], led1_pwm_val_array[60]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[30], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[31], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[32], Led_on); //R
			
			/* LED11 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[33], led1_pwm_val_array[60]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[34], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[35], led1_pwm_val_array[60]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[33], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[34], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[35], Led_on); //R
			i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
			mdelay(1000);
			}else if (g_cmd == vol_fiv_step){/* 音量第五级 */
				if (g_cmd_array[0] != g_cmd){
					goto fun_end4;
					}
			/* LED0 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[0], led1_pwm_val_array[50]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[1], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[2], led1_pwm_val_array[50]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[0], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[1], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[2], Led_on); //R
			
			/* LED1 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[3], led1_pwm_val_array[50]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[4], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[5], led1_pwm_val_array[50]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[3], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[4], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[5], Led_on); //R
			
			/* LED2 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[6], led1_pwm_val_array[50]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[7], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[8], led1_pwm_val_array[50]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[6], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[7], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[8], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED3 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[9], led1_pwm_val_array[50]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[10], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[11], led1_pwm_val_array[50]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[9], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[10], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[11], Led_on); //R
			
			/* LED4 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[12], led1_pwm_val_array[50]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[13], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[14], led1_pwm_val_array[50]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[12], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[13], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[14], Led_on); //R
			
			/* LED5 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[15], led1_pwm_val_array[50]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[16], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[17], led1_pwm_val_array[50]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[15], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[16], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[17], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED6 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[18], led1_pwm_val_array[50]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[19], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[20], led1_pwm_val_array[50]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[18], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[19], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[20], Led_on); //R
			
			/* LED7 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[21], led1_pwm_val_array[50]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[22], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[23], led1_pwm_val_array[50]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[21], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[22], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[23], Led_on); //R
			
			/* LED8 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[24], led1_pwm_val_array[50]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[25], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[26], led1_pwm_val_array[50]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[24], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[25], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[26], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED9 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[27], led1_pwm_val_array[50]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[28], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[29], led1_pwm_val_array[50]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[27], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[28], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[29], Led_on); //R
			
			/* LED10 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[30], led1_pwm_val_array[50]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[31], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[32], led1_pwm_val_array[50]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[30], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[31], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[32], Led_on); //R
			
			/* LED11 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[33], led1_pwm_val_array[50]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[34], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[35], led1_pwm_val_array[50]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[33], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[34], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[35], Led_on); //R
			i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
			mdelay(1000);
			}else if (g_cmd == vol_six_step){/* 音量第六级 */
				if (g_cmd_array[0] != g_cmd){
					goto fun_end4;
					}
			/* LED0 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[0], led1_pwm_val_array[40]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[1], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[2], led1_pwm_val_array[40]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[0], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[1], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[2], Led_on); //R
			
			/* LED1 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[3], led1_pwm_val_array[40]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[4], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[5], led1_pwm_val_array[40]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[3], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[4], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[5], Led_on); //R
			
			/* LED2 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[6], led1_pwm_val_array[40]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[7], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[8], led1_pwm_val_array[40]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[6], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[7], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[8], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED3 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[9], led1_pwm_val_array[40]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[10], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[11], led1_pwm_val_array[40]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[9], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[10], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[11], Led_on); //R
			
			/* LED4 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[12], led1_pwm_val_array[40]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[13], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[14], led1_pwm_val_array[40]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[12], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[13], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[14], Led_on); //R
			
			/* LED5 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[15], led1_pwm_val_array[40]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[16], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[17], led1_pwm_val_array[40]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[15], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[16], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[17], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED6 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[18], led1_pwm_val_array[40]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[19], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[20], led1_pwm_val_array[40]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[18], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[19], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[20], Led_on); //R
			
			/* LED7 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[21], led1_pwm_val_array[40]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[22], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[23], led1_pwm_val_array[40]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[21], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[22], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[23], Led_on); //R
			
			/* LED8 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[24], led1_pwm_val_array[40]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[25], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[26], led1_pwm_val_array[40]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[24], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[25], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[26], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED9 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[27], led1_pwm_val_array[40]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[28], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[29], led1_pwm_val_array[40]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[27], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[28], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[29], Led_on); //R
			
			/* LED10 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[30], led1_pwm_val_array[40]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[31], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[32], led1_pwm_val_array[40]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[30], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[31], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[32], Led_on); //R
			
			/* LED11 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[33], led1_pwm_val_array[40]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[34], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[35], led1_pwm_val_array[40]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[33], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[34], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[35], Led_on); //R
			i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
			mdelay(1000);
			}else if (g_cmd == vol_sev_step){/* 音量第七级 */
				if (g_cmd_array[0] != g_cmd){
					goto fun_end4;
					}
			/* LED0 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[0], led1_pwm_val_array[30]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[1], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[2], led1_pwm_val_array[30]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[0], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[1], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[2], Led_on); //R
			
			/* LED1 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[3], led1_pwm_val_array[30]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[4], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[5], led1_pwm_val_array[30]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[3], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[4], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[5], Led_on); //R
			
			/* LED2 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[6], led1_pwm_val_array[30]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[7], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[8], led1_pwm_val_array[30]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[6], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[7], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[8], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED3 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[9], led1_pwm_val_array[30]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[10], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[11], led1_pwm_val_array[30]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[9], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[10], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[11], Led_on); //R
			
			/* LED4 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[12], led1_pwm_val_array[30]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[13], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[14], led1_pwm_val_array[30]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[12], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[13], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[14], Led_on); //R
			
			/* LED5 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[15], led1_pwm_val_array[30]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[16], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[17], led1_pwm_val_array[30]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[15], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[16], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[17], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED6 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[18], led1_pwm_val_array[30]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[19], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[20], led1_pwm_val_array[30]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[18], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[19], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[20], Led_on); //R
			
			/* LED7 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[21], led1_pwm_val_array[30]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[22], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[23], led1_pwm_val_array[30]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[21], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[22], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[23], Led_on); //R
			
			/* LED8 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[24], led1_pwm_val_array[30]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[25], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[26], led1_pwm_val_array[30]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[24], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[25], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[26], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED9 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[27], led1_pwm_val_array[30]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[28], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[29], led1_pwm_val_array[30]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[27], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[28], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[29], Led_on); //R
			
			/* LED10 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[30], led1_pwm_val_array[30]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[31], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[32], led1_pwm_val_array[30]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[30], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[31], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[32], Led_on); //R
			
			/* LED11 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[33], led1_pwm_val_array[30]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[34], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[35], led1_pwm_val_array[30]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[33], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[34], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[35], Led_on); //R
			i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
			mdelay(1000);
			}else if (g_cmd == vol_eig_step){/* 音量第八级 */
				if (g_cmd_array[0] != g_cmd){
					goto fun_end4;
					}
			/* LED0 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[0], led1_pwm_val_array[20]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[1], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[2], led1_pwm_val_array[20]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[0], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[1], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[2], Led_on); //R
			
			/* LED1 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[3], led1_pwm_val_array[20]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[4], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[5], led1_pwm_val_array[20]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[3], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[4], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[5], Led_on); //R
			
			/* LED2 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[6], led1_pwm_val_array[20]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[7], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[8], led1_pwm_val_array[20]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[6], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[7], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[8], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED3 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[9], led1_pwm_val_array[20]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[10], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[11], led1_pwm_val_array[20]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[9], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[10], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[11], Led_on); //R
			
			/* LED4 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[12], led1_pwm_val_array[20]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[13], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[14], led1_pwm_val_array[20]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[12], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[13], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[14], Led_on); //R
			
			/* LED5 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[15], led1_pwm_val_array[20]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[16], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[17], led1_pwm_val_array[20]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[15], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[16], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[17], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED6 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[18], led1_pwm_val_array[20]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[19], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[20], led1_pwm_val_array[20]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[18], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[19], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[20], Led_on); //R
			
			/* LED7 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[21], led1_pwm_val_array[20]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[22], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[23], led1_pwm_val_array[20]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[21], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[22], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[23], Led_on); //R
			
			/* LED8 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[24], led1_pwm_val_array[20]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[25], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[26], led1_pwm_val_array[20]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[24], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[25], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[26], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED9 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[27], led1_pwm_val_array[20]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[28], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[29], led1_pwm_val_array[20]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[27], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[28], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[29], Led_on); //R
			
			/* LED10 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[30], led1_pwm_val_array[20]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[31], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[32], led1_pwm_val_array[20]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[30], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[31], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[32], Led_on); //R
			
			/* LED11 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[33], led1_pwm_val_array[20]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[34], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[35], led1_pwm_val_array[20]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[33], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[34], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[35], Led_on); //R
			i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
			mdelay(1000);
			}else if (g_cmd == vol_nin_step){/* 音量第九级 */
				if (g_cmd_array[0] != g_cmd){
					goto fun_end4;
					}
			/* LED0 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[0], led1_pwm_val_array[10]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[1], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[2], led1_pwm_val_array[10]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[0], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[1], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[2], Led_on); //R
			
			/* LED1 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[3], led1_pwm_val_array[10]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[4], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[5], led1_pwm_val_array[10]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[3], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[4], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[5], Led_on); //R
			
			/* LED2 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[6], led1_pwm_val_array[10]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[7], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[8], led1_pwm_val_array[10]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[6], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[7], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[8], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED3 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[9], led1_pwm_val_array[10]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[10], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[11], led1_pwm_val_array[10]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[9], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[10], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[11], Led_on); //R
			
			/* LED4 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[12], led1_pwm_val_array[10]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[13], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[14], led1_pwm_val_array[10]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[12], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[13], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[14], Led_on); //R
			
			/* LED5 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[15], led1_pwm_val_array[10]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[16], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[17], led1_pwm_val_array[10]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[15], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[16], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[17], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED6 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[18], led1_pwm_val_array[10]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[19], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[20], led1_pwm_val_array[10]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[18], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[19], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[20], Led_on); //R
			
			/* LED7 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[21], led1_pwm_val_array[10]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[22], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[23], led1_pwm_val_array[10]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[21], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[22], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[23], Led_on); //R
			
			/* LED8 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[24], led1_pwm_val_array[10]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[25], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[26], led1_pwm_val_array[10]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[24], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[25], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[26], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED9 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[27], led1_pwm_val_array[10]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[28], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[29], led1_pwm_val_array[10]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[27], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[28], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[29], Led_on); //R
			
			/* LED10 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[30], led1_pwm_val_array[10]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[31], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[32], led1_pwm_val_array[10]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[30], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[31], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[32], Led_on); //R
			
			/* LED11 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[33], led1_pwm_val_array[10]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[34], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[35], led1_pwm_val_array[10]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[33], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[34], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[35], Led_on); //R
			i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
			mdelay(1000);
			}else if (g_cmd == vol_ten_step){/* 音量第十级 */
				if (g_cmd_array[0] != g_cmd){
					goto fun_end4;
					}
			/* LED0 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[0], led1_pwm_val_array[0]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[1], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[2], led1_pwm_val_array[0]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[0], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[1], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[2], Led_on); //R
			
			/* LED1 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[3], led1_pwm_val_array[0]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[4], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[5], led1_pwm_val_array[0]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[3], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[4], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[5], Led_on); //R
			
			/* LED2 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[6], led1_pwm_val_array[0]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[7], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[8], led1_pwm_val_array[0]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[6], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[7], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[8], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED3 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[9], led1_pwm_val_array[0]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[10], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[11], led1_pwm_val_array[0]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[9], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[10], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[11], Led_on); //R
			
			/* LED4 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[12], led1_pwm_val_array[0]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[13], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[14], led1_pwm_val_array[0]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[12], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[13], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[14], Led_on); //R
			
			/* LED5 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[15], led1_pwm_val_array[0]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[16], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[17], led1_pwm_val_array[0]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[15], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[16], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[17], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED6 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[18], led1_pwm_val_array[0]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[19], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[20], led1_pwm_val_array[0]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[18], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[19], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[20], Led_on); //R
			
			/* LED7 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[21], led1_pwm_val_array[0]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[22], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[23], led1_pwm_val_array[0]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[21], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[22], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[23], Led_on); //R
			
			/* LED8 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[24], led1_pwm_val_array[0]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[25], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[26], led1_pwm_val_array[0]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[24], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[25], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[26], Led_on); //R
			
			if (g_cmd_array[0] != g_cmd){
				goto fun_end4;
				}
			/* LED9 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[27], led1_pwm_val_array[0]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[28], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[29], led1_pwm_val_array[0]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[27], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[28], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[29], Led_on); //R
			
			/* LED10 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[30], led1_pwm_val_array[0]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[31], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[32], led1_pwm_val_array[0]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[30], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[31], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[32], Led_on); //R
			
			/* LED11 */
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[33], led1_pwm_val_array[0]);//G
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[34], led1_pwm_val_array[255]);//B
			i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[35], led1_pwm_val_array[0]);//R
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[33], Led_on);//G
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[34], Led_on);//B
			i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[35], Led_on); //R
			i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
			mdelay(1000);
		}
        
fun_end4:
	g_switch_status = 0;
}

/* ********************************************************
 * 函数功能: 以任意颜色???任意pwm占空???(0x00~0xff)点亮任意???个LED???
 * pwm_reg   : 要点亮哪???个LED???
 * pwm_val : PWM占空???
 * oo    : 关闭或点???
 * colour : 点亮颜色
 * mtime : 点亮???个LED后延时多少毫秒再点亮下一个LED灯或者持续点亮多长时???
 * ********************************************************/
static int light_one_led( u8 pwm_reg, u8 pwm_val,
                            bool oo, u8 colour, u32 mtime )
{
    if (colour == 0){   /* 绿色 */
		if (g_cmd_array[0] != g_cmd){
			goto fun_end1;
			}
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg], led1_pwm_val_array[pwm_val]);
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg+1], 0);
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg+2], 0);
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg], oo);
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg+1], Led_off); /* 关闭红色绿色 */
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg+2], Led_off);
        i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
        mdelay(mtime);
        }else if (colour == 1){ /* 蓝色 */
			if (g_cmd_array[0] != g_cmd){
				goto fun_end1;
				}
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg-1], 0);
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg], led1_pwm_val_array[pwm_val]);
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg+1], 0);
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg-1], Led_off); 
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg], oo);
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg+1], Led_off);
       i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
        mdelay(mtime);
        }else if (colour == 2){  /* 红色 */
			if (g_cmd_array[0] != g_cmd){
				goto fun_end1;
				}
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg-2], 0);
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg-1],0);
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg], led1_pwm_val_array[pwm_val]);
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg-2], Led_off); 
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg-1], Led_off);
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg], oo);
        i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
        mdelay(mtime);
        }else if (colour == 3){  /* 白色 */
			if (g_cmd_array[0] != g_cmd){
				goto fun_end1;
				}
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg-2], led1_pwm_val_array[pwm_val]);
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg-1], led1_pwm_val_array[pwm_val]);
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg-0], led1_pwm_val_array[pwm_val]);
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg-2], Led_on);
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg-1], Led_on);
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg-0], Led_on); 
        i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
        mdelay(mtime);
        }else if (colour == 4){   /* 红色和绿色合成黄??? pwm_reg选择led2 */
			if (g_cmd_array[0] != g_cmd){
				goto fun_end1;
				}
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg-2], led1_pwm_val_array[pwm_val]);
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg-1], 0);
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg], led1_pwm_val_array[pwm_val]);
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg-2], Led_on);  
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg-1], Led_off); 
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg], Led_on);/* 关闭蓝色 */
        i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
        mdelay(mtime);
        }else if (colour == 5){   /* 蓝色和绿色合成青??? pwm_reg选择led2 */
			if (g_cmd_array[0] != g_cmd){
				goto fun_end1;
				}
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg-2], led1_pwm_val_array[pwm_val]);
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg-1], led1_pwm_val_array[pwm_val]);
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg], 0);
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg-2], Led_on);  
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg-1], Led_on); 
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg], Led_off);
        i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
        mdelay(mtime);
          }else if (colour == 6){   /* 蓝色和红色合成品??? pwm_reg选择led2*/
			  if (g_cmd_array[0] != g_cmd){
				  goto fun_end1;
				  }
          i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg-2], 0);
          i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg-1], led1_pwm_val_array[pwm_val]);
          i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg], led1_pwm_val_array[pwm_val]);
          i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg-2], Led_off);  
          i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg-1], Led_on); 
          i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg], Led_on);
          i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
          mdelay(mtime);
        }else if (colour == 7){   /* ??? RGB=250/140/53 pwm_reg选择led2*/
			if (g_cmd_array[0] != g_cmd){
				goto fun_end1;
				}
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg-2], led1_pwm_val_array[40]);//G
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg-1], led1_pwm_val_array[0]);//B
        i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg], led1_pwm_val_array[250]);//R
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg-2], Led_on);  
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg-1], Led_on); 
        i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg], Led_on);
        i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
        mdelay(mtime);
          }else if (colour == 8){   /* 关闭???有LED*/
			  if (g_cmd_array[0] != g_cmd){
				  goto fun_end1;
				  }
          i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg-2], 0);//G
          i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg-1], 0);//B
          i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg], 0);//R
          i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg-2], Led_off);  
          i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg-1], Led_off); 
          i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg], Led_off);
          i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
          mdelay(mtime);
          }else if (colour == 9){   /* 淡蓝色，RGB值135,206,250*/
			  if (g_cmd_array[0] != g_cmd){
				  goto fun_end1;
				  }
          i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg-2], 206);//G
          i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg-1], 250);//B
          i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[pwm_reg], 135);//R
          i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg-2], Led_on);  
          i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg-1], Led_on); 
          i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[pwm_reg], Led_on);
          i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
          mdelay(mtime);
        }
fun_end1:
  g_switch_status = 0;			
    return 0;
}

static long led_ioctl(struct file *file, unsigned int cmd,
			    unsigned long arg)
{
	if (!g_num){
    	g_cmd = cmd;
		g_num =1;
	}

	if (g_num){
		g_cmd_array[0] = cmd;
		}
	LED_DBG("led_ioctl, cmd = %d\n", cmd);
	    
    return 0;
}

static int white_round_val_fun(void)
{
    int i;  
#if 1
    for (i=0;i<156;i +=12){
		if (g_cmd_array[0] != g_cmd){
			goto fun_end2;
			}
//    mdelay(200);
    
    /* LED0 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[0], led1_pwm_val_array[white_round_val[i][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[1], led1_pwm_val_array[white_round_val[i][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[2], led1_pwm_val_array[white_round_val[i][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[0], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[1], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[2], Led_on); 

    /* LED1 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[3], led1_pwm_val_array[white_round_val[i+1][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[4], led1_pwm_val_array[white_round_val[i+1][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[5], led1_pwm_val_array[white_round_val[i+1][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[3], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[4], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[5], Led_on); 
    
    /* LED2 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[6], led1_pwm_val_array[white_round_val[i+2][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[7], led1_pwm_val_array[white_round_val[i+2][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[8], led1_pwm_val_array[white_round_val[i+2][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[6], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[7], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[8], Led_on); 

    /* LED3 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[9], led1_pwm_val_array[white_round_val[i+3][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[10], led1_pwm_val_array[white_round_val[i+3][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[11], led1_pwm_val_array[white_round_val[i+3][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[9], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[10], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[11], Led_on); 

     /* LED4 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[12], led1_pwm_val_array[white_round_val[i+4][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[13], led1_pwm_val_array[white_round_val[i+4][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[14], led1_pwm_val_array[white_round_val[i+4][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[12], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[13], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[14], Led_on); 

    /* LED5 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[15], led1_pwm_val_array[white_round_val[i+5][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[16], led1_pwm_val_array[white_round_val[i+5][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[17], led1_pwm_val_array[white_round_val[i+5][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[15], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[16], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[17], Led_on); 

    /* LED6 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[18], led1_pwm_val_array[white_round_val[i+6][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[19], led1_pwm_val_array[white_round_val[i+6][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[20], led1_pwm_val_array[white_round_val[i+6][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[18], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[19], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[20], Led_on); 

    /* LED7 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[21], led1_pwm_val_array[white_round_val[i+7][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[22], led1_pwm_val_array[white_round_val[i+7][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[23], led1_pwm_val_array[white_round_val[i+7][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[21], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[22], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[23], Led_on); 

    /* LED8 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[24], led1_pwm_val_array[white_round_val[i+8][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[25], led1_pwm_val_array[white_round_val[i+8][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[26], led1_pwm_val_array[white_round_val[i+8][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[24], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[25], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[26], Led_on); 

    /* LED9 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[27], led1_pwm_val_array[white_round_val[i+9][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[28], led1_pwm_val_array[white_round_val[i+9][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[29], led1_pwm_val_array[white_round_val[i+9][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[27], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[28], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[29], Led_on); 

    /* LED10 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[30], led1_pwm_val_array[white_round_val[i+10][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[31], led1_pwm_val_array[white_round_val[i+10][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[32], led1_pwm_val_array[white_round_val[i+10][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[30], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[31], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[32], Led_on); 

    /* LED11 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[33], led1_pwm_val_array[white_round_val[i+11][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[34], led1_pwm_val_array[white_round_val[i+11][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[35], led1_pwm_val_array[white_round_val[i+11][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[33], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[34], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[35], Led_on); 
    i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
    }
#endif
fun_end2:
	g_switch_status = 0;		  
    return 0;
    }
				

static int little_blue_round_val_fun(void)
{
    int i;  
#if 1
    for (i=0;i<156;i +=12){
		if (g_cmd_array[0] != g_cmd){
			goto fun_end2;
			}
//    mdelay(200);
    
    /* LED0 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[0], led1_pwm_val_array[little_blue_round_val[i][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[1], led1_pwm_val_array[little_blue_round_val[i][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[2], led1_pwm_val_array[little_blue_round_val[i][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[0], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[1], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[2], Led_on); 

    /* LED1 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[3], led1_pwm_val_array[little_blue_round_val[i+1][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[4], led1_pwm_val_array[little_blue_round_val[i+1][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[5], led1_pwm_val_array[little_blue_round_val[i+1][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[3], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[4], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[5], Led_on); 
    
    /* LED2 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[6], led1_pwm_val_array[little_blue_round_val[i+2][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[7], led1_pwm_val_array[little_blue_round_val[i+2][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[8], led1_pwm_val_array[little_blue_round_val[i+2][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[6], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[7], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[8], Led_on); 

    /* LED3 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[9], led1_pwm_val_array[little_blue_round_val[i+3][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[10], led1_pwm_val_array[little_blue_round_val[i+3][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[11], led1_pwm_val_array[little_blue_round_val[i+3][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[9], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[10], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[11], Led_on); 

     /* LED4 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[12], led1_pwm_val_array[little_blue_round_val[i+4][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[13], led1_pwm_val_array[little_blue_round_val[i+4][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[14], led1_pwm_val_array[little_blue_round_val[i+4][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[12], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[13], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[14], Led_on); 

    /* LED5 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[15], led1_pwm_val_array[little_blue_round_val[i+5][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[16], led1_pwm_val_array[little_blue_round_val[i+5][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[17], led1_pwm_val_array[little_blue_round_val[i+5][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[15], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[16], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[17], Led_on); 

    /* LED6 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[18], led1_pwm_val_array[little_blue_round_val[i+6][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[19], led1_pwm_val_array[little_blue_round_val[i+6][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[20], led1_pwm_val_array[little_blue_round_val[i+6][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[18], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[19], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[20], Led_on); 

    /* LED7 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[21], led1_pwm_val_array[little_blue_round_val[i+7][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[22], led1_pwm_val_array[little_blue_round_val[i+7][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[23], led1_pwm_val_array[little_blue_round_val[i+7][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[21], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[22], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[23], Led_on); 

    /* LED8 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[24], led1_pwm_val_array[little_blue_round_val[i+8][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[25], led1_pwm_val_array[little_blue_round_val[i+8][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[26], led1_pwm_val_array[little_blue_round_val[i+8][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[24], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[25], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[26], Led_on); 

    /* LED9 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[27], led1_pwm_val_array[little_blue_round_val[i+9][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[28], led1_pwm_val_array[little_blue_round_val[i+9][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[29], led1_pwm_val_array[little_blue_round_val[i+9][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[27], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[28], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[29], Led_on); 

    /* LED10 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[30], led1_pwm_val_array[little_blue_round_val[i+10][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[31], led1_pwm_val_array[little_blue_round_val[i+10][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[32], led1_pwm_val_array[little_blue_round_val[i+10][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[30], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[31], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[32], Led_on); 

    /* LED11 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[33], led1_pwm_val_array[little_blue_round_val[i+11][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[34], led1_pwm_val_array[little_blue_round_val[i+11][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[35], led1_pwm_val_array[little_blue_round_val[i+11][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[33], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[34], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[35], Led_on); 
    i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
    }
#endif
fun_end2:
	g_switch_status = 0;		  
    return 0;
    }

static int blue_round_val_fun(void)
{
    int i;  
#if 1
    for (i=0;i<156;i +=12){
		if (g_cmd_array[0] != g_cmd){
			goto fun_end2;
			}
//    mdelay(200);
    
    /* LED0 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[0], led1_pwm_val_array[blue_round_val[i][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[1], led1_pwm_val_array[blue_round_val[i][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[2], led1_pwm_val_array[blue_round_val[i][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[0], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[1], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[2], Led_on); 

    /* LED1 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[3], led1_pwm_val_array[blue_round_val[i+1][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[4], led1_pwm_val_array[blue_round_val[i+1][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[5], led1_pwm_val_array[blue_round_val[i+1][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[3], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[4], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[5], Led_on); 
    
    /* LED2 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[6], led1_pwm_val_array[blue_round_val[i+2][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[7], led1_pwm_val_array[blue_round_val[i+2][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[8], led1_pwm_val_array[blue_round_val[i+2][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[6], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[7], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[8], Led_on); 

    /* LED3 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[9], led1_pwm_val_array[blue_round_val[i+3][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[10], led1_pwm_val_array[blue_round_val[i+3][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[11], led1_pwm_val_array[blue_round_val[i+3][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[9], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[10], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[11], Led_on); 

     /* LED4 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[12], led1_pwm_val_array[blue_round_val[i+4][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[13], led1_pwm_val_array[blue_round_val[i+4][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[14], led1_pwm_val_array[blue_round_val[i+4][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[12], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[13], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[14], Led_on); 

    /* LED5 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[15], led1_pwm_val_array[blue_round_val[i+5][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[16], led1_pwm_val_array[blue_round_val[i+5][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[17], led1_pwm_val_array[blue_round_val[i+5][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[15], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[16], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[17], Led_on); 

    /* LED6 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[18], led1_pwm_val_array[blue_round_val[i+6][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[19], led1_pwm_val_array[blue_round_val[i+6][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[20], led1_pwm_val_array[blue_round_val[i+6][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[18], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[19], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[20], Led_on); 

    /* LED7 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[21], led1_pwm_val_array[blue_round_val[i+7][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[22], led1_pwm_val_array[blue_round_val[i+7][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[23], led1_pwm_val_array[blue_round_val[i+7][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[21], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[22], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[23], Led_on); 

    /* LED8 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[24], led1_pwm_val_array[blue_round_val[i+8][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[25], led1_pwm_val_array[blue_round_val[i+8][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[26], led1_pwm_val_array[blue_round_val[i+8][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[24], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[25], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[26], Led_on); 

    /* LED9 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[27], led1_pwm_val_array[blue_round_val[i+9][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[28], led1_pwm_val_array[blue_round_val[i+9][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[29], led1_pwm_val_array[blue_round_val[i+9][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[27], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[28], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[29], Led_on); 

    /* LED10 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[30], led1_pwm_val_array[blue_round_val[i+10][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[31], led1_pwm_val_array[blue_round_val[i+10][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[32], led1_pwm_val_array[blue_round_val[i+10][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[30], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[31], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[32], Led_on); 

    /* LED11 */
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[33], led1_pwm_val_array[blue_round_val[i+11][0]]);//G
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[34], led1_pwm_val_array[blue_round_val[i+11][1]]);//B
    i2c_smbus_write_byte_data(led1_client, led1_pwm_reg_array[35], led1_pwm_val_array[blue_round_val[i+11][2]]);//R
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[33], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[34], Led_on);
    i2c_smbus_write_byte_data(led1_client, led1_ctrl_reg_array[35], Led_on); 
    i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
    }
#endif
fun_end2:
	g_switch_status = 0;		  
    return 0;
    }


ssize_t leds_write (struct file *file, const char __user *buf, size_t count, loff_t *off)
{
   int k=0, i=10;
   
   LED_DBG("%s, g_cmd = %d\n", __FUNCTION__, g_cmd);
   while(1){
      set_current_state(TASK_UNINTERRUPTIBLE); /* 将当前的状???表示设置为休眠 */
        if (kthread_should_stop()) 
            break;   
        g_cmd = g_cmd_array[0];
//		LED_DBG("%s, %d, g_cmd = %d\n", __FUNCTION__, __LINE__,  g_cmd);
        switch (g_cmd)
        {
			case start_up :/* 橙色旋转 */
			do {				  
					for(i=1;i<36;i += 3){  //蓝色
					light_one_led( i, 255, 1, 1, 0);
					}
					i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
			}while((g_cmd == start_up) & (g_switch_status));
			break;
			
			case all_led_red :/* 红色旋转 */
			do {				 
				red_color_fun();
				i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
			}while((g_cmd == all_led_red) & (g_switch_status));
			break;
#if 1 
            case re_orange :/* 橙色旋转 */
            {
             do {                
                for(i=2;i<36;i += 3){  //橙色
                 light_one_led(i, 255, 1, 7, 0);
				 if (g_cmd_array[0] != g_cmd){
					 goto fun_end20;
					 }
                }
                
                 for(i=2;i<36;i += 3){  //白色旋转
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end20;
						 }
                 light_one_led( i, 255, 1, 3, 100);
                 if (i == 5){
                     light_one_led( 2, 255, 1, 7, 0);
                    }
                 if (i == 8){
                     light_one_led( 5, 255, 1, 7, 0);
                    }
                 if (i == 11){
                     light_one_led( 8, 255, 1, 7, 0);
                    }
                 if (i == 14){
                     light_one_led( 11, 255, 1, 7, 0);
                    }
                 if (i == 17){
                     light_one_led( 14, 255, 1, 7, 0);
                    }
                 if (i == 20){
                     light_one_led( 17, 255, 1, 7, 0);
                    }
                 if (i == 23){
                     light_one_led( 20, 255, 1, 7, 0);
                    }
                 if (i == 26){
                     light_one_led( 23, 255, 1, 7, 0);
                    }
                 if (i == 29){
                     light_one_led( 26, 255, 1, 7, 0);
                    }
                 if (i == 32){
                     light_one_led(29, 255, 1, 7, 0);
                    }
                 if (i == 35){
                     light_one_led( 32, 255, 1, 7, 0);
                    }
                 }
//                mdelay(1000);
fun_end20:
	g_switch_status = 0;

                 }while((g_cmd == re_orange) & (g_switch_status));
            }
         break;
            
            case orange_to_red :/* 橙色转红??? */
            {
             do {     
                for (k=0;k<2;k++){
                    for(i=2;i<36;i += 3){  //橙色
						if (g_cmd_array[0] != g_cmd){
							goto fun_end21;
							}
                     light_one_led( i, 255, 1, 7, 0);
                    }
                    
                     for(i=2;i<36;i += 3){  //白色旋转
						 if (g_cmd_array[0] != g_cmd){
							 goto fun_end21;
							 }
                     light_one_led( i, 255, 1, 3, 50);
                     if (i == 5){
                         light_one_led( 2, 255, 1, 7, 0);
                        }
                     if (i == 8){
                         light_one_led( 5, 255, 1, 7, 0);
                        }
                     if (i == 11){
                         light_one_led( 8, 255, 1, 7, 0);
                        }
                     if (i == 14){
                         light_one_led( 11, 255, 1, 7, 0);
                        }
                     if (i == 17){
                         light_one_led( 14, 255, 1, 7, 0);
                        }
                     if (i == 20){
                         light_one_led( 17, 255, 1, 7, 0);
                        }
                     if (i == 23){
                         light_one_led( 20, 255, 1, 7, 0);
                        }
                     if (i == 26){
                         light_one_led( 23, 255, 1, 7, 0);
                        }
                     if (i == 29){
                         light_one_led( 26, 255, 1, 7, 0);
                        }
                     if (i == 32){
                         light_one_led(29, 255, 1, 7, 0);
                        }
                     if (i == 35){
                         light_one_led( 32, 255, 1, 7, 0);
                        }
                     }
                  }

                for (k=0;k<2;k++){
                    for(i=2;i<36;i += 3){  //红色
						if (g_cmd_array[0] != g_cmd){
							goto fun_end21;
							}
                    light_one_led( i, 255, 1, 2, 0);
                    }
                    
                     for(i=2;i<36;i += 3){  //白色旋转
						 if (g_cmd_array[0] != g_cmd){
							 goto fun_end21;
							 }
                     light_one_led( i, 255, 1, 3, 50);
                     if (i == 5){
                         light_one_led( 2, 255, 1, 7, 0);
                        }
                     if (i == 8){
                         light_one_led( 5, 255, 1, 7, 0);
                        }
                     if (i == 11){
                         light_one_led( 8, 255, 1, 7, 0);
                        }
                     if (i == 14){
                         light_one_led( 11, 255, 1, 7, 0);
                        }
                     if (i == 17){
                         light_one_led( 14, 255, 1, 7, 0);
                        }
                     if (i == 20){
                         light_one_led( 17, 255, 1, 7, 0);
                        }
                     if (i == 23){
                         light_one_led( 20, 255, 1, 7, 0);
                        }
                     if (i == 26){
                         light_one_led( 23, 255, 1, 7, 0);
                        }
                     if (i == 29){
                         light_one_led( 26, 255, 1, 7, 0);
                        }
                     if (i == 32){
                         light_one_led(29, 255, 1, 7, 0);
                        }
                     if (i == 35){
                         light_one_led( 32, 255, 1, 7, 0);
                        }
                     }
                  }
fun_end21:
	g_switch_status = 0;
                }while((g_cmd == orange_to_red) & (g_switch_status));
            }
         break;
            
            case orange_to_green :/* 橙色转绿??? */
            {
             do {     
                for (k=0;k<2;k++){
                    for(i=2;i<36;i += 3){  //橙色
						if (g_cmd_array[0] != g_cmd){
							goto fun_end22;
							}
                     light_one_led( i, 255, 1, 7, 0);
                    }
                    
                     for(i=2;i<36;i += 3){  //白色旋转
						 if (g_cmd_array[0] != g_cmd){
							 goto fun_end22;
							 }
                     light_one_led( i, 255, 1, 3, 50);
                     if (i == 5){
                         light_one_led( 2, 255, 1, 7, 0);
                        }
                     if (i == 8){
                         light_one_led( 5, 255, 1, 7, 0);
                        }
                     if (i == 11){
                         light_one_led( 8, 255, 1, 7, 0);
                        }
                     if (i == 14){
                         light_one_led( 11, 255, 1, 7, 0);
                        }
                     if (i == 17){
                         light_one_led( 14, 255, 1, 7, 0);
                        }
                     if (i == 20){
                         light_one_led( 17, 255, 1, 7, 0);
                        }
                     if (i == 23){
                         light_one_led( 20, 255, 1, 7, 0);
                        }
                     if (i == 26){
                         light_one_led( 23, 255, 1, 7, 0);
                        }
                     if (i == 29){
                         light_one_led( 26, 255, 1, 7, 0);
                        }
                     if (i == 32){
                         light_one_led(29, 255, 1, 7, 0);
                        }
                     if (i == 35){
                         light_one_led( 32, 255, 1, 7, 0);
                        }
                     }
                  }

                for (k=0;k<2;k++){
                    for(i=0;i<36;i += 3){  //绿色
						if (g_cmd_array[0] != g_cmd){
							goto fun_end22;
							}
                    light_one_led( i, 255, 1, 0, 0);
                    }
                    
                     for(i=2;i<36;i += 3){  //白色旋转
						 if (g_cmd_array[0] != g_cmd){
							 goto fun_end22;
							 }
                     light_one_led( i, 255, 1, 3, 50);
                     if (i == 5){
                         light_one_led( 2, 255, 1, 7, 0);
                        }
                     if (i == 8){
                         light_one_led( 5, 255, 1, 7, 0);
                        }
                     if (i == 11){
                         light_one_led( 8, 255, 1, 7, 0);
                        }
                     if (i == 14){
                         light_one_led( 11, 255, 1, 7, 0);
                        }
                     if (i == 17){
                         light_one_led( 14, 255, 1, 7, 0);
                        }
                     if (i == 20){
                         light_one_led( 17, 255, 1, 7, 0);
                        }
                     if (i == 23){
                         light_one_led( 20, 255, 1, 7, 0);
                        }
                     if (i == 26){
                         light_one_led( 23, 255, 1, 7, 0);
                        }
                     if (i == 29){
                         light_one_led( 26, 255, 1, 7, 0);
                        }
                     if (i == 32){
                         light_one_led(29, 255, 1, 7, 0);
                        }
                     if (i == 35){
                         light_one_led( 32, 255, 1, 7, 0);
                        }
                     }
                  }
fun_end22:
	g_switch_status = 0;
                }while((g_cmd == orange_to_green) & (g_switch_status));
            }
         break;

            case red_fast_turn:/* 红快??? */
            {
             do {     
                 for(i=2;i<36;i += 3){  //浅红???
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end23;
						 }
                  light_one_led( i, 100, 1, 2, 0);
                 }
                 mdelay(200);
                 
                    for(i=2;i<36;i += 3){  //红色
						if (g_cmd_array[0] != g_cmd){
							goto fun_end23;
							}
                     light_one_led( i, 255, 1, 2, 0);
                    }
                    mdelay(300);
                    for(i=2;i<36;i += 3){  //???
						if (g_cmd_array[0] != g_cmd){
							goto fun_end23;
							}
                     light_one_led( i, 255, 1, 8, 0);
                    }
                    mdelay(300);
fun_end23:
	g_switch_status = 0;
                }while((g_cmd == red_fast_turn) & (g_switch_status));
            }
         break;
                
            case red_2_times_turn:/* ???2倍快??? */
            {
             do {     
                 for(i=2;i<36;i += 3){  //红色
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end24;
						 }
                  light_one_led( i, 255, 1, 2, 0);
                 }
                 mdelay(400);
                 
                 for(i=2;i<36;i += 3){  //???
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end24;
						 }
                  light_one_led( i, 255, 1, 8, 0);
                 }
                 mdelay(400);
/* =================================================== */                 
                 for(i=2;i<36;i += 3){  //红色
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end24;
						 }
                  light_one_led( i, 255, 1, 2, 0);
                 }
                 mdelay(400);
                 
                 for(i=2;i<36;i += 3){  //???
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end24;
						 }
                  light_one_led( i, 255, 1, 8, 0);
                 }
                 mdelay(400);
 /* =================================================== */                 
                 
                 for(i=2;i<36;i += 3){  //红色
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end24;
						 }
                  light_one_led( i, 255, 1, 2, 0);
                 }
                 mdelay(400);
                 
                 for(i=2;i<36;i += 3){  //浅红???
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end24;
						 }
                  light_one_led( i, 100, 1, 2, 0);
                 }
                 mdelay(300);
 /* =================================================== */                 
                  for(i=2;i<36;i += 3){  //红色
					  if (g_cmd_array[0] != g_cmd){
						  goto fun_end24;
						  }
                   light_one_led( i, 255, 1, 2, 0);
                  }
                  mdelay(400);
                  
                  for(i=2;i<36;i += 3){  //???
					  if (g_cmd_array[0] != g_cmd){
						  goto fun_end24;
						  }
                   light_one_led( i, 255, 1, 8, 0);
                  }
                  mdelay(400);
fun_end24:
	g_switch_status = 0;
                }while((g_cmd == red_2_times_turn) & (g_switch_status));
            }
         break;
            
            case red_slow_turn:
            {
             do {   
                
                 for(i=2;i<36;i += 3){  //???
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end25;
						 }
                  light_one_led( i, 255, 1, 8, 0);
                 }
                 mdelay(200);
                 
                 for(i=2;i<36;i += 3){  //浅红???
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end25;
						 }
                  light_one_led( i, 50, 1, 2, 0);
                 }
                 mdelay(200);
                 
                for(i=2;i<36;i += 3){  
					if (g_cmd_array[0] != g_cmd){
						goto fun_end25;
						}
                 light_one_led( i, 100, 1, 2, 0);
                }
                mdelay(200);
                
                 for(i=2;i<36;i += 3){  
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end25;
						 }
                  light_one_led( i, 150, 1, 2, 0);
                 }
                 mdelay(200);
                 
                for(i=2;i<36;i += 3){  
					if (g_cmd_array[0] != g_cmd){
						goto fun_end25;
						}
                 light_one_led( i, 200, 1, 2, 0);
                }
                mdelay(200);
                
                for(i=2;i<36;i += 3){  //???亮红???
					if (g_cmd_array[0] != g_cmd){
						goto fun_end25;
						}
                 light_one_led( i, 256, 1, 2, 0);
                }
//                mdelay(200);
/* =================================================== */                                 
                for(i=2;i<36;i += 3){  
					if (g_cmd_array[0] != g_cmd){
						goto fun_end25;
						}
                 light_one_led( i, 200, 1, 2, 0);
                }
                mdelay(200);
                
                for(i=2;i<36;i += 3){  
					if (g_cmd_array[0] != g_cmd){
						goto fun_end25;
						}
                 light_one_led( i, 150, 1, 2, 0);
                }
                mdelay(200);
                
                for(i=2;i<36;i += 3){  
					if (g_cmd_array[0] != g_cmd){
						goto fun_end25;
						}
                 light_one_led( i, 100, 1, 2, 0);
                }
                mdelay(200);
                
                for(i=2;i<36;i += 3){  //浅红???
					if (g_cmd_array[0] != g_cmd){
						goto fun_end25;
						}
                 light_one_led( i, 50, 1, 2, 0);
                }
fun_end25:
	g_switch_status = 0;
                mdelay(200);
                }while((g_cmd == red_slow_turn) & (g_switch_status));
            }
         break;

            case progress_bar:
            {
             do {     
                 for(i=2;i<36;i += 3){  //黄色
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end26;
						 }
                  light_one_led( i, 100, 1,4, 100);
                 }
//                 mdelay(200);
                 
                 for(i=2;i<36;i += 3){  //关闭???有LED???
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end26;
						 }
                  light_one_led( i, 100, 1,8, 0);
                 }
fun_end26:
	g_switch_status = 0;
                 mdelay(200);
                }while((g_cmd == progress_bar) & (g_switch_status));
            }
         break;
            
            case on_and_listening:/* ???+??? */
            {
             do {     
				 if (g_cmd_array[0] != g_cmd){
					 goto fun_end27;
					 }
                light_one_led(1, 255, 1, 3, 0);//白色
				 if (g_cmd_array[0] != g_cmd){
					 goto fun_end27;
					 }
                light_one_led(4, 255, 1, 3, 0);
				 if (g_cmd_array[0] != g_cmd){
					 goto fun_end27;
					 }
                light_one_led(7, 255, 1, 3, 0);
				if (g_cmd_array[0] != g_cmd){
					goto fun_end27;
					}
                
                  light_one_led( 10 ,255, 1,1, 0); //蓝色
				if (g_cmd_array[0] != g_cmd){
					goto fun_end27;
					}
                  light_one_led( 31 ,255, 1,1, 0);
				if (g_cmd_array[0] != g_cmd){
					goto fun_end27;
					}
                  light_one_led( 19 ,255, 1,1, 0);
				  if (g_cmd_array[0] != g_cmd){
					  goto fun_end27;
					  }
                  light_one_led( 28 ,255, 1,1, 0);
				  if (g_cmd_array[0] != g_cmd){
					  goto fun_end27;
					  }
                  light_one_led( 13 ,255, 1,1, 0);
				  if (g_cmd_array[0] != g_cmd){
					  goto fun_end27;
					  }
                  light_one_led( 25 ,255, 1,1, 0);
				  if (g_cmd_array[0] != g_cmd){
					  goto fun_end27;
					  }
                  light_one_led( 16 ,255, 1,1, 0);
				  if (g_cmd_array[0] != g_cmd){
					  goto fun_end27;
					  }
                  light_one_led( 22 ,255, 1,1, 0);
				  if (g_cmd_array[0] != g_cmd){
					  goto fun_end27;
					  }
                  light_one_led( 19 ,255, 1,1, 0);
				  if (g_cmd_array[0] != g_cmd){
					  goto fun_end27;
					  }
                  light_one_led( 34 ,255, 1,1, 0);
				  if (g_cmd_array[0] != g_cmd){
					  goto fun_end27;
					  }
                    mdelay(3000);
                 
                 for(i=10;i<36;i += 3){  //关闭???有LED???
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end27;
						 }
                  light_one_led( i, 100, 1,8, 0);
                 }
fun_end27:
	g_switch_status = 0;
                 mdelay(3000);
                }while((g_cmd == on_and_listening) & (g_switch_status));
            }
         break;
            
             case re_blue :/* 蓝灯旋转 */
             {
              do {                
                  for(i=1;i<36;i += 3){  //蓝色
					  if (g_cmd_array[0] != g_cmd){
						  goto fun_end28;
						  }
                   light_one_led(i, 255, 1, 1, 0);
                  }
                  
                  for(i=2;i<36;i += 3){  
					  if (g_cmd_array[0] != g_cmd){
						  goto fun_end28;
						  }
                   light_one_led(i, 255, 1, 3, 200);//白色
					  if (g_cmd_array[0] != g_cmd){
						  goto fun_end28;
						  }
                   if (i == 5){
                    light_one_led(1, 255, 1, 1, 200);
					if (g_cmd_array[0] != g_cmd){
						goto fun_end28;
						}
				   }
                   if (i == 8){
                    light_one_led(4, 255, 1, 1, 200);
					if (g_cmd_array[0] != g_cmd){
						goto fun_end28;
						}
				   }
                   if (i == 11){
                    light_one_led(7, 255, 1, 1, 200);
					if (g_cmd_array[0] != g_cmd){
						goto fun_end28;
						}
				   }
                   if (i == 14){
					   if (g_cmd_array[0] != g_cmd){
						   goto fun_end28;
						   }
                    light_one_led(10, 255, 1, 1, 200);
				   }
                   if (i == 17){
                    light_one_led(13, 255, 1, 1, 200);
					if (g_cmd_array[0] != g_cmd){
						goto fun_end28;
						}
				   }
                   if (i == 20){
                    light_one_led(16, 255, 1, 1, 200);
					if (g_cmd_array[0] != g_cmd){
						goto fun_end28;
						}
				   }
                   if (i == 23){
                    light_one_led(19, 255, 1, 1, 200);
					if (g_cmd_array[0] != g_cmd){
						goto fun_end28;
						}
				   }
                   if (i == 26){
                    light_one_led(22, 255, 1, 1, 200);
					if (g_cmd_array[0] != g_cmd){
						goto fun_end28;
						}
				   }
                    if (i == 29){
                     light_one_led(25, 255, 1, 1, 200);
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end28;
						 }
					}
                    if (i == 32){
                     light_one_led(28, 255, 1, 1, 200);
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end28;
						 }
					}
                    if (i == 35){
                     light_one_led(31, 255, 1, 1, 200);
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end28;
						 }
					}
                  }
fun_end28:
	g_switch_status = 0;
             }while((g_cmd == re_blue) & (g_switch_status));
                }
          break;
             
             case blue_slow_turn:/* 蓝慢??? */
             {
              do{   
                blue_slow_turn_fun();  //蓝慢???
                }while((g_cmd == blue_slow_turn) & (g_switch_status));
             }
         break;
/////////////////////////////////////////////////////vol start/////////////////// 
  			 case vol_zer_step:/* 音量0 */
  			 {
  			  do {   
  				  all_leds_white();
  				 }while((g_cmd == vol_zer_step) & (g_switch_status));
  			 }
  		  break;

             case vol_one_step:/* 音量1 */
             {
              do {   
                mod_pwm_val(); 
                 }while((g_cmd == vol_one_step) & (g_switch_status));
             }
          break;
			 
             case vol_two_step:/* 音量2 */
             {
              do {   
                mod_pwm_val(); 
                 }while((g_cmd == vol_two_step) & (g_switch_status));
             }
          break;
			 
             case vol_thr_step:/* 音量3 */
             {
              do {   
                mod_pwm_val(); 
                 }while((g_cmd == vol_thr_step) & (g_switch_status));
             }
          break;
			 
             case vol_fou_step:/* 音量4 */
             {
              do {   
                mod_pwm_val(); 
                 }while((g_cmd == vol_fou_step) & (g_switch_status));
             }
          break;
			 
             case vol_fiv_step:/* 音量5 */
             {
              do {   
                mod_pwm_val(); 
                 }while((g_cmd == vol_fiv_step) & (g_switch_status));
             }
          break;
			 
			case vol_six_step:/* 音量6 */
			{
			 do {   
			   mod_pwm_val(); 
				}while((g_cmd == vol_six_step) & (g_switch_status));
			}
		 break;
			
			case vol_sev_step:/* 音量7 */
			{
			 do {   
			   mod_pwm_val(); 
				}while((g_cmd == vol_sev_step) & (g_switch_status));
			}
		 break;
			
			case vol_eig_step:/* 音量8 */
			{
			 do {   
			   mod_pwm_val(); 
				}while((g_cmd == vol_eig_step) & (g_switch_status));
			}
		 break;
			
			case vol_nin_step:/* 音量9 */
			{
			 do {   
			   mod_pwm_val(); 
				}while((g_cmd == vol_nin_step) & (g_switch_status));
			}
		 break;
			
			case vol_ten_step:/* 音量10 */
			{
			 do {   
			   mod_pwm_val(); 
				}while((g_cmd == vol_ten_step) & (g_switch_status));
			}
		 break;
//////////////////////////////////////////////////////vol end///////////////////             
             case mul_color_re:/* 彩色旋转 */
             {
              do {   
                mul_color_re_fun(); 
                 }while((g_cmd == mul_color_re) & (g_switch_status));
             }
          break;
			 
			  case all_leds_orange:
			  {
			   do {	  
				   for(i=2;i<36;i += 3){  //橙色
					   if (g_cmd_array[0] != g_cmd){
						   goto fun_end29;
						   }
					light_one_led( i, 100, 1,7, 100);
				   }
fun_end29:
	g_switch_status = 0;				   
				  }while((g_cmd == all_leds_orange) & (g_switch_status));
			  }
		   break;
			 
			 case re_green:
			 {
			  do{ 	 
				  for(i=0;i<36;i += 3){  /* 绿色旋转 */
					  if (g_cmd_array[0] != g_cmd){
						  goto fun_end30;
						  }
				   light_one_led( i, 100, 1,0, 100);
				  }
 // 				mdelay(200);
				  
				  for(i=2;i<36;i += 3){  /* 关闭所有LED */
					  if (g_cmd_array[0] != g_cmd){
						  goto fun_end30;
						  }
				   light_one_led( i, 100, 1,8, 0);
				  }
fun_end30:
	g_switch_status = 0;
				  mdelay(200);
				 }while((g_cmd == re_green) & (g_switch_status));
			 }
		  break;
			 
			  case re_red:
			  {
			   do {	  
				   for(i=2;i<36;i += 3){  /* 红色旋转 */
					   if (g_cmd_array[0] != g_cmd){
						   goto fun_end31;
						   }
					light_one_led( i, 100, 1,2, 100);
				   }
  //				 mdelay(200);
				   
				   for(i=2;i<36;i += 3){  /* 关闭所有LED */
					   if (g_cmd_array[0] != g_cmd){
						   goto fun_end31;
						   }
					light_one_led( i, 100, 1,8, 0);
				   }
fun_end31:
	g_switch_status = 0;
				   mdelay(200);
				  }while((g_cmd == re_red) & (g_switch_status));
			  }
		   break;
			  
			 case one_blue_bar:
			 {
				 for(i=2;i<36;i += 3){	/* 关闭所有LED */
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end32;
						 }
				  light_one_led( i, 100, 1,8, 0);
				 }
			  do { 	 
				  for(i=2;i<3;i += 3){  //黄色
					  if (g_cmd_array[0] != g_cmd){
						  goto fun_end32;
						  }
				   light_one_led( i, 100, 1,4, 0);
				  }
fun_end32:
	g_switch_status = 0;
				}while((g_cmd == one_blue_bar) & (g_switch_status));
			 }
		  break;
			 
			 case two_blue_bar:
			 {
				 for(i=2;i<36;i += 3){	/* 关闭所有LED */
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end33;
						 }
				  light_one_led( i, 100, 1,8, 0);
				 }
			  do{ 	 
				  for(i=2;i<6;i += 3){  //黄色
					  if (g_cmd_array[0] != g_cmd){
						  goto fun_end33;
						  }
				   light_one_led( i, 100, 1,4, 0);
				  }
fun_end33:
	g_switch_status = 0;
				}while((g_cmd == two_blue_bar) & (g_switch_status));
			 }
		  break;
			 
			 case thr_blue_bar:
			 {
				 for(i=2;i<36;i += 3){	/* 关闭所有LED */
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end34;
						 }
				  light_one_led( i, 100, 1,8, 0);
				 }
			  do { 	 
				  for(i=2;i<9;i += 3){  //黄色
					  if (g_cmd_array[0] != g_cmd){
						  goto fun_end34;
						  }
				   light_one_led( i, 100, 1,4, 0);
				  }
fun_end34:
	g_switch_status = 0;
				}while((g_cmd == thr_blue_bar) & (g_switch_status));
			 }
		  break;
			 
			 case fou_blue_bar:
			 {
				 for(i=2;i<36;i += 3){	/* 关闭所有LED */
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end35;
						 }
				  light_one_led( i, 100, 1,8, 0);
				 }
			  do { 	 
				  for(i=2;i<12;i += 3){  //黄色
					  if (g_cmd_array[0] != g_cmd){
						  goto fun_end35;
						  }
				   light_one_led( i, 100, 1,4, 0);
				  }
fun_end35:
	g_switch_status = 0;
				}while((g_cmd == fou_blue_bar) & (g_switch_status));
			 }
		  break;
			 
			 case fiv_blue_bar:
			 {
				 for(i=2;i<36;i += 3){	/* 关闭所有LED */
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end36;
						 }
				  light_one_led( i, 100, 1,8, 0);
				 }
			  do { 	 
				  for(i=2;i<15;i += 3){  //黄色
					  if (g_cmd_array[0] != g_cmd){
						  goto fun_end36;
						  }
				   light_one_led( i, 100, 1,4, 0);
				  }
fun_end36:
	g_switch_status = 0;
				}while((g_cmd == fiv_blue_bar) & (g_switch_status));
			 }
		  break;
			 
			 case six_blue_bar:
			 {
				 for(i=2;i<36;i += 3){	/* 关闭所有LED */
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end37;
						 }
				  light_one_led( i, 100, 1,8, 0);
				 }
			  do { 	 
				  for(i=2;i<18;i += 3){  //黄色
					  if (g_cmd_array[0] != g_cmd){
						  goto fun_end37;
						  }
				   light_one_led( i, 100, 1,4, 0);
				  }
fun_end37:
	g_switch_status = 0;
				}while((g_cmd == six_blue_bar) & (g_switch_status));
			 }
		  break;
			 
			 case sev_blue_bar:
			 {
				 for(i=2;i<36;i += 3){	/* 关闭所有LED */
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end38;
						 }
				  light_one_led( i, 100, 1,8, 0);
				 }
			  do { 	 
				  for(i=2;i<21;i += 3){  //黄色
					  if (g_cmd_array[0] != g_cmd){
						  goto fun_end38;
						  }
				   light_one_led( i, 100, 1,4, 0);
				  }
fun_end38:
	g_switch_status = 0;
				}while((g_cmd == sev_blue_bar) & (g_switch_status));
			 }
		  break;
			 
			 case eig_blue_bar:
			 {
				 for(i=2;i<36;i += 3){	/* 关闭所有LED */
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end39;
						 }
				  light_one_led( i, 100, 1,8, 0);
				 }
			  do { 	 
				  for(i=2;i<24;i += 3){  //黄色
					  if (g_cmd_array[0] != g_cmd){
						  goto fun_end39;
						  }
				   light_one_led( i, 100, 1,4, 0);
				  }
fun_end39:
	g_switch_status = 0;
				}while((g_cmd == eig_blue_bar) & (g_switch_status));
			 }
		  break;
			 
			 case nin_blue_bar:
			 {
				 for(i=2;i<36;i += 3){	/* 关闭所有LED */
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end40;
						 }
				  light_one_led( i, 100, 1,8, 0);
				 }
			  do { 	 
				  for(i=2;i<27;i += 3){  //黄色
					  if (g_cmd_array[0] != g_cmd){
						  goto fun_end40;
						  }
				   light_one_led( i, 100, 1,4, 0);
				  }
fun_end40:
	g_switch_status = 0;
				}while((g_cmd == nin_blue_bar) & (g_switch_status));
			 }
		  break;
			 
			 case ten_blue_bar:
			 {
				 for(i=2;i<36;i += 3){	/* 关闭所有LED */
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end41;
						 }
				  light_one_led( i, 100, 1,8, 0);
				 }
			  do { 	 
				  for(i=2;i<30;i += 3){  //黄色
					  if (g_cmd_array[0] != g_cmd){
						  goto fun_end41;
						  }
				   light_one_led( i, 100, 1,4, 0);
				  }
fun_end41:
	g_switch_status = 0;
				}while((g_cmd == ten_blue_bar) & (g_switch_status));
			 }
		  break;
			 
			 case ele_blue_bar:
			 {
				 for(i=2;i<36;i += 3){	/* 关闭所有LED */
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end42;
						 }
				  light_one_led( i, 100, 1,8, 0);
				 }
			  do { 	 
				  for(i=2;i<33;i += 3){  //黄色
					  if (g_cmd_array[0] != g_cmd){
						  goto fun_end42;
						  }
				   light_one_led( i, 100, 1,4, 0);
				  }
fun_end42:
	g_switch_status = 0;
				}while((g_cmd == ele_blue_bar) & (g_switch_status));
			 }
		  break;
			 
			 case twe_blue_bar:
			 {
				 for(i=2;i<36;i += 3){	/* 关闭所有LED */
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end43;
						 }
				  light_one_led( i, 100, 1,8, 0);
				 }
			  do { 	 
				  for(i=2;i<36;i += 3){  //黄色
					  if (g_cmd_array[0] != g_cmd){
						  goto fun_end43;
						  }
				   light_one_led( i, 100, 1,4, 0);
				  }
fun_end43:
	g_switch_status = 0;
				}while((g_cmd == twe_blue_bar) & (g_switch_status));
			 }
		  break;
			 
			 
#endif
          /* 控制单个LED模式*/
             case first_led:
             {
              do {   
                ctl_first_led_fun(); 
				i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
                 }while((g_cmd == first_led) & (g_switch_status));
             }
          break;
             
             case second_led:
             {
              do {   
                ctl_secod_led_fun(); 
				i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
                 }while((g_cmd == first_led) & (g_switch_status));
             }
          break;
             
             case third_led:
             {
              do {   
                ctl_third_led_fun(); 
				i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
                 }while((g_cmd == third_led) & (g_switch_status));
             }
          break;
             
             case fourth_led:
             {
              do {   
                ctl_fourth_led_fun(); 
				i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
                 }while((g_cmd == fourth_led) & (g_switch_status));
             }
          break;
             
             case fifth_led:
             {
              do {   
                ctl_fifth_led_fun(); 
				i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
                 }while((g_cmd == fifth_led) & (g_switch_status));
             }
          break;
             
             case sixth_led:
             {
              do {   
                ctl_sixth_led_fun(); 
				i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
                 }while((g_cmd == sixth_led) & (g_switch_status));
             }
          break;
             
             case seventh_led:
             {
              do {   
                ctl_seventh_led_fun(); 
				i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
                 }while((g_cmd == seventh_led) & (g_switch_status));
             }
          break;
             
             case eighth_led:
             {
              do{   
                ctl_eighth_led_fun(); 
				i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
                 }while((g_cmd == eighth_led) & (g_switch_status));
             }
          break;
             
             case ninth_led:
             {
              do{   
                ctl_ninth_led_fun(); 
				i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
                 }while((g_cmd == ninth_led) & (g_switch_status));
             }
          break;
             
             case tenth_led:
             {
              do {   
                ctl_tenth_led_fun(); 
				i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
                 }while((g_cmd == tenth_led) & (g_switch_status));
             }
          break;
             
             case eleventh_led:
             {
              do {   
                ctl_eleventh_led_fun(); 
				i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
                 }while((g_cmd == eleventh_led) & (g_switch_status));
             }
          break;
             
             case twelfth_led:
             {
              do {   
                ctl_twelfth_led_fun(); 
				i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
                 }while((g_cmd == twelfth_led) & (g_switch_status));
             }
          break;

		  case one_white_blue :
		  {
			do {				
				for(i=1;i<36;i += 3){  //蓝色
					if (g_cmd_array[0] != g_cmd){
						goto fun_end200;
						}
					if (i == 1){
						light_one_led(i, 255, 1, 3, 0);//白色
						}else{
						 light_one_led(i, 255, 1, 1, 0);
						}
				}
fun_end200:
  g_switch_status = 0;
		   }while((g_cmd == one_white_blue) & (g_switch_status));
			  }
		break;

          /* 关闭所有LED */
             case off_all_led:
             {
              do {   
                ctl_off_all_led_fun(); 
                 }while((g_cmd == off_all_led) & (g_switch_status));
              i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
             }
          break;
			 
          /* 红色LED常亮 */
             case all_leds_long_time_red:
             {
              do {   
                all_leds_long_time_red_fun(); 
                 }while((g_cmd == all_leds_long_time_red) & (g_switch_status));
              i2c_smbus_write_byte_data(led1_client, Update_Register, Led_on);
             }
          break;
			 
#if 0
          
		 /* 第 一颗亮白色其余亮蓝色 */
		   case one_white_blue :
		   {
			do {				
				for(i=1;i<36;i += 3){  //蓝色
					if (i == 1){
						light_one_led(i+1, 255, 1, 3, 0);	/* 亮白色 */
						}
						if (g_cmd_array[0] != g_cmd){
							goto fun_end200;
							}
				 light_one_led(i, 255, 1, 1, 0);
				}
				
fun_end200:
  g_switch_status = 0;
		   }while((g_cmd == one_white_blue) & (g_switch_status));
			  }
		break;
#endif		   
//#endif
		 /* 第二颗亮白色其余亮蓝色 */
		   case two_white_blue :
		   {
			do {				
				for(i=1;i<36;i += 3){  //蓝色
				light_one_led(i, 255, 1, 1, 0);
					if (i == 4){
						light_one_led(i+1, 255, 1, 3, 0); /* 亮白色 */
						}
						if (g_cmd_array[0] != g_cmd){
							goto fun_end201;
							}
				}
				
fun_end201:
  g_switch_status = 0;
		   }while((g_cmd == two_white_blue) & (g_switch_status));
			  }
		break;

		
		 /* 第三颗亮白色其余亮蓝色 */
		   case thr_white_blue :
		   {
			do {				
				for(i=1;i<36;i += 3){  //蓝色
					light_one_led(i, 255, 1, 1, 0);
						if (i == 7){
							light_one_led(i+1, 255, 1, 3, 0); /* 亮白色 */
							}
							if (g_cmd_array[0] != g_cmd){
								goto fun_end201;
								}
					}
				
	fun_end202:
	g_switch_status = 0;
		   }while((g_cmd == thr_white_blue) & (g_switch_status));
			  }
		break;
		   
		 /* 第四颗亮白色其余亮蓝色 */
		   case fou_white_blue :
		   {
			do {				
				for(i=1;i<36;i += 3){  //蓝色
					light_one_led(i, 255, 1, 1, 0);
						if (i == 10){
							light_one_led(i+1, 255, 1, 3, 0); /* 亮白色 */
							}
							if (g_cmd_array[0] != g_cmd){
								goto fun_end201;
								}
					}
fun_end203:
  g_switch_status = 0;
		   }while((g_cmd == fou_white_blue) & (g_switch_status));
			  }
		break;
		   
		 /* 第五颗亮白色其余亮蓝色 */
		   case fiv_white_blue :
		   {
			do {				
				for(i=1;i<36;i += 3){  //蓝色
					light_one_led(i, 255, 1, 1, 0);
						if (i == 13){
							light_one_led(i+1, 255, 1, 3, 0); /* 亮白色 */
							}
							if (g_cmd_array[0] != g_cmd){
								goto fun_end201;
								}
					}
				
fun_end204:
  g_switch_status = 0;
		   }while((g_cmd == fiv_white_blue) & (g_switch_status));
			  }
		break;
		   
		 /* 第六颗亮白色其余亮蓝色 */
		   case six_white_blue :
		   {
			do {				
				for(i=1;i<36;i += 3){  //蓝色
					light_one_led(i, 255, 1, 1, 0);
						if (i == 16){
							light_one_led(i+1, 255, 1, 3, 0); /* 亮白色 */
							}
							if (g_cmd_array[0] != g_cmd){
								goto fun_end201;
								}
					}
				
fun_end205:
  g_switch_status = 0;
		   }while((g_cmd == six_white_blue) & (g_switch_status));
			  }
		break;
		   
		 /* 第七颗亮白色其余亮蓝色 */
		   case sev_white_blue :
		   {
			do {				
				for(i=1;i<36;i += 3){  //蓝色
					light_one_led(i, 255, 1, 1, 0);
						if (i == 19){
							light_one_led(i+1, 255, 1, 3, 0); /* 亮白色 */
							}
							if (g_cmd_array[0] != g_cmd){
								goto fun_end201;
								}
					}
fun_end206:
  g_switch_status = 0;
		   }while((g_cmd == sev_white_blue) & (g_switch_status));
			  }
		break;

		 /* 第八颗亮白色其余亮蓝色 */
		   case eig_white_blue :
		   {
			do {				
				for(i=1;i<36;i += 3){  //蓝色
					light_one_led(i, 255, 1, 1, 0);
						if (i == 22){
							light_one_led(i+1, 255, 1, 3, 0); /* 亮白色 */
							}
							if (g_cmd_array[0] != g_cmd){
								goto fun_end201;
								}
					}
fun_end207:
  g_switch_status = 0;
		   }while((g_cmd == eig_white_blue) & (g_switch_status));
			  }
		break;

		
		 /* 第九颗亮白色其余亮蓝色 */
		   case nin_white_blue :
		   {
			do {				
				for(i=1;i<36;i += 3){  //蓝色
					light_one_led(i, 255, 1, 1, 0);
						if (i == 25){
							light_one_led(i+1, 255, 1, 3, 0); /* 亮白色 */
							}
							if (g_cmd_array[0] != g_cmd){
								goto fun_end201;
								}
					}
	fun_end208:
	g_switch_status = 0;
		   }while((g_cmd == nin_white_blue) & (g_switch_status));
			  }
		break;
		   
		 /* 第十颗亮白色其余亮蓝色 */
		   case ten_white_blue :
		   {
			do {				
				for(i=1;i<36;i += 3){  //蓝色
					light_one_led(i, 255, 1, 1, 0);
						if (i == 28){
							light_one_led(i+1, 255, 1, 3, 0); /* 亮白色 */
							}
							if (g_cmd_array[0] != g_cmd){
								goto fun_end201;
								}
					}
fun_end209:
  g_switch_status = 0;
		   }while((g_cmd == ten_white_blue) & (g_switch_status));
			  }
		break;
		   
		 /* 第十一颗亮白色其余亮蓝色 */
		   case ele_white_blue :
		   {
			do {				
				for(i=1;i<36;i += 3){  //蓝色
					light_one_led(i, 255, 1, 1, 0);
						if (i == 31){
							light_one_led(i+1, 255, 1, 3, 0); /* 亮白色 */
							}
							if (g_cmd_array[0] != g_cmd){
								goto fun_end201;
								}
					}
fun_end210:
  g_switch_status = 0;
		   }while((g_cmd == ele_white_blue) & (g_switch_status));
			  }
		break;
		   
		 /* 第十二颗亮白色其余亮蓝色 */
		   case twl_white_blue :
		   {
			do {				
				for(i=1;i<36;i += 3){  //蓝色
					light_one_led(i, 255, 1, 1, 0);
						if (i == 34){
							light_one_led(i+1, 255, 1, 3, 0); /* 亮白色 */
							}
							if (g_cmd_array[0] != g_cmd){
								goto fun_end201;
								}
					}
fun_end211:
  g_switch_status = 0;
		   }while((g_cmd == twl_white_blue) & (g_switch_status));
			  }
		break;
		   
		 /* 常亮蓝色 */
		   case all_blue :
		   {
			do {				
				for(i=1;i<36;i += 3){  //蓝色
					if (g_cmd_array[0] != g_cmd){
						goto fun_end212;
						}
				 light_one_led(i, 255, 1, 1, 0);
				}
				
fun_end212:
  g_switch_status = 0;
		   }while((g_cmd == all_blue) & (g_switch_status));
			  }
		break;
		   
		 /* 常亮绿色 */
		   case all_green :
		   {
			do {				
				for(i=0;i<36;i += 3){  //绿色
					if (g_cmd_array[0] != g_cmd){
						goto fun_end2120;
						}
				 light_one_led(i, 255, 1, 0, 0);
				}
				
fun_end2120:
  g_switch_status = 0;
		   }while((g_cmd == all_green) & (g_switch_status));
			  }
		break;
		   
		 /* 常亮白色 */
		   case all_white :
		   {
			do {				
				for(i=2;i<36;i += 3){  //白色
					if (g_cmd_array[0] != g_cmd){
						goto fun_end2121;
						}
				 light_one_led(i, 255, 1, 3, 0);
				}
				
fun_end2121:
  g_switch_status = 0;
		   }while((g_cmd == all_white) & (g_switch_status));
			  }
		break;

/* 看见智能灯效=======================开始==========20180131============== */		
		 /* 启动白色---变白光跑马灯---白色 */
		 case start_white_turn_white_around :
		 {
		  do {				  
			  for(i=1;i<36;i += 3){  //白色
				  if (g_cmd_array[0] != g_cmd){
					  goto fun_end213;
					  }
			   light_one_led(i, 255, 1, 3, 0);
			  }	
			  mdelay(2000);
			  for (i = 0; i < 4; i++){
			  	white_round_val_fun(); /* 白光跑马 */
			  }
			  
			for(i=1;i<36;i += 3){  //白色
				if (g_cmd_array[0] != g_cmd){
					goto fun_end213;
					}
			 light_one_led(i, 255, 1, 3, 0);
			} 
			mdelay(2000);
fun_end213:
g_switch_status = 0;
		 }while((g_cmd == start_white_turn_white_around) & (g_switch_status));
			}
	  break;

	/* 白色脉冲 */
		 case pulse_white :
		 {
		  do {				  
			  for(i=2;i<36;i += 3){  
				  if (g_cmd_array[0] != g_cmd){
					  goto fun_end214;
					  }
					  light_one_led(i, 255, 1, 3, 0);//白色
			  }
			  mdelay(500);
			  
			for(i=1;i<36;i += 3){  
				if (g_cmd_array[0] != g_cmd){
					goto fun_end214;
					}
					light_one_led(i+1, 30, 1, 3, 0);//关闭
			}
			mdelay(500);
fun_end214:
g_switch_status = 0;
		 }while((g_cmd == pulse_white) & (g_switch_status));
			}
	  break;
		 
	  case little_blue_around:/* 淡蓝色跑马灯 旋转多久不确定 */
	  {
	   do {   
		 little_blue_round_val_fun(); 
		  }while((g_cmd == little_blue_around) & (g_switch_status));
	  }
   break;
		  /* 保持弱淡蓝色 */
		   case keep_little_blue :
		   {
			do {				
				for(i=1;i<36;i += 3){  
					if (g_cmd_array[0] != g_cmd){
						goto fun_end216;
						}
						light_one_led(i, 255, 1, 9, 0);//淡蓝色
				}
fun_end216:
  g_switch_status = 0;
		   }while((g_cmd == keep_little_blue) & (g_switch_status));
			  }
		break;
		   
		  /* 橙色脉冲 */
		   case pulse_orange :
		   {
			do {				
				for(i = 1;i < 36;i += 3){  
					if (g_cmd_array[0] != g_cmd){
						goto fun_end217;
						}
				 light_one_led(i+1, 255, 1, 7, 0);
				}
				mdelay(500);
				
				for(i = 1;i < 36;i += 3){  
					if (g_cmd_array[0] != g_cmd){
						goto fun_end217;
						}
						light_one_led(i+1, 0, 1, 8, 0);
				}
				mdelay(500);
fun_end217:
  g_switch_status = 0;
		   }while((g_cmd == pulse_orange) & (g_switch_status));
			  }
		break;
		   
		   /* 保持橙色 */
			case keep_orange :
			{
			 do {				 
				 for(i = 1;i < 36;i += 3){	
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end218;
						 }
				  light_one_led(i+1, 255, 1, 7, 0);
				 }
 fun_end218:
   g_switch_status = 0;
			}while((g_cmd == keep_orange) & (g_switch_status));
			   }
		 break;
			
		/* 蓝色跑马灯 旋转多久不确定*/
		   case blue_around:
		   {
			do {   
			  blue_round_val_fun(); 
			   }while((g_cmd == blue_around) & (g_switch_status));
		   }
		break;
		   
		  /* 红色脉冲 */
		   case pulse_red :
		   {
			do {				
				for(i = 1;i < 36;i += 3){  
					if (g_cmd_array[0] != g_cmd){
						goto fun_end219;
						}
				 light_one_led(i+1, 255, 1, 2, 0);
				}
				mdelay(100);
				
				for(i = 1;i < 36;i += 3){  
					if (g_cmd_array[0] != g_cmd){
						goto fun_end219;
						}
						light_one_led(i+1, 0, 1, 8, 0);
				}
				mdelay(100);
fun_end219:
  g_switch_status = 0;
		   }while((g_cmd == pulse_red) & (g_switch_status));
			  }
		break;
		   
		   /* 保持红色 */
			case keep_red :
			{
			 do {				 
				 for(i = 1;i < 36;i += 3){	
					 if (g_cmd_array[0] != g_cmd){
						 goto fun_end220;
						 }
				  light_one_led(i+1, 255, 1, 2, 0);
				 }
 fun_end220:
   g_switch_status = 0;
			}while((g_cmd == keep_red) & (g_switch_status));
			   }
		 break;
		   
/* 看见智能灯效=======================结束==========20180131============== */		
          
        }
        
    /* 
     * 让出CPU运行其他线程，并在指定的时间内重新被调度   条件为假 
     */
    schedule_timeout(HZ);   /* 休眠，与set_current_state配合使用，需要计算，这里表示休眠?????? */
   
   }

      return 0;
}

static struct file_operations led1_fops = {
	.owner     = THIS_MODULE,
    .unlocked_ioctl = led_ioctl,
    .write     = leds_write,
};

static int fl3236_led_fetch_sysconfig_para(void){
    int ret = -1;
    u32 fl3236_led_used_flag;
    script_item_value_type_e type;
    script_item_u fl3236_led_item_temp;
       
   LED_DBG("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	type = script_get_item("fl3236_led_para", "fl3236_led_used", &fl3236_led_item_temp);
	if(type == SCIRPT_ITEM_VALUE_TYPE_INT){
		fl3236_led_used_flag = fl3236_led_item_temp.val;
	}else{
		printk("script_parser_fetch fl3236_led_used failed\n");
		fl3236_led_used_flag = 0;
	}
             
    if (1 != fl3236_led_used_flag ) {
    	printk(KERN_ERR"%s: fl3236_led_used_flag = %d\n", __func__, fl3236_led_used_flag);
    }else{
    	printk("fl3236_led_used_flag = %d\n", fl3236_led_used_flag);
    	}
    
    type = script_get_item("fl3236_led_para", "fl3236_led_shut", &fl3236_led_item_temp);
    if (SCIRPT_ITEM_VALUE_TYPE_PIO == type) {
    	printk("get fl3236_led_shut gpio OK!\n");
    	fl3236_led_shut.gpio = fl3236_led_item_temp.gpio.gpio;        
    } else {
         printk("get fl3236_led_shut gpio failed\n");
    }       
                
    /* 
     * 申请GPIO使能SDB 设置输出高电??? 
     */
	ret = gpio_request(fl3236_led_shut.gpio, "fl3236_led_shut");
	if (ret < 0) {
		printk("Failed to request gpio [%d] for fl3236_led_shut\n", fl3236_led_shut.gpio);
		gpio_free(fl3236_led_shut.gpio);
		return ret;
	}
	else
		{
			printk("request gpio [%d] for fl3236_led_shut OK!\n",fl3236_led_shut.gpio);
			}

	if (0 != gpio_direction_output(fl3236_led_shut.gpio, 1)){
		pr_err("fl3236_led_shut set err!");
	}
	__gpio_set_value(fl3236_led_shut.gpio, 1);
	msleep(150);
    LED_DBG("fl3236_led_shut.gpio = %d\n",__gpio_get_value(fl3236_led_shut.gpio));

	return 0;
}

static int __devinit led_probe(struct i2c_client *client,
				  const struct i2c_device_id *id)
{
    LED_DBG("client->addr = 0x%x\n", client->addr);
        if (client->addr == 0x3f)
        {
            led1_client = client;
            
            if (!(major = register_chrdev(0, "led1", &led1_fops))) {
                pr_warn("unable to register led1\n");
            }
            
            class = class_create(THIS_MODULE, "led1");
            if (IS_ERR(class)) {
                unregister_chrdev(major, "led1");
                pr_warn("unable to register led1\n");
            }
            led1_dev = device_create(class, NULL, MKDEV(major, 0), NULL, "led1_dev"); /* dev/led1_dev */
            if (IS_ERR(led1_dev)) {
                printk(KERN_WARNING "led1_dev create failed\n");
                return PTR_ERR(led1_dev);
            }
        }
#if 1
        i2c_smbus_write_byte_data(client, 0x4f, 0xff);   /* 用户写任何字节数据即复位 */
        i2c_smbus_write_byte_data(client, 0x4a, 0x0);    /* 使能???有LED通道 */
        i2c_smbus_write_byte_data(client, 0x00, 0x01);   /* 设置软件关机模式 bit[0] = 1*/
        i2c_smbus_write_byte_data(client, 0x25, 0x01);   /* 全局控制寄存??? */
#endif
	return 0;
}

static int __devexit led_remove(struct i2c_client *client)
{
	LED_DBG("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	device_destroy(class, MKDEV(major, 0));
	class_destroy(class);
	unregister_chrdev(major, "led");
		
	return 0;
}

static const struct i2c_device_id led_id_table[] = {
    { "led1", led_drv_bus_num},
	{}
};

    /* 
     * 1. 分配/设置i2c_driver 
     */
static struct i2c_driver led_driver = {
	.driver	= {
		.name	= "R16",
		.owner	= THIS_MODULE,
	},
	.probe		= led_probe,
	.remove		= __devexit_p(led_remove),
	.id_table	= led_id_table,
};

#if 0
int leds_light(void *data){
    int i;
    while(1){
    
       set_current_state(TASK_UNINTERRUPTIBLE); /* 将当前的状???表示设置为休眠 */
    
         if (kthread_should_stop()) 
             break;                                    
         
    switch (g_cmd) {
    case PowerOn:
        while(g_cmd == PowerOn) {
            Leds_PowerOn_Mode(0);
 //       for(i=0;i<36;i +=3){
 //       light_one_led(NULL, i, 255, 1, 0, 0);//绿色
 //       }
 //       mdelay(1000);
            }
        break;
        
    case 101:
        while(g_cmd == 101) {
            for(i=1;i<36;i +=3){
            light_one_led(NULL, i, 255, 1, 1, 0);//蓝色
            }
            mdelay(1000);
            }
        break;
        
    case 102:
        while(g_cmd == 102) {
            for(i=2;i<36;i +=3){
            light_one_led(NULL, i, 255, 1, 2, 0);//红色
            }
            mdelay(1000);
            }
        break;
        
    case 103:
        while(g_cmd == 103) {
            for(i=2;i<36;i +=3){
            light_one_led(NULL, i, 255, 1, 3, 0);//白色
            }
            mdelay(1000);
            }
        break;
        
    case 104:
        while(g_cmd == 104) {
            for(i=2;i<36;i +=3){
            light_one_led(NULL, i, 255, 1, 4, 0);//黄色
            }
            mdelay(1000);
            }
        break;
        
    case 105:
        while(g_cmd == 105) {
            for(i=2;i<36;i +=3){
            light_one_led(NULL, i, 255, 1, 5, 0);//青色
            }
            mdelay(1000);
            }
        break;
                            
    case 106:
        while(g_cmd == 106) {
            for(i=2;i<36;i +=3){
            light_one_led(NULL, i, 255, 1, 6, 0);//品色
            }
            mdelay(1000);
            }
        break;
        
    case 107:
        while(g_cmd == 107) {
            for(i=2;i<36;i +=3){
            light_one_led(NULL, i, 255, 1, 7, 0);//橙色
            }
            mdelay(1000);
            }
        break;
                
    }
     /* 
      * 让出CPU运行其他线程，并在指定的时间内重新被调度   条件为假 
      */
     schedule_timeout(HZ);   /* 休眠，与set_current_state配合使用，需要计算，这里表示休眠?????? */
    
    }

       return 0;

}
#endif
static int led_drv_init(void)
{
        int err;
	LED_DBG("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	fl3236_led_fetch_sysconfig_para();
    
	/* 2. 注册i2c_driver */
	i2c_add_driver(&led_driver);
//    led_light_task = kthread_create(leds_light, NULL, "led_light_task");
    led_light_task = kthread_create(leds_write, NULL, "led_light_task");
    led_ctrl_task = kthread_create(led_ioctl, NULL, "led_ctrl_task");

    if((IS_ERR(led_light_task)) && (IS_ERR(led_ctrl_task))){

     printk("Unable to start kernel thread.\n");

      err = PTR_ERR(led_light_task);
      err = PTR_ERR(led_ctrl_task);

      led_light_task =NULL;
      led_ctrl_task =NULL;

      return err;
    }
    wake_up_process(led_light_task);
    wake_up_process(led_ctrl_task);
	return 0;
}

static void led_drv_exit(void)
{
	LED_DBG("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
    gpio_free(fl3236_led_shut.gpio);
	i2c_del_driver(&led_driver);
}

module_init(led_drv_init);
module_exit(led_drv_exit);
MODULE_LICENSE("GPL");


