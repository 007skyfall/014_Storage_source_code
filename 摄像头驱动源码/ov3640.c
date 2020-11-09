#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <media/v4l2-device.h>
#include <media/v4l2-subdev.h>
#include <media/v4l2-i2c-drv.h>
#include <media/ov3640_platform.h>

/* add by leesheen */
#include <linux/regulator/consumer.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/regs-gpio.h>
#include <mach/regs-clock.h>
/* end add */

#ifdef CONFIG_VIDEO_SAMSUNG_V4L2
#include <linux/videodev2_samsung.h>
#endif

#include "ov3640.h"

#define OV3640_DRIVER_NAME	"OV3640"

/* Default resolution & pixelformat. plz ref ov2655_platform.h */
#define DEFAULT_RES		WVGA	/* Index of resoultion */
#define DEFAUT_FPS_INDEX	OV3640_15FPS
#define DEFAULT_FMT		V4L2_PIX_FMT_UYVY	/* YUV422 */

/* add by leesheen */
#ifndef CONFIG_TC4_EVT
struct regulator *vdd18_cam_regulator = NULL;	
struct regulator *vdd28_cam_regulator = NULL;

struct regulator *VDD28_AF = NULL;
struct regulator *VDD28_CAM = NULL;
struct regulator *VDD18_CAM = NULL;
#endif
/* end add */

struct ov3640_userset {
	signed int exposure_bias;	/* V4L2_CID_EXPOSURE */
	unsigned int ae_lock;
	unsigned int awb_lock;
	unsigned int auto_wb;	/* V4L2_CID_CAMERA_WHITE_BALANCE */
	unsigned int manual_wb;	/* V4L2_CID_WHITE_BALANCE_PRESET */
	unsigned int wb_temp;	/* V4L2_CID_WHITE_BALANCE_TEMPERATURE */
	unsigned int effect;	/* Color FX (AKA Color tone) */
	unsigned int contrast;	/* V4L2_CID_CONTRAST */
	unsigned int saturation;	/* V4L2_CID_SATURATION */
	unsigned int sharpness;		/* V4L2_CID_SHARPNESS */
	unsigned int glamour;
};



struct ov3640_state {
	struct ov3640_platform_data *pdata;
	struct v4l2_subdev sd;
	struct v4l2_pix_format pix;
	struct v4l2_fract timeperframe;
	struct ov3640_userset userset;
	int framesize_index;
	int freq;	/* MCLK in KHz */
	int is_mipi;
	int isize;
	int ver;
	int fps;
	int check_previewdata;
};

/********notice*********/
enum
{
	OV3640_PREVIEW_SVGA,
};
struct ov3640_enum_framesize
{
	unsigned int index;
	unsigned int width;
	unsigned int height;
};

struct ov3640_enum_framesize ov3640_framesize_list[] = {
	{ OV3640_PREVIEW_SVGA, 640, 480 }
};

static inline struct ov3640_state *to_state(struct v4l2_subdev *sd)
{
	return container_of(sd, struct ov3640_state, sd);
}

static const char *ov3640_querymenu_wb_preset[] =
{ "WB Tungsten", "WB Fluorescent", "WB sunny", "WB cloudy", NULL };

static const char *ov3640_querymenu_effect_mode[] =
{ "Effect Sepia", "Effect Aqua", "Effect Monochrome", "Effect Negative",
		"Effect Sketch", NULL };

static const char *ov3640_querymenu_ev_bias_mode[] =
{ "-3EV", "-2,1/2EV", "-2EV", "-1,1/2EV", "-1EV", "-1/2EV", "0", "1/2EV",
		"1EV", "1,1/2EV", "2EV", "2,1/2EV", "3EV", NULL };


static struct v4l2_queryctrl ov3640_controls[] =
{
	{
		/*
		 * For now, we just support in preset type
		 * to be close to generic WB system,
		 * we define color temp range for each preset
		 */
		.id = V4L2_CID_WHITE_BALANCE_TEMPERATURE,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "White balance in kelvin",
		.minimum = 0,
		.maximum = 10000,
		.step = 1,
		.default_value = 0, /* FIXME */
	},
	{
		.id = V4L2_CID_WHITE_BALANCE_PRESET,
		.type = V4L2_CTRL_TYPE_MENU,
		.name = "White balance preset",
		.minimum = 0,
		.maximum = ARRAY_SIZE(ov3640_querymenu_wb_preset) - 2,
		.step = 1,
		.default_value = 0,
	},
	{
		.id = V4L2_CID_CAMERA_WHITE_BALANCE,
		.type = V4L2_CTRL_TYPE_BOOLEAN,
		.name = "Auto white balance",
		.minimum = 0,
		.maximum = 1,
		.step = 1,
		.default_value = 0,
	},
	{
		.id = V4L2_CID_EXPOSURE,
		.type = V4L2_CTRL_TYPE_MENU,
		.name = "Exposure bias",
		.minimum = 0,
		.maximum = ARRAY_SIZE(ov3640_querymenu_ev_bias_mode) - 2,
		.step = 1,
		.default_value =
		(ARRAY_SIZE(ov3640_querymenu_ev_bias_mode) - 2) / 2,
		/* 0 EV */
	},
	{
		.id = V4L2_CID_CAMERA_EFFECT,
		.type = V4L2_CTRL_TYPE_MENU,
		.name = "Image Effect",
		.minimum = 0,
		.maximum = ARRAY_SIZE(ov3640_querymenu_effect_mode) - 2,
		.step = 1,
		.default_value = 0,
	},
	{
		.id = V4L2_CID_CAMERA_CONTRAST,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "Contrast",
		.minimum = 0,
		.maximum = 4,
		.step = 1,
		.default_value = 2,
	},
	{
		.id = V4L2_CID_CAMERA_SATURATION,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "Saturation",
		.minimum = 0,
		.maximum = 4,
		.step = 1,
		.default_value = 2,
	},
	{
		.id = V4L2_CID_CAMERA_SHARPNESS,
		.type = V4L2_CTRL_TYPE_INTEGER,
		.name = "Sharpness",
		.minimum = 0,
		.maximum = 4,
		.step = 1,
		.default_value = 2,
	},
};



static int ov3640_i2c_write(struct v4l2_subdev *sd, unsigned char i2c_data[],
		unsigned char length)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	unsigned char buf[length], i;
	struct i2c_msg msg =
	{ client->addr, 0, length, buf };

	for (i = 0; i < length; i++)
		buf[i] = i2c_data[i];

	return i2c_transfer(client->adapter, &msg, 1) == 1 ? 0 : -EIO;
}

static inline int OV3640_write(struct v4l2_subdev *sd, unsigned short addr,
		u8 val)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct i2c_msg msg[1];
	unsigned char reg[3];

	if (!client->adapter)
		return -ENODEV;

	msg->addr = client->addr;
	msg->flags = 0;
	msg->len = 3;
	msg->buf = reg;

	reg[0] = addr >> 8;
	reg[1] = addr & 0xFF;
	reg[2] = val & 0xFF;

	if (i2c_transfer(client->adapter, msg, 1) != 1)
	{
		printk("ov3640 i2c_transfer failed\n");
		return -EIO;
	}

	return 0;
}

static int __OV3640_init_2bytes(struct v4l2_subdev *sd, unsigned short *reg[],
		int total)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int ret = -EINVAL, i;
	unsigned short *item;
	unsigned char bytes;

	for (i = 0; i < total; i++)
	{
		item = (unsigned short *) &reg[i];

		if (item[0] == REG_DELAY)
		{
			mdelay(item[1]);
			ret = 0;
		}
		/*else if (item[0] == REG_ID)
		 {
		 DDI_I2C_Read(sd, 0x0001, 2, &bytes, 1); // 1byte read
		 printk("===== cam sensor ov2655 ID : 0x%x ", bytes);
		 DDI_I2C_Read(sd, 0x0002, 2, &bytes, 1); // 1byte read
		 }*/
		else
		{
			ret = OV3640_write(sd, item[0], item[1]);
		}

		if (ret < 0)
			v4l_info(client, "%s: register set failed\n", __func__);
	}

	return ret;
}


static int ov3640_write_regs(struct v4l2_subdev *sd, unsigned char regs[],
		int size)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int i, err;

	for (i = 0; i < size; i++)
	{
		err = ov3640_i2c_write(sd, &regs[i], sizeof(regs[i]));
		if (err < 0)
			v4l_info(client, "%s: register set failed\n", __func__);
	}

	return 0;
}

/* add by leesheen */
static int ov3640_power(int flag)
{
	int err;
	printk("leesheen : sensor is power %s\n",flag == 1 ?"on":"off");
	//Attention: Power On the all the camera module when system turn on
	//Here only control the reset&&powerdown pin
  
	/* Camera A */
	if(flag == 1)
	{	
		// for TC4 zhuxuezhen
		//poweron

#ifndef CONFIG_TC4_EVT
       	regulator_enable(vdd18_cam_regulator); 
	       udelay(10);
		regulator_enable(vdd28_cam_regulator); 
	       udelay(10);

		regulator_enable(VDD28_AF); 
	       udelay(10);
		regulator_enable(VDD28_CAM); 
	       udelay(10);
		regulator_enable(VDD18_CAM); 
	       udelay(10);
#endif
		err = gpio_request(EXYNOS4_GPL0(3), "GPL0_3");
		if (err)
			printk(KERN_ERR "#### failed to request GPL0_3 ####\n");
		s3c_gpio_setpull(EXYNOS4_GPL0(3), S3C_GPIO_PULL_NONE);
		gpio_direction_output(EXYNOS4_GPL0(3), 0);
		gpio_free(EXYNOS4_GPL0(3));
		
		//VDD
		//PMIC_Control_Set(PMIC_CONTROL_VDD18_CAM,POWERON);

		//reset
		err = gpio_request(EXYNOS4_GPL0(1), "GPL0_1");
		if (err)
			printk(KERN_ERR "#### failed to request GPL0_1 ####\n");
		s3c_gpio_setpull(EXYNOS4_GPL0(1), S3C_GPIO_PULL_NONE);
		gpio_direction_output(EXYNOS4_GPL0(1), 1);
		msleep(5);
		gpio_direction_output(EXYNOS4_GPL0(1), 0);
		msleep(5);
		gpio_direction_output(EXYNOS4_GPL0(1), 1);
		
		gpio_free(EXYNOS4_GPL0(1));
		msleep(1);
		//VDD_PLL
		//PMIC_Control_Set(PMIC_CONTROL_VDD28_CAM,POWERON);
		
	}
	else
	{		
		// for TC4 zhuxuezhen
		//PMIC_Control_Set(PMIC_CONTROL_VDD28_CAM,POWEROFF);

		err = gpio_request(EXYNOS4_GPL0(1), "GPL0_1");
		if (err)
			printk(KERN_ERR "#### failed to request GPL0_1 ####\n");
		s3c_gpio_setpull(EXYNOS4_GPL0(1), S3C_GPIO_PULL_NONE);
		gpio_direction_output(EXYNOS4_GPL0(1), 0);
		gpio_free(EXYNOS4_GPL0(1));

		//powerdown
		err = gpio_request(EXYNOS4_GPL0(3), "GPL0_3");
		if (err)
			printk(KERN_ERR "#### failed to request GPL0_3 ####\n");
		s3c_gpio_setpull(EXYNOS4_GPL0(3), S3C_GPIO_PULL_NONE);
		gpio_direction_output(EXYNOS4_GPL0(3), 1);
		gpio_free(EXYNOS4_GPL0(3));
#ifndef CONFIG_TC4_EVT
       	regulator_disable(vdd18_cam_regulator); 
	       udelay(10);
		regulator_disable(vdd28_cam_regulator); 
	       udelay(10);

		regulator_disable(VDD28_AF);
		udelay(10);
		regulator_disable(VDD28_CAM);
		udelay(10);
		regulator_disable(VDD18_CAM);
		udelay(10);
#endif	   
		
		//PMIC_Control_Set(PMIC_CONTROL_VDD18_CAM,POWEROFF);
		
	}

	return 0;
}
/* end add */



static int ov3640_init(struct v4l2_subdev *sd, u32 val)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct ov3640_state *state = to_state(sd);
	int err = -EINVAL;

	v4l_info(client, "%s: camera initialization start\n", __func__);

	ov3640_power(1);

	err = __OV3640_init_2bytes(sd, (unsigned short **)ov3640_init_reg, OV3640_INIT_REGS);

	if (err < 0)
	{
		/* This is preview fail */
		state->check_previewdata = 100;
		v4l_err(client, "%s: camera initialization failed. err(%d)\n",
				__func__, state->check_previewdata);
		return -EIO;
	}

	/* This is preview success */
	state->check_previewdata = 0;
	return 0;
}


static int ov3640_s_config(struct v4l2_subdev *sd, int irq, void *platform_data)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct ov3640_state *state = to_state(sd);
	struct ov3640_platform_data *pdata;

	dev_info(&client->dev, "fetching platform data\n");

	pdata = client->dev.platform_data;

	if (!pdata)
	{
		dev_err(&client->dev, "%s: no platform data\n", __func__);
		return -ENODEV;
	}

	/*
	 * Assign default format and resolution
	 * Use configured default information in platform data
	 * or without them, use default information in driver
	 */
	if (!(pdata->default_width && pdata->default_height))
	{
		/* TODO: assign driver default resolution */
	}
	else
	{
		state->pix.width = pdata->default_width;
		state->pix.height = pdata->default_height;
	}

	if (!pdata->pixelformat)
		state->pix.pixelformat = DEFAULT_FMT;
	else
		state->pix.pixelformat = pdata->pixelformat;

	if (!pdata->freq)
		state->freq = 24000000; /* 24MHz default */
	else
		state->freq = pdata->freq;

	if (!pdata->is_mipi)
	{
		state->is_mipi = 0;
		dev_info(&client->dev, "parallel mode\n");
	}
	else
		state->is_mipi = pdata->is_mipi;

	return 0;
}

//add by leesheen
static int ov3640_s_stream(struct v4l2_subdev *sd, int enable)
{
	struct ov3640_state *state = to_state(sd);
	int err = 0;

	printk("s_stream: mode = %d\n", enable);

	switch (enable) {
	case STREAM_MODE_CAM_OFF:
#if 0
		if (state->sensor_mode == SENSOR_CAMERA) {
#ifdef SUPPORT_FACTORY_TEST
			if (state->check_dataline)
			//	err = sr200pc20_check_dataline(sd, 0);
			else
#endif
				if (state->pdata->is_mipi) {
					printk("%s: is_mipi in\n");
					//err = sr200pc20_control_stream(sd,
					//		STREAM_STOP);
				}
		}
		break;
#endif
	case STREAM_MODE_CAM_ON:
#if 0
		if ((state->sensor_mode == SENSOR_CAMERA)
		    && (state->req_fmt.priv == V4L2_PIX_FMT_MODE_CAPTURE)) {
			printk("%s: in cam_on in\n");
		//	err = sr200pc20_set_capture_start(sd);
		}
		else
		//	err = sr200pc20_set_preview_start(sd);
			printk("%s: in cam_on else\n");
		break;

#endif
	case STREAM_MODE_MOVIE_ON:
		printk("%s: do nothing(movie on)!!\n", __func__);
		break;

	case STREAM_MODE_MOVIE_OFF:
		printk("%s: do nothing(movie off)!!\n", __func__);
		break;

#ifdef CONFIG_VIDEO_IMPROVE_STREAMOFF
	case STREAM_MODE_WAIT_OFF:
		printk("STREAM_MODE_WAIT_OFF\n");
		//err = sr200pc20_wait_steamoff(sd);
		break;
#endif
	default:
		printk("%s: ERROR, Invalid stream mode %d\n",
					__func__, enable);
		err = -EINVAL;
		break;
	}

	/*********************************************************
	remove by cym 20130222
	because if the device dose not exit the Camera App will no response
	*********************************************************/
	//CHECK_ERR_MSG(err, "stream on(off) fail")
	/* end remove */
	
	return 0;
}



static int ov3640_queryctrl(struct v4l2_subdev *sd, struct v4l2_queryctrl *qc)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ov3640_controls); i++)
	{
		if (ov3640_controls[i].id == qc->id)
		{
			memcpy(qc, &ov3640_controls[i], sizeof(struct v4l2_queryctrl));
			return 0;
		}
	}

	return -EINVAL;
}

const char **ov3640_ctrl_get_menu(u32 id)
{
	switch (id)
	{
	case V4L2_CID_WHITE_BALANCE_PRESET:
		return ov3640_querymenu_wb_preset;

	case V4L2_CID_CAMERA_EFFECT:
		return ov3640_querymenu_effect_mode;

	case V4L2_CID_EXPOSURE:
		return ov3640_querymenu_ev_bias_mode;

	default:
		return v4l2_ctrl_get_menu(id);
	}
}

static int ov3640_querymenu(struct v4l2_subdev *sd, struct v4l2_querymenu *qm)
{
	struct v4l2_queryctrl qctrl;

	qctrl.id = qm->id;
	ov3640_queryctrl(sd, &qctrl);

	return v4l2_ctrl_query_menu(qm, &qctrl, ov3640_ctrl_get_menu(qm->id));
}



static int ov3640_g_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct ov3640_state *state = to_state(sd);
	struct ov3640_userset userset = state->userset;
	int err = 0;

	switch (ctrl->id)
	{
	case V4L2_CID_CAMERA_WHITE_BALANCE:
		ctrl->value = userset.auto_wb;
		break;
	case V4L2_CID_WHITE_BALANCE_PRESET:
		ctrl->value = userset.manual_wb;
		break;
	case V4L2_CID_CAMERA_EFFECT:
		ctrl->value = userset.effect;
		break;
	case V4L2_CID_CAMERA_CONTRAST:
		ctrl->value = userset.contrast;
		break;
	case V4L2_CID_CAMERA_SATURATION:
		ctrl->value = userset.saturation;
		break;
	case V4L2_CID_CAMERA_SHARPNESS:
		ctrl->value = userset.saturation;
		break;
	case V4L2_CID_CAM_JPEG_MAIN_SIZE:
	case V4L2_CID_CAM_JPEG_MAIN_OFFSET:
	case V4L2_CID_CAM_JPEG_THUMB_SIZE:
	case V4L2_CID_CAM_JPEG_THUMB_OFFSET:
	case V4L2_CID_CAM_JPEG_POSTVIEW_OFFSET:
	case V4L2_CID_CAM_JPEG_MEMSIZE:
	case V4L2_CID_CAM_JPEG_QUALITY:
//	case V4L2_CID_CAMERA_AUTO_FOCUS_RESULT_FIRST:
	case V4L2_CID_CAM_DATE_INFO_YEAR:
	case V4L2_CID_CAM_DATE_INFO_MONTH:
	case V4L2_CID_CAM_DATE_INFO_DATE:
	case V4L2_CID_CAM_SENSOR_VER:
	case V4L2_CID_CAM_FW_MINOR_VER:
	case V4L2_CID_CAM_FW_MAJOR_VER:
	case V4L2_CID_CAM_PRM_MINOR_VER:
	case V4L2_CID_CAM_PRM_MAJOR_VER:
	//case V4L2_CID_ESD_INT:
//	case V4L2_CID_CAMERA_GET_ISO:
//	case V4L2_CID_CAMERA_GET_SHT_TIME:
	case V4L2_CID_CAMERA_OBJ_TRACKING_STATUS:
	case V4L2_CID_CAMERA_SMART_AUTO_STATUS:
		ctrl->value = 0;
		break;
	case V4L2_CID_EXPOSURE:
		ctrl->value = userset.exposure_bias;
		break;
	default:
		dev_err(&client->dev, "%s: no such ctrl\n", __func__);
		/* err = -EINVAL; */
		break;
	}

	return err;
}

static int ov3640_reset(struct v4l2_subdev *sd)
{
	return ov3640_init(sd, 0);
}

static int ov3640_s_ctrl(struct v4l2_subdev *sd, struct v4l2_control *ctrl)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	struct ov3640_state *state = to_state(sd);
	int err = 0;
	int value = ctrl->value;

	switch (ctrl->id)
	{

	case V4L2_CID_CAMERA_FLASH_MODE:
	case V4L2_CID_CAMERA_BRIGHTNESS:
		break;
	case V4L2_CID_CAMERA_WHITE_BALANCE:
		dev_dbg(&client->dev, "%s: V4L2_CID_CAMERA_WHITE_BALANCE\n", __func__);

		if (value <= WHITE_BALANCE_AUTO)
		{
			err = ov3640_write_regs(sd,
					(unsigned char *) ov3640_regs_awb_enable[value],
					sizeof(ov3640_regs_awb_enable[value]));
		}
		else
		{
			err = ov3640_write_regs(sd,
					(unsigned char *) ov3640_regs_wb_preset[value - 2],
					sizeof(ov3640_regs_wb_preset[value - 2]));
		}
		break;
	case V4L2_CID_CAMERA_EFFECT:
		dev_dbg(&client->dev, "%s: V4L2_CID_CAMERA_EFFECT\n", __func__);
		err = ov3640_write_regs(sd,
				(unsigned char *) ov3640_regs_color_effect[value - 1],
				sizeof(ov3640_regs_color_effect[value - 1]));
		break;
	case V4L2_CID_CAMERA_ISO:
	case V4L2_CID_CAMERA_METERING:
		break;
	case V4L2_CID_CAMERA_CONTRAST:
		dev_dbg(&client->dev, "%s: V4L2_CID_CAMERA_CONTRAST\n", __func__);
		err = ov3640_write_regs(sd,
				(unsigned char *) ov3640_regs_contrast_bias[value],
				sizeof(ov3640_regs_contrast_bias[value]));
		break;
	case V4L2_CID_CAMERA_SATURATION:
		dev_dbg(&client->dev, "%s: V4L2_CID_CAMERA_SATURATION\n", __func__);
		err = ov3640_write_regs(sd,
				(unsigned char *) ov3640_regs_saturation_bias[value],
				sizeof(ov3640_regs_saturation_bias[value]));
		break;
	case V4L2_CID_CAMERA_SHARPNESS:
		dev_dbg(&client->dev, "%s: V4L2_CID_CAMERA_SHARPNESS\n", __func__);
		err = ov3640_write_regs(sd,
				(unsigned char *) ov3640_regs_sharpness_bias[value],
				sizeof(ov3640_regs_sharpness_bias[value]));
		break;
	case V4L2_CID_CAMERA_WDR:
	case V4L2_CID_CAMERA_FACE_DETECTION:
	case V4L2_CID_CAMERA_FOCUS_MODE:
	case V4L2_CID_CAM_JPEG_QUALITY:
	case V4L2_CID_CAMERA_SCENE_MODE:
	case V4L2_CID_CAMERA_GPS_LATITUDE:
	case V4L2_CID_CAMERA_GPS_LONGITUDE:
	case V4L2_CID_CAMERA_GPS_TIMESTAMP:
	case V4L2_CID_CAMERA_GPS_ALTITUDE:
	case V4L2_CID_CAMERA_OBJECT_POSITION_X:
	case V4L2_CID_CAMERA_OBJECT_POSITION_Y:
	case V4L2_CID_CAMERA_SET_AUTO_FOCUS:
	case V4L2_CID_CAMERA_FRAME_RATE:
		break;
	case V4L2_CID_CAM_PREVIEW_ONOFF:
		if (state->check_previewdata == 0)
			err = 0;
		else
			err = -EIO;
		break;
	case V4L2_CID_CAMERA_CHECK_DATALINE:
	case V4L2_CID_CAMERA_CHECK_DATALINE_STOP:
		break;
	case V4L2_CID_CAMERA_RESET:
		dev_dbg(&client->dev, "%s: V4L2_CID_CAMERA_RESET\n", __func__);
		err = ov3640_reset(sd);
		break;
	case V4L2_CID_EXPOSURE:
		dev_dbg(&client->dev, "%s: V4L2_CID_EXPOSURE\n", __func__);
		err = ov3640_write_regs(sd,
				(unsigned char *) ov3640_regs_ev_bias[value],
				sizeof(ov3640_regs_ev_bias[value]));
		break;
	default:
		dev_err(&client->dev, "%s: no such control\n", __func__);
		/* err = -EINVAL; */
		break;
	}

	if (err < 0)
		dev_dbg(&client->dev, "%s: vidioc_s_ctrl failed\n", __func__);

	return err;
}

/************************************************************************/
/* modify by leesheen */
#ifdef NEW_CAM_DRV
static int ov3640_g_mbus_fmt(struct v4l2_subdev *sd,
			struct v4l2_mbus_framefmt *fmt)
#else
static int ov3640_g_fmt(struct v4l2_subdev *sd, struct v4l2_format *fmt)
#endif
{
	int err = 0;

	printk("%s\n", __func__);

	return err;
}

/* modify by leesheen */
#ifdef NEW_CAM_DRV
static int ov3640_s_mbus_fmt(struct v4l2_subdev *sd,
			struct v4l2_mbus_framefmt *fmt)
#else
static int ov3640_s_fmt(struct v4l2_subdev *sd, struct v4l2_format *fmt)
#endif
{
	int err = 0;

	struct ov3640_state *state = to_state(sd);
	u32 *width = NULL, *height = NULL;

	//cam_trace("E\n");
	/*
	 * Just copying the requested format as of now.
	 * We need to check here what are the formats the camera support, and
	 * set the most appropriate one according to the request from FIMC
	 */
#ifdef NEW_CAM_DRV
	//v4l2_fill_pix_format(&state->req_fmt, fmt);
	v4l2_fill_pix_format(&state->pix, fmt);
	state->pix.priv = fmt->field;
#else
	memcpy(&state->pix, &fmt->fmt.pix, sizeof(fmt->fmt.pix));
#endif
	printk("%s\n", __func__);

	switch (state->pix.priv) {
	case V4L2_PIX_FMT_MODE_PREVIEW:
		//width = &state->preview_frmsizes.width;
		//height = &state->preview_frmsizes.height;
		width = 640;
		height = 480;
		break;

	case V4L2_PIX_FMT_MODE_CAPTURE:
		//width = &state->capture_frmsizes.width;
		//height = &state->capture_frmsizes.height;
		width = 640;
		height = 480;
		break;

	default:
//		dev_err("%s: ERROR, inavlid FMT Mode(%d)\n",
//			__func__, state->pix.priv);
		return -EINVAL;
	}

	printk("%s\n", __func__);
	if ((*width != state->pix.width) ||
		(*height != state->pix.height)) {
//		dev_err("%s: ERROR, Invalid size. width= %d, height= %d\n",
//			__func__, state->pix.width, state->pix.height);
	}
	printk("%s\n", __func__);

	return err;
}

static int ov3640_enum_framesizes(struct v4l2_subdev *sd,
		struct v4l2_frmsizeenum *fsize)
{
	struct ov3640_state *state = to_state(sd);
	int num_entries = sizeof(ov3640_framesize_list) 
				/ sizeof(struct ov3640_enum_framesize);
	struct ov3640_enum_framesize *elem;
	int index = 0;
	int i = 0;

	printk("%s\n", __func__);

	/* The camera interface should read this value, this is the resolution
	 * at which the sensor would provide framedata to the camera i/f
	 *
	 * In case of image capture,
	 * this returns the default camera resolution (WVGA)
	 */
	fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;

	index = state->framesize_index;

	for (i = 0; i < num_entries; i++)
	{
		elem = &ov3640_framesize_list[i];
		if (elem->index == index)
		{
			fsize->discrete.width = ov3640_framesize_list[index].width;
			fsize->discrete.height = ov3640_framesize_list[index].height;
			return 0;
		}
	}

	return -EINVAL;
}

static int ov3640_enum_frameintervals(struct v4l2_subdev *sd,
		struct v4l2_frmivalenum *fival)
{
	int err = 0;

	printk("%s\n", __func__);

	return err;
}

static int ov3640_enum_fmt(struct v4l2_subdev *sd, struct v4l2_fmtdesc *fmtdesc)
{
	int err = 0;

	printk("%s\n", __func__);

	return err;
}
/* modify by leesheen */
#ifdef NEW_CAM_DRV
static int ov3640_try_mbus_fmt(struct v4l2_subdev *sd,
			struct v4l2_mbus_framefmt *fmt)
#else
static int ov3640_try_fmt(struct v4l2_subdev *sd, struct v4l2_format *fmt)
#endif
{
	int err = 0;

	printk("%s\n", __func__);

	return err;
}

static int ov3640_g_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *param)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int err = 0;

	dev_dbg(&client->dev, "%s\n", __func__);

	printk("%s\n", __func__);

	return err;
}
static int ov3640_s_parm(struct v4l2_subdev *sd, struct v4l2_streamparm *param)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);
	int err = 0;

	printk("%s\n", __func__);

	dev_dbg(&client->dev, "%s: numerator %d, denominator: %d\n", __func__,
			param->parm.capture.timeperframe.numerator,
			param->parm.capture.timeperframe.denominator);

	return err;
}

static const struct v4l2_subdev_core_ops ov3640_core_ops =
{
	.init = ov3640_init, /* initializing API */
//	.s_config = ov3640_s_config, /* Fetch platform data */
//	.queryctrl = ov3640_queryctrl,
//	.querymenu = ov3640_querymenu,
	.g_ctrl = ov3640_g_ctrl,
	.s_ctrl = ov3640_s_ctrl,
};

static const struct v4l2_subdev_video_ops ov3640_video_ops =
{
#ifdef NEW_CAM_DRV
	.g_mbus_fmt = ov3640_g_mbus_fmt,
	.s_mbus_fmt = ov3640_s_mbus_fmt,
#else
	.g_fmt = ov3640_g_fmt,
	.s_fmt = ov3640_s_fmt,
#endif
	.s_stream = ov3640_s_stream,
	.enum_framesizes = ov3640_enum_framesizes,
//	.enum_frameintervals = ov3640_enum_frameintervals,
	//.enum_fmt = ov3640_enum_fmt,
#ifdef NEW_CAM_DRV
	.try_mbus_fmt = ov3640_try_mbus_fmt,
#else
	.try_fmt = ov3640_try_fmt,
#endif
	.g_parm = ov3640_g_parm,
	.s_parm = ov3640_s_parm,
};

static const struct v4l2_subdev_ops ov3640_ops =
{
	.core = &ov3640_core_ops,
	.video = &ov3640_video_ops,
};

static int ov3640_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	struct ov3640_state *state;
	struct v4l2_subdev *sd;
	int ret = 0;

	state = kzalloc(sizeof(struct ov3640_state), GFP_KERNEL);
	if (state == NULL)
		return -ENOMEM;

	/* add by leesheen */
#ifndef CONFIG_TC4_EVT
	vdd18_cam_regulator = regulator_get(NULL, "vdd18_2m");
	
	if (IS_ERR(vdd18_cam_regulator)) {
		printk("%s: failed to get %s\n", __func__, "vdd18_2m");
		ret = -ENODEV;
		goto err_regulator;
	}
	//regulator_enable(vdd18_cam_regulator); 	

	vdd28_cam_regulator = regulator_get(NULL, "vdda28_2m");
	
	if (IS_ERR(vdd28_cam_regulator)) {
		printk("%s: failed to get %s\n", __func__, "vdda28_2m");
		ret = -ENODEV;
		goto err_regulator;
	}
	//regulator_enable(vdd28_cam_regulator); 
	
	VDD28_AF = regulator_get(NULL, "vdd28_af");
	
	if (IS_ERR(VDD28_AF)) {
		printk("%s: failed to get %s\n", __func__, "VDD28_AF");
		ret = -ENODEV;
		goto err_regulator;
	}

	VDD28_CAM = regulator_get(NULL, "vdd28_cam");
	
	if (IS_ERR(VDD28_CAM)) {
		printk("%s: failed to get %s\n", __func__, "VDD28_CAM");
		ret = -ENODEV;
		goto err_regulator;
	}

	VDD18_CAM = regulator_get(NULL, "vdd18_cam");
	
	if (IS_ERR(VDD18_CAM)) {
		printk("%s: failed to get %s\n", __func__, "VDD18_CAM");
		ret = -ENODEV;
		goto err_regulator;
	}

#endif
	/* end add */

	sd = &state->sd;
	strcpy(sd->name, OV3640_DRIVER_NAME);

	/* Registering subdev */
	v4l2_i2c_subdev_init(sd, client, &ov3640_ops);

/*modify by leesheen 20121121 */
#if 0
	err = sr200pc20_s_config(sd, 0, client->dev.platform_data);
	CHECK_ERR_MSG(err, "fail to s_config\n");
#else
	ret = ov3640_s_config(sd, 0, client->dev.platform_data);
	if(ret)
	{
		printk("%s(%d): Fail to s_config\n", __FUNCTION__, __LINE__);
		goto err_regulator;
	}
#endif


	dev_info(&client->dev, "ov3640 has been probed\n");

err_regulator:
	return 0;

/* add by leesheen */
#if 0	
	regulator_put(vdd18_cam_regulator);	
	regulator_put(vdd28_cam_regulator);

	regulator_put(VDD28_AF);
	regulator_put(VDD28_CAM);
	regulator_put(VDD18_CAM);
	return ret;
#endif



}

static int ov3640_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);

	v4l2_device_unregister_subdev(sd);
	kfree(to_state(sd));

	//add by leesheen
	ov3640_power(0);
	return 0;
}

static const struct i2c_device_id ov3640_id[] = {
	{ OV3640_DRIVER_NAME, 0 },
	{ }, 
};
MODULE_DEVICE_TABLE(i2c, ov2655_id);

static struct v4l2_i2c_driver_data v4l2_i2c_data =
{
	.name = OV3640_DRIVER_NAME,
	.probe = ov3640_probe,
	.remove = __devexit_p(ov3640_remove),
	.id_table = ov3640_id,
};

MODULE_DESCRIPTION("OV Electronics ov3640 UXGA camera driver");
MODULE_AUTHOR("George Ji <jihq@farsight.com.cn>");
MODULE_LICENSE("GPL");

