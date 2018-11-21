/*
 * drivers/input/touchscreen/apt8l08/apt8l08
 *
 * Copyright (c) 2017 shenzhen
 *	Ziven<liulibinhs@163.com>
 *	version v0.1
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <linux/of.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/pm_runtime.h>
#include <linux/pm.h>
#include <linux/input/mt.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/async.h>
#include <linux/hrtimer.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/device.h>
#include <linux/power/scenelock.h>
#include <linux/workqueue.h>
#include <mach/sys_config.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/init-input.h>
#include <linux/keyboard.h>
#include <linux/input.h>

#include "apt8l08.h"

///////////////////////////////////////////////
#define apt8l08_I2C_NAME		"apt8l08"
#define apt8l08_I2C_ADDR 	0x56

/* I2C总线宏和变量 */
#define bus_num   0 

#define apt8l08_INPUT_DEV_NAME ("apt8l08_tpad")
						 
//static struct input_dev *apt8l08_input;
#define MAX_KEY_NUM  100 

/*主控温度检测上报*/
//#define REPORT_TEMPERATURE

/*定义工作队列*/
#define APT_QUEUE
#ifdef APT_QUEUE
static struct work_struct  apt8l08_work;
static struct workqueue_struct *apt8l08_workqueue = NULL;
#else
static struct delayed_work apt8l08_work;
static struct workqueue_struct *apt8l08_workqueue = NULL;
#endif

/* apt8l08宏开关，0关闭调试打印，1打开 */
#define apt8l08_DBG  1 
#if (apt8l08_DBG)
    #define apt8l08_DBG(format,args...)  printk("[apt8l08] "format,##args)    
#else
    #define apt8l08_DBG(...)    
#endif

static unsigned int irq,irq_num;
int major;
static struct class *apt8l08_class=NULL;
static struct device *apt8l08_class_dev=NULL;

/*上一次读取的寄存器值*/
static u16 old_value = 0xff;
static int key_state;
static unsigned int t_time;
static int reg_val;

static struct pin_desc *irq_pd=NULL;
static struct timer_list buttons_timer;

bool short_press_flag, long_press_flag;
static struct timer_list s_timer;
int count,report_val;
u8 key_val=0 ;

char time_flag = 0;
u8 irq_num_flag = 0;

/*定义全局apt8l08_pad*/
struct apt8l08_pad *apt8l08_tpad1=NULL;
static const unsigned short normal_i2c[2] = {apt8l08_I2C_ADDR, I2C_CLIENT_END};
struct i2c_client *apt8l08_i2c=NULL;
static int apt8l08_reg_init(struct i2c_client *client);

/*定义按键值*/
static unsigned char keypad_keycodes[100] ={
	1,2,3,4,5,6,7,8,9,10,
	11,12,13,14,15,16,17,18,19,20,
	21,22,23,24,25,26,27,28,29,30,
	31,32,33,34,35,36,37,38,39,40,
	41,42,43,44,45,46,47,48,49,50,
	51,52,53,54,55,56,57,58,59,60,
	61,62,63,64,65,66,67,68,69,70,
	71,72,73,74,75,76,77,78,79,80,
	81,82,83,84,85,86,87,88,89,90,
	91,92,93,94,95,96,97,98,99,100
};

/*申请初始化apt8l08 input dev*/
static int apt8l08_input_dev_reg(struct apt8l08_pad *apt8l08_pad){
	int i=0,err=0;
	
	apt8l08_DBG("==%d---%s==\n",__LINE__, __FUNCTION__);
	apt8l08_pad->apt8l08_input = input_allocate_device();
	if(!apt8l08_pad->apt8l08_input){
		printk("apt8l08 input dev:not enough memory for input device \n");
		return -1;
	}
	apt8l08_pad->apt8l08_input->name = apt8l08_INPUT_DEV_NAME;
	apt8l08_pad->apt8l08_input->phys = "apt8l08/input0";
	apt8l08_pad->apt8l08_input->id.bustype = BUS_HOST;
	apt8l08_pad->apt8l08_input->id.vendor = 0x0010;
	apt8l08_pad->apt8l08_input->id.product = 0x0010;
	apt8l08_pad->apt8l08_input->id.version = 0x0100;
	apt8l08_pad->apt8l08_input->evbit[0] = BIT_MASK(EV_KEY);
	
	for(i = 0; i < MAX_KEY_NUM ; i++ ){
		set_bit(keypad_keycodes[i],apt8l08_pad->apt8l08_input->keybit);
	}
	err = input_register_device(apt8l08_pad->apt8l08_input);
	if(err){
		printk("apt8l08 input dev register error!\n");
		goto fail;
	}
	
	return 0;
fail:
	input_free_device(apt8l08_pad->apt8l08_input);
	return -1;

}

static int tpad_fetch_sysconfig_para(void){
	apt8l08_DBG("==%d---%s==\n",__LINE__, __FUNCTION__);
        int ret = -1;
        u32 tpad_used_flag;
        script_item_value_type_e type;
        script_item_u apt8l08_item_temp;
       
	type = script_get_item("apt8l08_para", "apt8l08_used", &apt8l08_item_temp);
	if(type == SCIRPT_ITEM_VALUE_TYPE_INT){
		tpad_used_flag = apt8l08_item_temp.val;
	}else{
		printk("script_parser_fetch apt8l08_tpad_used failed\n");
		tpad_used_flag = 0;
	}
        
        if (1 != tpad_used_flag ) {
        	printk(KERN_ERR"%s: tpad_unused. tpad_used_flag = %d\n", __func__, tpad_used_flag);
            return -EIO;
        }else{
        	printk("tpad_used_flag = %d\n", tpad_used_flag);
            
            ////////////////////apt8l08_int///////////////////////////////////////////       
            type = script_get_item("apt8l08_para", "apt8l08_int", &apt8l08_item_temp);
            if (SCIRPT_ITEM_VALUE_TYPE_PIO == type) { 
                  printk("get apt8l08_int gpio OK!\n");         
                config_info.apt8l08_int.gpio = apt8l08_item_temp.gpio.gpio;
            } else {
                    printk("get apt8l08_int gpio failed\n");
                    }
            
            type = script_get_item("apt8l08_para", "apt8l08_en", &apt8l08_item_temp);
            if (SCIRPT_ITEM_VALUE_TYPE_PIO == type) { 
                  printk("get apt8l08_en gpio OK!\n");         
                config_info.apt8l08_en.gpio = apt8l08_item_temp.gpio.gpio;
            } else {
                    printk("get apt8l08_en gpio failed\n");
                    }
            
            ret = gpio_request(config_info.apt8l08_en.gpio, "apt8l08_en");
                     if (ret < 0) {
                             printk("Failed to request gpio [%d] for apt8l08_en\n", config_info.apt8l08_en.gpio);
                             gpio_free(config_info.apt8l08_en.gpio);
                             return ret;
                     }
                     else
                     {
                         printk("request gpio [%d] for apt8l08_en OK!\n",config_info.apt8l08_en.gpio);
                     }
            
                     if (0 != gpio_direction_output(config_info.apt8l08_en.gpio, 1)){
                             pr_err("apt8l08_en set err!");
                     }
                     __gpio_set_value(config_info.apt8l08_en.gpio, 1);
                     printk("config_info.apt8l08_en.gpio = %d\n", __gpio_get_value(config_info.apt8l08_en.gpio));
                                    
            /* 
             * 中断申请
             */         
            if(0 != gpio_request(config_info.apt8l08_int.gpio,NULL)){
                gpio_free(config_info.apt8l08_int.gpio);
                printk("[%s]:%d=request apt8l08_int fail!\n",__func__,__LINE__);    
                }
                apt8l08_DBG("%s ok!\n",__func__);
            
                return 0;
        	}
        
        return -EINVAL;
}


static char init_apt8l08_chip(struct i2c_client *client)
{
	struct i2c_adapter *adapter = client->adapter;
	
    char ret=0;
	int i;
    mdelay(5);/* 上电后主控芯片需要等待5ms后才可以初始化 */
	
    ret = i2c_smbus_write_byte_data(client, 0x3A, 0x5A); /* SYSCON写入0x5A时，芯片进入系统配置模式 */
    if (!ret){
		mdelay(5);
        if ((i2c_smbus_read_byte_data(client, 0x3A)) == 0x5a){
			i2c_smbus_write_byte_data(client, 0x20, 0x02);
			i2c_smbus_write_byte_data(client, 0x21, 0x51);
			i2c_smbus_write_byte_data(client, 0x22, 0x20);
			i2c_smbus_write_byte_data(client, 0x23, 0xe1);
			i2c_smbus_write_byte_data(client, 0x24, 0x00);
			i2c_smbus_write_byte_data(client, 0x25, 0x00);
			i2c_smbus_write_byte_data(client, 0x26, 0x08);
			i2c_smbus_write_byte_data(client, 0x27, 0x02);
			i2c_smbus_write_byte_data(client, 0x28, 0x02);
			i2c_smbus_write_byte_data(client, 0x29, 0x10);
			i2c_smbus_write_byte_data(client, 0x2A, 0x10);
			i2c_smbus_write_byte_data(client, 0x2B, 0x04);
			i2c_smbus_write_byte_data(client, 0x2C, 0x00);
			i2c_smbus_write_byte_data(client, 0x2D, 0x00);
			i2c_smbus_write_byte_data(client, 0x3A, 0x00);
			mdelay(20);
		}
	}
	
	ret = i2c_smbus_write_byte_data(client, 0x3A, 0x5A); /* SYSCON写入0x5A时，芯片进入系统配置模式 */
    if (!ret){
		mdelay(5);
        if ((i2c_smbus_read_byte_data(client, 0x3A)) == 0x5a){
			
            i2c_smbus_write_byte_data(client, 0x21, 0x51); /* 500KHz 单键模式 按下低电平抬起高电平  */
            
 //           i2c_smbus_write_byte_data(client, 0x2D, 0x08); /* 设置开发模式 量产后必须恢复默认值0x00*/
 //           i2c_smbus_write_byte_data(client, 0x2A, 0xFF); /* 设置开发模式 量产后必须恢复默认值0x10*/
            
            i2c_smbus_write_byte_data(client, 0x23, 0xe1); /* 通道使能寄存器 使能K1 K2 K3 K4*/
            i2c_smbus_write_byte_data(client, 0x20, 0x02);/* 灵敏度寄存器，值越大灵敏度越高，不超过0x0F */
     //       i2c_smbus_write_byte_data(client, 0x2c, 0x0a); /* 靠靠0x00,靠靠靠縄NT靠靠靠靠 */
            
            i2c_smbus_write_byte_data(client, 0x22, 0x20); /* mod20171025 */
            i2c_smbus_write_byte_data(client, 0x29, 0x40); /* mod20171025 取三个按键平均值 (5+8+5)/3=0x6 */
            
            /* 按键阀值寄存器对应表,调整灵敏度寄存器 */
       #if 0
            i2c_smbus_write_byte_data(client, 0x00, 0xff); /* K00*/
            i2c_smbus_write_byte_data(client, 0x01, 0x7); /* K01*/
            i2c_smbus_write_byte_data(client, 0x02, 0xf); /* K02*/
            i2c_smbus_write_byte_data(client, 0x03, 0xa); /* K03*/
            i2c_smbus_write_byte_data(client, 0x04, 0x7); /* K04*/
            i2c_smbus_write_byte_data(client, 0x05, 0xff); /* K05*/
            i2c_smbus_write_byte_data(client, 0x06, 0xff); /* K06*/
            i2c_smbus_write_byte_data(client, 0x07, 0xff); /* K07*/
			#else // modfy 20180706
						i2c_smbus_write_byte_data(client, 0x00, 0x04); /* K00*/
            i2c_smbus_write_byte_data(client, 0x01, 0x04); /* K01*/
            i2c_smbus_write_byte_data(client, 0x02, 0x01); /* K02*/
            i2c_smbus_write_byte_data(client, 0x03, 0x04); /* K03*/
            i2c_smbus_write_byte_data(client, 0x04, 0x04); /* K04*/
            i2c_smbus_write_byte_data(client, 0x05, 0x04); /* K05*/
            i2c_smbus_write_byte_data(client, 0x06, 0x04); /* K06*/
            i2c_smbus_write_byte_data(client, 0x07, 0x18); /* K07*/
			#endif
						
            i2c_smbus_write_byte_data(client, 0x3A, 0x00);/* IC进入正常工作模式 */

			for(i=0;i<14;i++){
				apt8l08_DBG("i2c_smbus_read_byte_data(client, 0x%x) = 0x%x\n", i+0x20,i2c_smbus_read_byte_data(client, i+0x20));
			}
			printk("I2C Connect successed!\n");
				
			return 0;
            }else{
                printk("I2C Connect error!\n");
                return -EBUSY;
            }
        }
 }

static int apt8l08_detect(struct i2c_client *client, struct i2c_board_info *info)
{
	apt8l08_DBG("==%d---%s==\n",__LINE__, __FUNCTION__);
	struct i2c_adapter *adapter = client->adapter;
	char ret;

    if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
                return -ENODEV;

	if(bus_num == adapter->nr){
    	pr_info("%s: addr= 0x%x\n",__func__,client->addr);
        strlcpy(info->type, apt8l08_I2C_NAME, I2C_NAME_SIZE);
	}
    return 0;
}

static void apt8l08_work_func(struct work_struct *work)
{
	unsigned int time_val=0;
	u8 short_time_flag =0;
	
	msleep(1);
	key_state = __gpio_get_value(config_info.apt8l08_int.gpio);
//	apt8l08_DBG("apt8l08 init_val=%d\n",key_state);

	if(0 == t_time){
		reg_val = i2c_smbus_read_byte_data(apt8l08_i2c, 0x34);
		apt8l08_DBG("reg_val=%d\n",reg_val);
	}
	
	t_time++;
	if((90 == t_time) && (0 == key_state)){
		input_report_key(apt8l08_tpad1->apt8l08_input, reg_val+1, 1);//上报有按键按下
		input_sync(apt8l08_tpad1->apt8l08_input);
		apt8l08_DBG(" report_val=%d   long_report start\n\n",reg_val+1);
	}
	if(1 == key_state){
//		apt8l08_DBG("t_time=%d\n",t_time);
		if(t_time >= 90){
			input_report_key(apt8l08_tpad1->apt8l08_input, reg_val+1, 0);//放开
			input_sync(apt8l08_tpad1->apt8l08_input);
			apt8l08_DBG(" report_val=%d   long_report end\n\n",reg_val+1);
		}else if(reg_val != 0){
#ifdef REPORT_TEMPERATURE			
			if(16 == reg_val){
				int reg_val_temperature;
				reg_val_temperature = readl(0xf1c25020);//获取主控温度
				apt8l08_DBG("cpu_temperature_20=%x    %d\n",reg_val_temperature,((reg_val_temperature & 0xfff) - 1665)*100/618);
				msleep(20);
				input_report_key(apt8l08_tpad1->apt8l08_input, ((reg_val_temperature & 0xfff) - 1665)*100/618, 1);
				input_sync(apt8l08_tpad1->apt8l08_input);
				msleep(1);
				input_report_key(apt8l08_tpad1->apt8l08_input, ((reg_val_temperature & 0xfff) - 1665)*100/618, 0);
				input_sync(apt8l08_tpad1->apt8l08_input);
			}else{
#endif				
				input_report_key(apt8l08_tpad1->apt8l08_input, reg_val, 1);//上报有按键按下
				input_sync(apt8l08_tpad1->apt8l08_input);

				msleep(1);
				input_report_key(apt8l08_tpad1->apt8l08_input, reg_val, 0);//放开
				input_sync(apt8l08_tpad1->apt8l08_input);
				apt8l08_DBG(" report_val=%d   short_report\n\n",reg_val);
#ifdef REPORT_TEMPERATURE				
			}
#endif			
		}
		
		t_time = 0;
	}
}

static irqreturn_t apt8l08_irq_func(int irq, void *dev)
{
//	printk("==%d---%s==\n",__LINE__, __FUNCTION__);
#ifdef APT_QUEUE
	queue_work(apt8l08_workqueue, &apt8l08_work);
#else	
	queue_delayed_work(apt8l08_workqueue, &apt8l08_work, 10);
#endif	
	return IRQ_HANDLED;
}

static void irq_init(struct apt8l08_pad *pad)
{
	irq=gpio_to_irq(config_info.apt8l08_int.gpio);
	apt8l08_DBG("===========apt8l08 irqrequest num=%d!===========\n",irq);
//	request_irq(irq, apt8l08_irq_func,IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "apt8l08_irq", NULL);
	request_irq(irq, apt8l08_irq_func,IRQF_TRIGGER_LOW, "apt8l08_irq", NULL);
}

static int apt8l08_pad_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	struct apt8l08_pad *pad;
	int rc = 0;
	char ret=0;
	
	apt8l08_DBG("==%d---%s==\n",__LINE__, __FUNCTION__);
	apt8l08_DBG("apt8l08 Enter %s\n", __func__);
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev, "I2C functionality not supported\n");
		return -ENODEV;
	}

	pad = kzalloc(sizeof(*pad), GFP_KERNEL);
	if (!pad){
	        printk("allocate data fail!\n");
		return -ENOMEM;
	}

    apt8l08_i2c = client;
	pad->client = client;
	apt8l08_tpad1 = pad;
	i2c_set_clientdata(client, pad);

	/*注册输入设备*/
	apt8l08_input_dev_reg(pad);
	i2c_set_clientdata(client, apt8l08_tpad1);
	
	/*设置apt8l08寄存器*/
    ret = init_apt8l08_chip(client);
    if (ret){
        printk("init apt8l08 error!\n");
        }
    
	/*申请工作队列，做中断后端处理*/
    i2c_smbus_write_byte_data(client, 0x3A, 0x00);/* IC进入工作模式 */
	
#ifdef APT_QUEUE
	INIT_WORK(&apt8l08_work, apt8l08_work_func);                                                                                                         
    apt8l08_workqueue = create_singlethread_workqueue("apt8l08_workqueue");
#else	
	INIT_DELAYED_WORK(&apt8l08_work, apt8l08_work_func);
	apt8l08_workqueue = create_singlethread_workqueue("apt8l08_workqueue");
#endif	
	
	/*申请中断处理,中断初始化函数如果放在请求队列前面系统会崩溃*/
	irq_init(pad);
    
	return ret;

}

static int apt8l08_pad_remove(struct i2c_client *client)
{
	struct apt8l08_pad *pad = i2c_get_clientdata(client);
	apt8l08_DBG("==apt8l08 pad_remove=\n");

	kfree(pad);

	return 0;
}

static const struct i2c_device_id apt8l08_pad_id[] = {
	{apt8l08_I2C_NAME, bus_num},
	{}
};
MODULE_DEVICE_TABLE(i2c, apt8l08_pad_id);


static struct i2c_driver apt8l08_pad_driver = {
	.class = I2C_CLASS_HWMON,
	.driver = {
		.name = apt8l08_I2C_NAME,
		.owner = THIS_MODULE,
	},
	.probe = apt8l08_pad_probe,
	.remove = apt8l08_pad_remove,
	.id_table = apt8l08_pad_id,
	.address_list = normal_i2c,
	.detect = apt8l08_detect,

};

static int __init apt8l08_pad_init(void)
{
	apt8l08_DBG("==%d---%s\n",__LINE__, __FUNCTION__);
	int ret = -1;       
	tpad_fetch_sysconfig_para();
	ret = i2c_add_driver(&apt8l08_pad_driver);
	return ret;
}

static void __exit apt8l08_pad_exit(void)
{
	apt8l08_DBG("==apt8l08 pad_exit==\n");
    gpio_free(config_info.apt8l08_int.gpio);
    gpio_free(config_info.apt8l08_en.gpio);
    destroy_workqueue(apt8l08_workqueue);
	i2c_del_driver(&apt8l08_pad_driver);
}

module_init(apt8l08_pad_init);
module_exit(apt8l08_pad_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("apt8l08 touchpad controller driver");
MODULE_AUTHOR("siyrra, whw0701@126.com");
MODULE_ALIAS("platform:CP PAD");

