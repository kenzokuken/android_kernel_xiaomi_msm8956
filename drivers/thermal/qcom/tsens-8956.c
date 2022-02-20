// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2019-2022, Linaro Limited
 */

#include <linux/bitops.h>
#include <linux/regmap.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include "tsens.h"

/* eeprom layout data for msm8956/76 */
#define BASE0_MASK	0xff
#define BASE1_MASK	0xff
#define BASE1_SHIFT	8

#define S0_P1_MASK	0x3f00
#define S1_P1_MASK	0x3f00000
#define S2_P1_MASK	0x3f
#define S3_P1_MASK	0x3f000
#define S4_P1_MASK	0x3f00
#define S5_P1_MASK	0x3f00000
#define S6_P1_MASK	0x3f
#define S7_P1_MASK	0x3f000
#define S8_P1_MASK	0x1f8
#define S9_P1_MASK	0x1f8000
#define S10_P1_MASK	0xf8000000
#define S10_P1_MASK_1	0x1

#define S0_P2_MASK	0xfc000
#define S1_P2_MASK	0xfc000000
#define S2_P2_MASK	0xfc0
#define S3_P2_MASK	0xfc0000
#define S4_P2_MASK	0xfc000
#define S5_P2_MASK	0xfc000000
#define S6_P2_MASK	0xfc0
#define S7_P2_MASK	0xfc0000
#define S8_P2_MASK	0x7e00
#define S9_P2_MASK	0x7e00000
#define S10_P2_MASK	0x7e

#define S0_P1_SHIFT	8
#define S1_P1_SHIFT	20
#define S2_P1_SHIFT	0
#define S3_P1_SHIFT	12
#define S4_P1_SHIFT	8
#define S5_P1_SHIFT	20
#define S6_P1_SHIFT	0
#define S7_P1_SHIFT	12
#define S8_P1_SHIFT	3
#define S9_P1_SHIFT	15
#define S10_P1_SHIFT	27
#define S10_P1_SHIFT_1	0

#define S0_P2_SHIFT	14
#define S1_P2_SHIFT	26
#define S2_P2_SHIFT	6
#define S3_P2_SHIFT	18
#define S4_P2_SHIFT	14
#define S5_P2_SHIFT	26
#define S6_P2_SHIFT	6
#define S7_P2_SHIFT	18
#define S8_P2_SHIFT	9
#define S9_P2_SHIFT	21
#define S10_P2_SHIFT	1

#define CAL_SEL_MASK	0x3

#define CAL_DEGC_PT1	30
#define CAL_DEGC_PT2	120
#define SLOPE_FACTOR	1000
#define SLOPE_DEFAULT	3200

static void compute_intercept_slope(struct tsens_priv *priv,
			      u32 *p1, u32 *p2, u32 mode)
{
	int i;

	priv->sensor[0].slope = 3313;
	priv->sensor[1].slope = 3275;
	priv->sensor[2].slope = 3320;
	priv->sensor[3].slope = 3246;
	priv->sensor[4].slope = 3279;
	priv->sensor[5].slope = 3257;
	priv->sensor[6].slope = 3234;
	priv->sensor[7].slope = 3269;
	priv->sensor[8].slope = 3255;
	priv->sensor[9].slope = 3239;
	priv->sensor[10].slope = 3286;

	for (i = 0; i < priv->num_sensors; i++) {
		priv->sensor[i].offset = (p1[i] * SLOPE_FACTOR) -
				(CAL_DEGC_PT1 *
				priv->sensor[i].slope);
	}
}

static int calibrate_8956(struct tsens_priv *priv)
{
	int base0 = 0, base1 = 0, i;
	u32 p1[11], p2[11];
	int mode = 0, tmp = 0;
	u32 *qfprom_cdata;

	qfprom_cdata = (u32 *)qfprom_read(priv->dev, "calib");
	if (IS_ERR(qfprom_cdata))
		return PTR_ERR(qfprom_cdata);

	mode = (qfprom_cdata[4] & CAL_SEL_MASK);
	dev_dbg(priv->dev, "calibration mode is %d\n", mode);

	switch (mode) {
	case TWO_PT_CALIB:
		base1 = (qfprom_cdata[2] & BASE1_MASK) >> BASE1_SHIFT;
		p2[0] = (qfprom_cdata[0] & S0_P2_MASK) >> S0_P2_SHIFT;
		p2[1] = (qfprom_cdata[0] & S1_P2_MASK) >> S1_P2_SHIFT;
		p2[2] = (qfprom_cdata[1] & S2_P2_MASK) >> S2_P2_SHIFT;
		p2[3] = (qfprom_cdata[1] & S3_P2_MASK) >> S3_P2_SHIFT;
		p2[4] = (qfprom_cdata[2] & S4_P2_MASK) >> S4_P2_SHIFT;
		p2[5] = (qfprom_cdata[2] & S5_P2_MASK) >> S5_P2_SHIFT;
		p2[6] = (qfprom_cdata[3] & S6_P2_MASK) >> S6_P2_SHIFT;
		p2[7] = (qfprom_cdata[3] & S7_P2_MASK) >> S7_P2_SHIFT;
		p2[8] = (qfprom_cdata[4] & S8_P2_MASK) >> S8_P2_SHIFT;
		p2[9] = (qfprom_cdata[4] & S9_P2_MASK) >> S9_P2_SHIFT;
		p2[10] = (qfprom_cdata[5] & S10_P2_MASK) >> S10_P2_SHIFT;

		for (i = 0; i < priv->num_sensors; i++)
			p2[i] = ((base1 + p2[i]) << 2);
		fallthrough;
	case ONE_PT_CALIB2:
		base0 = qfprom_cdata[0] & BASE0_MASK;
		p1[0] = (qfprom_cdata[0] & S0_P1_MASK) >> S0_P1_SHIFT;
		p1[1] = (qfprom_cdata[0] & S1_P1_MASK) >> S1_P1_SHIFT;
		p1[2] = (qfprom_cdata[1] & S2_P1_MASK) >> S2_P1_SHIFT;
		p1[3] = (qfprom_cdata[1] & S3_P1_MASK) >> S3_P1_SHIFT;
		p1[4] = (qfprom_cdata[2] & S4_P1_MASK) >> S4_P1_SHIFT;
		p1[5] = (qfprom_cdata[2] & S5_P1_MASK) >> S5_P1_SHIFT;
		p1[6] = (qfprom_cdata[3] & S6_P1_MASK) >> S6_P1_SHIFT;
		p1[7] = (qfprom_cdata[3] & S7_P1_MASK) >> S7_P1_SHIFT;
		p1[8] = (qfprom_cdata[4] & S8_P1_MASK) >> S8_P1_SHIFT;
		p1[9] = (qfprom_cdata[4] & S9_P1_MASK) >> S9_P1_SHIFT;
		p1[10] = (qfprom_cdata[4] & S10_P1_MASK) >> S10_P1_SHIFT;
		tmp = (qfprom_cdata[5] & S10_P1_MASK_1) << S10_P1_SHIFT_1;
		p1[10] |= tmp;

		for (i = 0; i < priv->num_sensors; i++)
			p1[i] = (((base0) + p1[i]) << 2);
		break;
	default:
		for (i = 0; i < priv->num_sensors; i++) {
			p1[i] = 500;
			p2[i] = 780;
		}
		break;
	}

	compute_intercept_slope(priv, p1, p2, mode);
	kfree(qfprom_cdata);

	return 0;
}

static const struct tsens_ops ops_8956 = {
	.init		= init_common,
	.calibrate	= calibrate_8956,
	.get_temp	= get_temp_common,
};

/* Valid for both MSM8956 and MSM8976. */
struct tsens_plat_data data_8956 = {
	.num_sensors	= 11,
	.ops		= &ops_8956,
	.hw_ids		= (unsigned int[]){0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
