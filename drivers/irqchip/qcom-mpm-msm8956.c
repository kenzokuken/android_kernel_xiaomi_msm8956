// SPDX-License-Identifier: GPL-2.0
// Copyright (c) 2022, Benarji Anand <benarji385@gmail.com>

#include <soc/qcom/mpm.h>

const struct mpm_pin mpm_msm8956_gic_chip_data[] = {
	{2, 216}, /* tsens_upper_lower_int */
	{37, 252}, /* qmp_usb3_lfps_rxterm_irq -> ss_phy_irq */
	{49, 168}, /* qusb2phy_dpse_hv -> hs_phy_irq*/
	{53, 104}, /* mdss_irq */
	{58, 168}, /* qusb2phy_dmse_hv -> hs_phy_irq*/
	{88, 222}, /* ee0_krait_hlos_spmi_periph_irq */
	{-1},
};
