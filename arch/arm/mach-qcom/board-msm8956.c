// SPDX-License-Identifier: GPL-2.0
// Copyright (c) 2013-2015, The Linux Foundation. All rights reserved.
// Copyright (c) 2023, Benarji Anand <benarji385@gmail.com>

#include <linux/kernel.h>
#include "board-dt.h"
#include <asm/mach/map.h>
#include <asm/mach/arch.h>

static const char *msm8956_dt_match[] __initconst = {
	"qcom,msm8956",
	NULL
};

static void __init msm8956_init(void)
{
	board_dt_populate(NULL);
}

DT_MACHINE_START(MSM8956_DT,
	"Qualcomm Technologies, Inc. MSM8956 (Flattened Device Tree)")
	.init_machine		= msm8956_init,
	.dt_compat		= msm8956_dt_match,
MACHINE_END

