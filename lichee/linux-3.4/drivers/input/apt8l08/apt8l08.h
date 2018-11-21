#ifndef apt8l08_H
#define apt8l08_H

/*apt8l08 REG*/

/*ÒôÁ¿°´¼ü*/
#define vol_1 0x1
#define vol_2 0x2
#define vol_3 0x4
#define vol_4 0x8
#define vol_5 0x10 

struct apt8l08_pad_data{
	u8 asr;
	u8 isr;
	u8 slidsr;
};
struct apt8l08_config_info{
	int tpad_used;
	struct gpio_config apt8l08_int;
    struct gpio_config apt8l08_en;
}config_info;

struct apt8l08_pad{
	struct i2c_client *client;
	struct input_dev *apt8l08_input;
	u8 device_id;
};

#endif
