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

/* apt8l08宏开关，0关闭调试打印，1打开 */
#define apt8l08_DBG  1

#if (apt8l08_DBG)
    #define apt8l08_DBG(format,args...)  printk("[apt8l08] "format,##args)    
#else
    #define apt8l08_DBG(...)    
#endif

///////////////////////////////////////////////
#define apt8l08_I2C_NAME		"apt8l08"
#define apt8l08_I2C_ADDR 	0x56

/* I2C总线宏和变量 */
#define bus_num   0 
static __u32 twi_id = 0;

#define apt8l08_INPUT_DEV_NAME ("apt8l08_tpad")
						 
static unsigned int irq,irq_num;
int major;
static struct class *apt8l08_class=NULL;
static struct device *apt8l08_class_dev=NULL;

/*定义工作队列*/
static struct delayed_work apt8l08_work;
static struct workqueue_struct *apt8l08_workqueue = NULL;
/*上一次读取的寄存器值*/
static u16 old_value = 0xff;

static struct pin_desc *irq_pd=NULL;
static struct timer_list buttons_timer;

//static struct input_dev *apt8l08_input;
//#define MAX_KEY_NUM 3
/*定义按键值*/
//static unsigned char keypad_mapindex[10] ={
//	0,1,2,3,4,5,6,7,8,9
//};

bool short_press_flag, long_press_flag;
struct timer_list s_timer;
int count,report_val;
u8 key_val=0 ;

/*定义全局apt8l08_pad*/
struct apt8l08_pad *apt8l08_tpad1;
static const unsigned short normal_i2c[2] = {apt8l08_I2C_ADDR, I2C_CLIENT_END};
struct i2c_client *apt8l08_i2c;
static int apt8l08_reg_init(struct i2c_client *client);

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
	
//	for(i = 0; i < MAX_KEY_NUM ; i++ ){
//		set_bit(keypad_mapindex[i],apt8l08_pad->apt8l08_input->keybit);
//	}
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
        }else{
        	printk("tpad_used_flag = %d\n", tpad_used_flag);
        	}
                
        ////////////////////apt8l08_int///////////////////////////////////////////       
        type = script_get_item("apt8l08_para", "apt8l08_int", &apt8l08_item_temp);
        if (SCIRPT_ITEM_VALUE_TYPE_PIO == type) { 
        	  printk("get apt8l08_int gpio OK!\n");         
            config_info.apt8l08_int.gpio = apt8l08_item_temp.gpio.gpio;
        } else {
            printk("get apt8l08_int gpio failed\n");
        }
        						
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

static char tpad_i2c_test(struct i2c_client *client)
{
	apt8l08_DBG("==%d---%s==\n",__LINE__, __FUNCTION__);

	char ret=0;
	u16 data;
    
    return 0;

}

static char init_apt8l08_chip(struct i2c_client *client)
{
    char ret=0;
    mdelay(5);/* 上电后主控芯片需要等待5ms后才可以初始化 */
    ret = i2c_smbus_write_byte_data(client, 0x3A, 0x5A); /* SYSCON写入0x5A时，芯片进入系统配置模式 */
  
    if (!ret){
        if ((i2c_smbus_read_byte_data(client, 0x3A)) == 0x5a){
            i2c_smbus_write_byte_data(client, 0x21, 0x51); /* 500KHz 单键模式 按下低电平抬起高电平  */
            
            i2c_smbus_write_byte_data(client, 0x2D, 0x08); /* 设置开发模式 量产后必须恢复默认值0x00*/
            i2c_smbus_write_byte_data(client, 0x2A, 0xFF); /* 设置开发模式 量产后必须恢复默认值0x10*/
            i2c_smbus_write_byte_data(client, 0x23, 0x00); /* 通道使能寄存器 写0全部使能*/
            i2c_smbus_write_byte_data(client, 0x20, 0x01);/* 灵敏度寄存器，值越大灵敏度越高，不超过0x0F */
            
            /* 按键阀值寄存器对应表 */
            i2c_smbus_write_byte_data(client, 0x00, 0x04); /* K00*/
            i2c_smbus_write_byte_data(client, 0x01, 0x04); /* K01*/
            i2c_smbus_write_byte_data(client, 0x02, 0x04); /* K02*/
            i2c_smbus_write_byte_data(client, 0x03, 0x04); /* K03*/
            i2c_smbus_write_byte_data(client, 0x04, 0x04); /* K04*/
            i2c_smbus_write_byte_data(client, 0x05, 0x04); /* K05*/
            i2c_smbus_write_byte_data(client, 0x06, 0x04); /* K06*/
            i2c_smbus_write_byte_data(client, 0x07, 0x04); /* K07*/

            i2c_smbus_write_byte_data(client, 0x3A, 0x00);/* IC进入正常工作模式 */
            
            apt8l08_DBG("i2c_smbus_read_byte_data(client, 0x3A) = 0x%x\n", i2c_smbus_read_byte_data(client, 0x3A));
            apt8l08_DBG("i2c_smbus_read_byte_data(client, 0x21) = 0x%x\n", i2c_smbus_read_byte_data(client, 0x21));
            apt8l08_DBG("i2c_smbus_read_byte_data(client, 0x2D) = 0x%x\n", i2c_smbus_read_byte_data(client, 0x2D));
            apt8l08_DBG("i2c_smbus_read_byte_data(client, 0x2A) = 0x%x\n", i2c_smbus_read_byte_data(client, 0x2A));
            apt8l08_DBG("i2c_smbus_read_byte_data(client, 0x23) = 0x%x\n", i2c_smbus_read_byte_data(client, 0x23));
            apt8l08_DBG("i2c_smbus_read_byte_data(client, 0x20) = 0x%x\n", i2c_smbus_read_byte_data(client, 0x20));
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

	if(twi_id == adapter->nr){
    	pr_info("%s: addr= 0x%x\n",__func__,client->addr);
        strlcpy(info->type, apt8l08_I2C_NAME, I2C_NAME_SIZE);
	}
    return 0;
}

static void second_timer_handle(unsigned long arg)
{
	mod_timer(&s_timer,jiffies + HZ/2);
	count++;
	if(count == 2){
        if (report_val == 0x4){  /* V+ */
    		report_val = report_val + 2;
    		input_report_key(apt8l08_tpad1->apt8l08_input, report_val, 1);//上报有按键按下
            input_sync(apt8l08_tpad1->apt8l08_input);
    		printk("\n[jeffrey_apt8l08] report_val=%d  v+ long_start\n",report_val);
        }else if (report_val == 0x8){  /* V- */
                report_val = report_val + 2;
                input_report_key(apt8l08_tpad1->apt8l08_input, report_val, 1);//上报有按键按下
                input_sync(apt8l08_tpad1->apt8l08_input);
                printk("\n[jeffrey_apt8l08] report_val=%d  v- long_start\n",report_val);
        }else if (report_val == 0x20){  /* mic */
                report_val = report_val + 2;
                input_report_key(apt8l08_tpad1->apt8l08_input, report_val, 1);//上报有按键按下
                input_sync(apt8l08_tpad1->apt8l08_input);
                printk("\n[jeffrey_apt8l08] report_val=%d  mic long_start\n",report_val);
            }
	}
}

static void apt8l08_work_func(struct work_struct *work)
{
//	apt8l08_DBG("==%d---%s==\n",__LINE__, __FUNCTION__);
    key_val = i2c_smbus_read_byte_data(apt8l08_i2c, 0x34);

    if ((key_val == 0x4) ||(key_val == 0x8) ||(key_val == 0x20)){ /* V+  V- Mic*/
    count = 0;
    init_timer(&s_timer);
    s_timer.function = &second_timer_handle;
    s_timer.expires = jiffies + HZ;//1s后timer被激活
    add_timer(&s_timer);
    report_val = key_val;
    }else{
		if(count < 2){
            if (report_val == 0x4){  /* V+被按下 */
			report_val = report_val;
			input_report_key(apt8l08_tpad1->apt8l08_input, report_val, 1);//上报有按键按下
			input_sync(apt8l08_tpad1->apt8l08_input);
			printk("\n[jeffrey_apt8l08] report_val=%d  v+ short_start\n",report_val);
			
			msleep(1);
			input_report_key(apt8l08_tpad1->apt8l08_input, report_val+1, 0);//放开
			input_sync(apt8l08_tpad1->apt8l08_input);
			printk("\n[jeffrey_apt8l08] report_val=%d  v+ short_end\n",report_val+1);
            }else if (report_val == 0x8){  /* V-被按下 */
                report_val = report_val;
                input_report_key(apt8l08_tpad1->apt8l08_input, report_val, 1);//上报有按键按下
                input_sync(apt8l08_tpad1->apt8l08_input);
                printk("\n[jeffrey_apt8l08] report_val=%d  v- short_start\n",report_val);
                
                msleep(1);
                input_report_key(apt8l08_tpad1->apt8l08_input, report_val+1, 0);//放开
                input_sync(apt8l08_tpad1->apt8l08_input);
                printk("\n[jeffrey_apt8l08] report_val=%d  v- short_end\n",report_val+1);
                }else if (report_val == 0x20){  /* Mic被按下 */
                report_val = report_val;
                input_report_key(apt8l08_tpad1->apt8l08_input, report_val, 1);//上报有按键按下
                input_sync(apt8l08_tpad1->apt8l08_input);
                printk("\n[jeffrey_apt8l08] report_val=%d  mic short_start\n",report_val);
                
                msleep(1);
                input_report_key(apt8l08_tpad1->apt8l08_input, report_val+1, 0);//放开
                input_sync(apt8l08_tpad1->apt8l08_input);
                printk("\n[jeffrey_apt8l08] report_val=%d  mic short_end\n",report_val+1);
                }
		}else{
    		if (report_val == 0x6){
    			input_report_key(apt8l08_tpad1->apt8l08_input, report_val+1, 0);//放开
    			input_sync(apt8l08_tpad1->apt8l08_input);
    			printk("\n[jeffrey_apt8l08] report_val=%d  v+ long_end\n",report_val+1);
            }else if (report_val == 0x0a){
    			input_report_key(apt8l08_tpad1->apt8l08_input, report_val+1, 0);//放开
    			input_sync(apt8l08_tpad1->apt8l08_input);
    			printk("\n[jeffrey_apt8l08] report_val=%d  v- long_end\n",report_val+1);
            }else if (report_val == 0x22){
                input_report_key(apt8l08_tpad1->apt8l08_input, report_val+1, 0);//放开
                input_sync(apt8l08_tpad1->apt8l08_input);
                printk("\n[jeffrey_apt8l08] report_val=%d  mic long_end\n",report_val+1);
            }
		}
		del_timer(&s_timer);
			
	}	
}

static irqreturn_t apt8l08_irq_func(int irq, void *dev)
{
//	apt8l08_DBG("==%d---%s==\n",__LINE__, __FUNCTION__);
	queue_delayed_work(apt8l08_workqueue, &apt8l08_work, 10);
	
	return IRQ_HANDLED;
}

static void irq_init(struct apt8l08_pad *pad)
{
	irq=gpio_to_irq(config_info.apt8l08_int.gpio);
	apt8l08_DBG("===========apt8l08 irqrequest num=%d!===========\n",irq);
	request_irq(irq, apt8l08_irq_func,IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "apt8l08_irq", NULL);
	
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
	INIT_DELAYED_WORK(&apt8l08_work, apt8l08_work_func);
	apt8l08_workqueue = create_singlethread_workqueue("apt8l08_workqueue");
	
	/*申请中断处理*/
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
	i2c_del_driver(&apt8l08_pad_driver);
}

module_init(apt8l08_pad_init);
module_exit(apt8l08_pad_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("apt8l08 touchpad controller driver");
MODULE_AUTHOR("siyrra, whw0701@126.com");
MODULE_ALIAS("platform:CP PAD");

