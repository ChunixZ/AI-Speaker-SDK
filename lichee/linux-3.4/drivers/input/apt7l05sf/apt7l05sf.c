#include <linux/init.h>
#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/time.h>
#include <linux/hrtimer.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/io.h>
#include <linux/input.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/kernel.h>
#include <linux/ioport.h>

#include <asm/irq.h>
#include <mach/hardware.h>
#include <mach/sys_config.h>
#include <mach/irqs.h>
#include <mach/gpio.h>
#include <linux/pinctrl/consumer.h>
#include <linux/pinctrl/pinconf-sunxi.h>

#include <linux/input.h>
#include <linux/keyboard.h>

#include <linux/of.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
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
#include <linux/platform_device.h>
#include <linux/async.h>
#include <linux/hrtimer.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/device.h>
#include <linux/power/scenelock.h>

#define key_total_num 3
/*定义工作队列*/
static struct delayed_work apt7l05sf_micm_work,apt7l05sf_voladd_work,apt7l05sf_volsub_work;
static struct workqueue_struct *apt7l05sf_micm_workqueue = NULL,*apt7l05sf_voladd_workqueue = NULL,*apt7l05sf_volsub_workqueue = NULL;

/* 定义并初始化等待队列头 */
static DECLARE_WAIT_QUEUE_HEAD(apt7l05sf_waitq);
static volatile int ev_press = 0;
static struct fasync_struct *apt7l05sf_async=NULL;

int major;
static struct class *apt7l05sf_class=NULL;
static struct device *apt7l05sf_class_dev=NULL;

static script_item_u apt7l05sf_item;
struct gpio_config apt7l05sf_micm;
struct gpio_config apt7l05sf_voladd;
struct gpio_config apt7l05sf_volsub;
static script_item_value_type_e apt7l05sf_type;

#define INPUT_DEV_NAME ("apt7l05sf_detect")
static unsigned char keypad_apt7l05sf[key_total_num] = {1,2,3};
static struct input_dev *apt7l05sf_detect_dev=NULL;
static unsigned int micm_irq, voladd_irq, volsub_irq;
u8 report_val = 0, count =0;
u8 micm_flag=0, voladd_flag=0, volsub_flag=0;

struct timer_list s_timer;


static void micm_second_timer_handle(unsigned long arg)
{
	mod_timer(&s_timer,jiffies + HZ/2);
	count++;
	if(count == 2){
		report_val = report_val + 3;
		input_report_key(apt7l05sf_detect_dev, report_val, 1);//上报有按键按下
        input_sync(apt7l05sf_detect_dev);
		printk("\nmicm key long pressing...\n");
		printk("[jeffrey_apt7l05sf] report_val=%d  micm key long_start\n", report_val);
		
		/* ev_press=1表示中断发生了 */
		ev_press = 1;		
		
		/* 唤醒休眠的进程 */
		wake_up_interruptible(&apt7l05sf_waitq);  
		
		/* POLL_IN有数据时异步通知应用程序读 */
		kill_fasync (&apt7l05sf_async, SIGIO, POLL_IN);	
	}
}

static void voladd_second_timer_handle(unsigned long arg)
{
	mod_timer(&s_timer,jiffies + HZ/2);
	count++;
	if(count == 2){
		report_val = report_val + 7;
		input_report_key(apt7l05sf_detect_dev, report_val, 1);//上报有按键按下
        input_sync(apt7l05sf_detect_dev);
		printk("\nvoladd key long pressing...\n");
		printk("[jeffrey_apt7l05sf] report_val=%d  voladd key long_start\n", report_val);
		
		/* ev_press=1表示中断发生了 */
		ev_press = 1;		
		
		/* 唤醒休眠的进程 */
		wake_up_interruptible(&apt7l05sf_waitq);  
		
		/* POLL_IN有数据时异步通知应用程序读 */
		kill_fasync (&apt7l05sf_async, SIGIO, POLL_IN);	
	}
}

static void volsub_second_timer_handle(unsigned long arg)
{
	mod_timer(&s_timer,jiffies + HZ/2);
	count++;
	if(count == 2){
		report_val = report_val + 11;
		input_report_key(apt7l05sf_detect_dev, report_val, 1);//上报有按键按下
        input_sync(apt7l05sf_detect_dev);
		printk("\nvolsub key long pressing...\n");
		printk("[jeffrey_apt7l05sf] report_val=%d  volsub key long_start\n", report_val);
		
		/* ev_press=1表示中断发生了 */
		ev_press = 1;		
		
		/* 唤醒休眠的进程 */
		wake_up_interruptible(&apt7l05sf_waitq);  
		
		/* POLL_IN有数据时异步通知应用程序读 */
		kill_fasync (&apt7l05sf_async, SIGIO, POLL_IN);	
	}
}

static void apt7l05sf_micm_work_func(struct work_struct *work)
{	
	if(!gpio_get_value(apt7l05sf_micm.gpio)){
		count = 0;
		init_timer(&s_timer);
		s_timer.function = &micm_second_timer_handle;
		s_timer.expires = jiffies + HZ;
		add_timer(&s_timer);
       }else{
		if(count < 2){
            report_val=1;
            printk("\nmicm key short press!\n");
			input_report_key(apt7l05sf_detect_dev, report_val, 1);//上报有按键按下
			input_sync(apt7l05sf_detect_dev);
			printk("[jeffrey_apt7l05sf] report_val=%d  micm key short_start\n", report_val);
			
			/* 发生中断 */
			ev_press = 1;		
			
			/* 唤醒休眠的进程 */
			wake_up_interruptible(&apt7l05sf_waitq);  
			
			/* POLL_IN表示有数据时异步通知应用程序读 */
			kill_fasync (&apt7l05sf_async, SIGIO, POLL_IN);
			
			msleep(1);
            report_val = report_val + 1;
			input_report_key(apt7l05sf_detect_dev, report_val, 0);//放开
			input_sync(apt7l05sf_detect_dev);
            printk("\nmicm key short press up!\n");
			
			/* ev_press=1表示中断发生了 */
			ev_press = 1;		
			
			/* 唤醒休眠的进程 */
			wake_up_interruptible(&apt7l05sf_waitq);  
			
			/* POLL_IN表示有数据时异步通知应用程序读 */
			kill_fasync (&apt7l05sf_async, SIGIO, POLL_IN);
			
			printk("[jeffrey_apt7l05sf] report_val=%d  micm key short_end\n", report_val);
            report_val = 0;
		}else{
            report_val = report_val + 1;
			input_report_key(apt7l05sf_detect_dev, report_val, 0);//放开
			input_sync(apt7l05sf_detect_dev);
			
			/* ev_press=1表示中断发生了 */
			ev_press = 1;		
			
			/* 唤醒休眠的进程 */
			wake_up_interruptible(&apt7l05sf_waitq);  
			
			/* POLL_IN表示有数据时异步通知应用程序读 */
			kill_fasync (&apt7l05sf_async, SIGIO, POLL_IN);	
            printk("\nmicm key long press up!\n");
			printk("[jeffrey_apt7l05sf] report_val=%d  micm key long_end\n", report_val);
            report_val = 0;
		}
		del_timer(&s_timer);
        }

}


static void apt7l05sf_voladd_work_func(struct work_struct *work)
{	
	if(!gpio_get_value(apt7l05sf_voladd.gpio)){
		count = 0;
		init_timer(&s_timer);
		s_timer.function = &voladd_second_timer_handle;
		s_timer.expires = jiffies + HZ;
		add_timer(&s_timer);
       }else{
		if(count < 2){
            report_val=5;
            printk("\nvoladd key short press!\n");
			input_report_key(apt7l05sf_detect_dev, report_val, 1);//上报有按键按下
			input_sync(apt7l05sf_detect_dev);
			printk("[jeffrey_apt7l05sf] report_val=%d  voladd key short_start\n", report_val);
			
			/* 发生中断 */
			ev_press = 1;		
			
			/* 唤醒休眠的进程 */
			wake_up_interruptible(&apt7l05sf_waitq);  
			
			/* POLL_IN表示有数据时异步通知应用程序读 */
			kill_fasync (&apt7l05sf_async, SIGIO, POLL_IN);
			
			msleep(1);
            report_val = report_val + 1;
			input_report_key(apt7l05sf_detect_dev, report_val, 0);//放开
			input_sync(apt7l05sf_detect_dev);
            printk("\nvoladd key short press up!\n");
			
			/* ev_press=1表示中断发生了 */
			ev_press = 1;		
			
			/* 唤醒休眠的进程 */
			wake_up_interruptible(&apt7l05sf_waitq);  
			
			/* POLL_IN表示有数据时异步通知应用程序读 */
			kill_fasync (&apt7l05sf_async, SIGIO, POLL_IN);
			
			printk("[jeffrey_apt7l05sf] report_val=%d  voladd key short_end\n", report_val);
            report_val = 0;
		}else{
            report_val = report_val + 1;
			input_report_key(apt7l05sf_detect_dev, report_val, 0);//放开
			input_sync(apt7l05sf_detect_dev);
			
			/* ev_press=1表示中断发生了 */
			ev_press = 1;		
			
			/* 唤醒休眠的进程 */
			wake_up_interruptible(&apt7l05sf_waitq);  
			
			/* POLL_IN表示有数据时异步通知应用程序读 */
			kill_fasync (&apt7l05sf_async, SIGIO, POLL_IN);	
            printk("\nvoladd key long press up!\n");
			printk("[jeffrey_apt7l05sf] report_val=%d  voladd key long_end\n", report_val);
            report_val = 0;
		}
		del_timer(&s_timer);
        }

}


static void apt7l05sf_volsub_work_func(struct work_struct *work)
{	
	if(!gpio_get_value(apt7l05sf_volsub.gpio)){
		count = 0;
		init_timer(&s_timer);
		s_timer.function = &volsub_second_timer_handle;
		s_timer.expires = jiffies + HZ;
		add_timer(&s_timer);
       }else{
		if(count < 2){
            report_val=9;
            printk("\nvolsub key short press!\n");
			input_report_key(apt7l05sf_detect_dev, report_val, 1);//上报有按键按下
			input_sync(apt7l05sf_detect_dev);
			printk("[jeffrey_apt7l05sf] report_val=%d  volsub key short_start\n", report_val);
			
			/* 发生中断 */
			ev_press = 1;		
			
			/* 唤醒休眠的进程 */
			wake_up_interruptible(&apt7l05sf_waitq);  
			
			/* POLL_IN表示有数据时异步通知应用程序读 */
			kill_fasync (&apt7l05sf_async, SIGIO, POLL_IN);
			
			msleep(1);
            report_val = report_val + 1;
			input_report_key(apt7l05sf_detect_dev, report_val, 0);//放开
			input_sync(apt7l05sf_detect_dev);
            printk("\nvolsub key short press up!\n");
			
			/* ev_press=1表示中断发生了 */
			ev_press = 1;		
			
			/* 唤醒休眠的进程 */
			wake_up_interruptible(&apt7l05sf_waitq);  
			
			/* POLL_IN表示有数据时异步通知应用程序读 */
			kill_fasync (&apt7l05sf_async, SIGIO, POLL_IN);
			
			printk("[jeffrey_apt7l05sf] report_val=%d  volsub key short_end\n", report_val);
            report_val = 0;
		}else{
            report_val = report_val + 1;
			input_report_key(apt7l05sf_detect_dev, report_val, 0);//放开
			input_sync(apt7l05sf_detect_dev);
			
			/* ev_press=1表示中断发生了 */
			ev_press = 1;		
			
			/* 唤醒休眠的进程 */
			wake_up_interruptible(&apt7l05sf_waitq);  
			
			/* POLL_IN表示有数据时异步通知应用程序读 */
			kill_fasync (&apt7l05sf_async, SIGIO, POLL_IN);	
            printk("\nvolsub key long press up!\n");
			printk("[jeffrey_apt7l05sf] report_val=%d  volsub key long_end\n", report_val);
            report_val = 0;
		}
		del_timer(&s_timer);
        }

}

static int get_gpio_script(void)
{
	int req_st = -1,req_st1 = -1,req_st2 = -1;
	u32 apt7l05sf_used_flag; 
	
	apt7l05sf_type = script_get_item("apt7l05sf_para", "apt7l05sf_used", &apt7l05sf_item);
    if(apt7l05sf_type == SCIRPT_ITEM_VALUE_TYPE_INT){
     apt7l05sf_used_flag = apt7l05sf_item.val;
     }else{
         printk("script_parser_fetch apt7l05sf_used_flag failed\n");
         apt7l05sf_used_flag = 0;
         }

     if (1 != apt7l05sf_used_flag ) {
          printk(KERN_ERR"%s: tpad_unused. apt7l05sf_used_flag = %d\n", __func__, apt7l05sf_used_flag);
      }else{
          printk("apt7l05sf_used_flag = %d\n", apt7l05sf_used_flag);
          }
      
      apt7l05sf_type = script_get_item("apt7l05sf_para", "apt7l05sf_micm", &apt7l05sf_item);
       if (SCIRPT_ITEM_VALUE_TYPE_PIO == apt7l05sf_type) {
               printk("get apt7l05sf_micm gpio OK!\n");
               apt7l05sf_micm.gpio = apt7l05sf_item.gpio.gpio;
       } else {
            printk("get apt7l05sf_micm gpio failed\n");
       }
       
       apt7l05sf_type = script_get_item("apt7l05sf_para", "apt7l05sf_voladd", &apt7l05sf_item);
        if (SCIRPT_ITEM_VALUE_TYPE_PIO == apt7l05sf_type) {
                printk("get apt7l05sf_voladd gpio OK!\n");
                apt7l05sf_voladd.gpio = apt7l05sf_item.gpio.gpio;
        } else {
             printk("get apt7l05sf_voladd gpio failed\n");
        }
        
        apt7l05sf_type = script_get_item("apt7l05sf_para", "apt7l05sf_volsub", &apt7l05sf_item);
         if (SCIRPT_ITEM_VALUE_TYPE_PIO == apt7l05sf_type) {
                 printk("get apt7l05sf_volsub gpio OK!\n");
                 apt7l05sf_volsub.gpio = apt7l05sf_item.gpio.gpio;
         } else {
              printk("get apt7l05sf_volsub gpio failed\n");
         }

	req_st = gpio_request_one(apt7l05sf_micm.gpio, GPIOF_DIR_IN,NULL);
	if(0 != req_st){
		printk("[%s]=%d: request gpio failed !\n",__func__, __LINE__);
		return -EFAULT;
	}
    
	req_st = gpio_request_one(apt7l05sf_voladd.gpio, GPIOF_DIR_IN,NULL);
	if(0 != req_st){
		printk("[%s]=%d: request gpio failed !\n",__func__, __LINE__);
		return -EFAULT;
	}
    
	req_st = gpio_request_one(apt7l05sf_volsub.gpio, GPIOF_DIR_IN,NULL);
	if(0 != req_st){
		printk("[%s]=%d: request gpio failed !\n",__func__, __LINE__);
		return -EFAULT;
	}

	/*
	type = script_get_item("audio1", "audio_echo_power", &item_echo_power);

	if(SCIRPT_ITEM_VALUE_TYPE_PIO != type){
		printk("[%s]=%d:script_get_item return type err!\n",__func__, __LINE__);
		return -EFAULT;
	}
	req_st1 = gpio_request(item_echo_power.gpio.gpio, NULL);
	if(0 != req_st){
		printk("[%s]=%d: request gpio echo_power failed !\n",__func__, __LINE__);
		return -EFAULT;
	}	
	gpio_direction_output(item_echo_power.gpio.gpio, 0);
	
	type = script_get_item("echo", "audio_echo_mute", &item_echo_mute);

	if(SCIRPT_ITEM_VALUE_TYPE_PIO != type){
		printk("[%s]=%d:item_echo_mute return type err!\n",__func__, __LINE__);
		return -EFAULT;
	}
	req_st2 = gpio_request(item_echo_mute.gpio.gpio, NULL);
	if(0 != req_st2){
		printk("[%s]=%d: request gpio item_echo_mute failed !\n",__func__, __LINE__);
		return -EFAULT;
	}	
	gpio_direction_output(item_echo_mute.gpio.gpio, 0);*/
	
	return 0;
}

static irqreturn_t apt7l05sf_micm_irq_func(int irq, void *dev)
{
    micm_flag = 1;
//    printk("%s %d irq=%d\n", __FUNCTION__, __LINE__, irq);
	queue_delayed_work(apt7l05sf_micm_workqueue, &apt7l05sf_micm_work, 10);
	return IRQ_HANDLED;
}

static irqreturn_t apt7l05sf_voladd_irq_func(int irq, void *dev)
{
    voladd_flag = 1;
//    printk("%s %d irq=%d\n", __FUNCTION__, __LINE__, irq);
	queue_delayed_work(apt7l05sf_voladd_workqueue, &apt7l05sf_voladd_work, 10);
	return IRQ_HANDLED;
}

static irqreturn_t apt7l05sf_volsub_irq_func(int irq, void *dev)
{
    volsub_flag = 1;
 //   printk("%s %d irq=%d\n", __FUNCTION__, __LINE__, irq);
	queue_delayed_work(apt7l05sf_volsub_workqueue, &apt7l05sf_volsub_work, 10);
	return IRQ_HANDLED;
}

static void irq_init(void)
{
	micm_irq=gpio_to_irq(apt7l05sf_micm.gpio);
	voladd_irq=gpio_to_irq(apt7l05sf_voladd.gpio);
	volsub_irq=gpio_to_irq(apt7l05sf_volsub.gpio);
    
	request_irq(micm_irq, apt7l05sf_micm_irq_func, IRQF_TRIGGER_FALLING |IRQF_TRIGGER_RISING, "micm_irq", NULL);
	request_irq(voladd_irq, apt7l05sf_voladd_irq_func, IRQF_TRIGGER_FALLING |IRQF_TRIGGER_RISING, "voladd_irq", NULL);
	request_irq(volsub_irq, apt7l05sf_volsub_irq_func, IRQF_TRIGGER_FALLING |IRQF_TRIGGER_RISING, "volsub_irq", NULL);
    printk("%d %s micm_irq = %d\n", __LINE__, __FUNCTION__,  micm_irq);
    printk("%d %s voladd_irq = %d\n", __LINE__, __FUNCTION__,  voladd_irq);
    printk("%d %s volsub_irq = %d\n", __LINE__, __FUNCTION__,  volsub_irq);
}

static int apt7l05sf_drv_open(struct inode *inode, struct file *file)
{
	printk("%s\n", __FUNCTION__);
	return 0;
}

ssize_t apt7l05sf_drv_read (struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	if (size != 1)
		return -EINVAL;
	
	/* 如果没有按键动作, 休眠 */
	wait_event_interruptible(apt7l05sf_waitq, ev_press);

	/* 如果有按键动作, 返回键值 */
	if (copy_to_user(buf, &report_val, 1)){
		return -EFAULT;
	}
	ev_press = 0;
//	printk("apt7l05sf_drv_read = %d\n", report_val);
	
	return 1;
}

int apt7l05sf_drv_close(struct inode *inode, struct file *file)
{
	free_irq(micm_irq, NULL);
	free_irq(voladd_irq, NULL);
	free_irq(volsub_irq, NULL);
    
	return 0;
}

static int apt7l05sf_drv_fasync (int fd, struct file *filp, int on)
{
	printk("driver: apt7l05sf_drv_fasync\n");
	return fasync_helper (fd, filp, on, &apt7l05sf_async);
}

static struct file_operations apt7l05sf_drv_fops = {
    .owner    =  THIS_MODULE,    
    .open     =  apt7l05sf_drv_open,     
	.read	  =	 apt7l05sf_drv_read,	   
	.fasync	  =  apt7l05sf_drv_fasync,
	.release  =  apt7l05sf_drv_close,
};

static int __init apt7l05sf_init(void)
{
	int ret = 0 ,err = -1;
	int i;
	
	ret = get_gpio_script();
    #if 1
	if(ret < 0){
		printk("[%s]%d:get_gpio_script failed ! \n",__func__,__LINE__);
		return ret;
	}
	apt7l05sf_detect_dev = input_allocate_device();
	if(apt7l05sf_detect_dev){
		printk("power_detect not enough memory for input device! \n");
	}
	apt7l05sf_detect_dev->name = INPUT_DEV_NAME;
	apt7l05sf_detect_dev-> phys = "input/apt7l05sf_detect";
	apt7l05sf_detect_dev->id.bustype = BUS_HOST;
	apt7l05sf_detect_dev->id.vendor = 0x0003;
	apt7l05sf_detect_dev->id.product = 0x0003;
	apt7l05sf_detect_dev->id.version = 0x0200;
	apt7l05sf_detect_dev->evbit[0] = BIT_MASK(EV_KEY); //支持按键类事件
	for(i = 0; i < key_total_num; i++)
		set_bit( keypad_apt7l05sf[i], apt7l05sf_detect_dev->keybit);
	
	err = input_register_device(apt7l05sf_detect_dev);
	if(err < 0){
		input_free_device(apt7l05sf_detect_dev);
		printk("register apt7l05sf_detect_dev err! \n");
	}	
#endif	
    
	major = register_chrdev(0, "apt7l05sf", &apt7l05sf_drv_fops);
	apt7l05sf_class = class_create(THIS_MODULE, "apt7l05sf");
	apt7l05sf_class_dev = device_create(apt7l05sf_class, NULL, MKDEV(major, 0), NULL, "apt7l05sf"); 
	
	INIT_DELAYED_WORK(&apt7l05sf_micm_work, apt7l05sf_micm_work_func);
	INIT_DELAYED_WORK(&apt7l05sf_voladd_work, apt7l05sf_voladd_work_func);
	INIT_DELAYED_WORK(&apt7l05sf_volsub_work, apt7l05sf_volsub_work_func);
	apt7l05sf_micm_workqueue = create_singlethread_workqueue("apt7l05sf_micm_workqueue");
	apt7l05sf_voladd_workqueue = create_singlethread_workqueue("apt7l05sf_voladd_workqueue");
	apt7l05sf_volsub_workqueue = create_singlethread_workqueue("apt7l05sf_volsub_workqueue");
    irq_init();

	return ret;
}
static void __exit apt7l05sf_exit(void)
{
	printk("[%s]%d:exit apt7l05sf ! \n",__func__,__LINE__);
}

module_init(apt7l05sf_init);
module_exit(apt7l05sf_exit);

MODULE_AUTHOR("siyrra");
MODULE_DESCRIPTION("R16(D9) apt7l05sf ,whw0701@126.com");
MODULE_LICENSE("GPL");

