// SPDX-License-Identifier: GPL-2.0
// Copyright (c) 2015, The Linux Foundation. All rights reserved.
// Copyright (c) 2023, Benarji Anand <benarji385@gmail.com>

#include "mpm.h"

const struct mpm_pin mpm_msm8976_gic_chip_data[] = {
	{2, 216}, /* tsens_upper_lower_int */
	{49, 172}, /* usb1_hs_async_wakeup_irq */
	{58, 166}, /* usb_hs_irq */
	{53, 104}, /* mdss_irq */
	{62, 222}, /* ee0_krait_hlos_spmi_periph_irq */
	{-1},
};
