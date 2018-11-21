#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/uaccess.h>


//#include <linux/sys_config.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>



#include <linux/leds.h>
#include <linux/delay.h>
#include <linux/unistd.h>


//#include "z8ida_data/MaxxAudio_Cmd_FULL.c"
//#include "1/MaxxAudio_Cmd_FULL.c"
//#include "1/MaxxAudio_48K_FULL.c"


#include <linux/io.h>
#include <mach/sys_config.h>

static int major;
static struct class *class;
static struct i2c_client *z8ida_client;

typedef unsigned short		u16;



/* 传入: buf[0] : addr
 * 输出: buf[0] : data
 */
static ssize_t z8ida_read(struct file * file, char __user *buf, size_t count, loff_t *off)
{
	unsigned char addr, data;
	//============wu  2017 07 13============//
	char data_user[3];
	copy_from_user(data_user, buf, 3);
	//======================================//
	return 1;
}

/* buf[0] : addr
 * buf[1] : data
 */
static ssize_t z8ida_write(struct file *file, const char __user *buf, size_t count, loff_t *off)
{
	unsigned char ker_buf[128];
	unsigned char addr, data;
	int ret1;
	int count_num = 0;

	memset(ker_buf,0,128);

	copy_from_user(ker_buf, buf, count);

	if(ker_buf[0] == 0x00)
		{
			i2c_master_send( z8ida_client, &ker_buf[1] , (ker_buf[1]+2));
		}
	else if (ker_buf[0] == 0x01)
		{
			i2c_master_recv(z8ida_client,ker_buf,30);
			copy_to_user(buf, ker_buf, 30);
		}
	return 0;	
}

static struct file_operations z8ida_fops = {
	.owner = THIS_MODULE,
	.read  = z8ida_read,
	.write = z8ida_write,
};


static int  z8ida_probe(struct i2c_client *client,
				  const struct i2c_device_id *id)
{
	z8ida_client = client;
		
	printk("####wu#####%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	major = register_chrdev(0, "z8ida", &z8ida_fops);
	class = class_create(THIS_MODULE, "z8ida");
	device_create(class, NULL, MKDEV(major, 0), NULL, "z8ida"); /* /dev/at24cxx */
	return 0;
}

static int  z8ida_remove(struct i2c_client *client)
{
	//printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	device_destroy(class, MKDEV(major, 0));
	class_destroy(class);
	unregister_chrdev(major, "z8ida");
	return 0;
}

static const struct i2c_device_id z8ida_id_table[] = {
	{ "z8ida", 0 },
	{}
};


/* 1. 分配/设置i2c_driver */
static struct i2c_driver z8ida_driver = {
	.driver	= {
		.name	= "z8ida",
		.owner	= THIS_MODULE,
	},
	.probe		= z8ida_probe,
	.remove		= z8ida_remove,
	.id_table	= z8ida_id_table,
};

static int z8ida_drv_init(void)
{
	/* 2. 注册i2c_driver */
	i2c_add_driver(&z8ida_driver);
	return 0;
}

static void z8ida_drv_exit(void)
{
	i2c_del_driver(&z8ida_driver);
}


module_init(z8ida_drv_init);
module_exit(z8ida_drv_exit);
MODULE_LICENSE("GPL");


