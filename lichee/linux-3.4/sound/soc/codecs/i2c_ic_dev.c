#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/err.h>
#include <linux/regmap.h>
#include <linux/slab.h>


static struct i2c_board_info wuinfo = {	
	I2C_BOARD_INFO("z8ida", 0x2A),
};

static struct i2c_client *z8idaclient;

static int z8idadev_init(void)
{
	struct i2c_adapter *i2c_adap;

	i2c_adap = i2c_get_adapter(0);
	z8idaclient = i2c_new_device(i2c_adap, &wuinfo);
	i2c_put_adapter(i2c_adap);
	
	return 0;
}

static void z8idadev_exit(void)
{
	i2c_unregister_device(z8idaclient);
}


module_init(z8idadev_init);
module_exit(z8idadev_exit);
MODULE_LICENSE("GPL");


