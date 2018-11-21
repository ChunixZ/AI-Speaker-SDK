#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/regmap.h>
#include <linux/slab.h>

#define led_dev_bus_num 1	

static struct i2c_client *led1_client = NULL;

static struct i2c_board_info led1_info = {	
	I2C_BOARD_INFO("led1", 0x3f),
};

static int led_dev_init(void)
{
	struct i2c_adapter *i2c_adap;
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

	i2c_adap = i2c_get_adapter(led_dev_bus_num);
	led1_client = i2c_new_device(i2c_adap, &led1_info);
	i2c_put_adapter(i2c_adap);
	
	return 0;
}

static void led_dev_exit(void)
{
	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	i2c_unregister_device(led1_client);
}

module_init(led_dev_init);
module_exit(led_dev_exit);
MODULE_LICENSE("GPL");
