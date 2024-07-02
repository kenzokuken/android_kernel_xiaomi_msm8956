# SPDX-License-Identifier: GPL-2.0
ifneq ($(CONFIG_ARCH_MSM8956),y)
dtb-$(CONFIG_ARCH_QCOM)	+= apq8016-sbc.dtb
dtb-$(CONFIG_ARCH_QCOM)	+= apq8096-db820c.dtb
dtb-$(CONFIG_ARCH_QCOM)	+= ipq8074-hk01.dtb
dtb-$(CONFIG_ARCH_QCOM)	+= msm8916-mtp.dtb
dtb-$(CONFIG_ARCH_QCOM)	+= msm8992-bullhead-rev-101.dtb
dtb-$(CONFIG_ARCH_QCOM)	+= msm8994-angler-rev-101.dtb
dtb-$(CONFIG_ARCH_QCOM)	+= msm8996-mtp.dtb
dtb-$(CONFIG_ARCH_QCOM)	+= sdm845-mtp.dtb
endif

ifeq ($(CONFIG_BUILD_ARM64_DT_OVERLAY),y)
dtbo-$(CONFIG_ARCH_MSM8956) += msm8956-cdp-overlay.dtbo \
	msm8956-mtp-overlay.dtbo \
	msm8956-qrd-overlay.dtbo

msm8956-mtp-overlay.dtbo-base := msm8956.dtb \
	msm8956-pmi8950.dtb
msm8956-cdp-overlay.dtbo-base := msm8956.dtb \
	msm8956-pmi8950.dtb
msm8956-qrd-overlay.dtbo-base := msm8956.dtb
else
dtb-$(CONFIG_ARCH_MSM8956) += msm8956-cdp.dtb \
	msm8956-mtp.dtb \
	msm8956-qrd-skun.dtb \
	msm8956-v1.1-cdp.dtb \
	msm8956-v1.1-mtp.dtb \
	msm8956-v1.1-qrd-skun.dtb
endif

ifeq ($(CONFIG_BUILD_ARM64_DT_OVERLAY),y)
dtbo-$(CONFIG_ARCH_MSM8976) += msm8976-cdp-overlay.dtbo \
	msm8976-mtp-overlay.dtbo \
	msm8976-qrd-overlay.dtbo

msm8976-mtp-overlay.dtbo-base := msm8976.dtb \
	msm8976-pmi8950.dtb
msm8976-cdp-overlay.dtbo-base := msm8976.dtb \
	msm8976-pmi8950.dtb
msm8976-qrd-overlay.dtbo-base := msm8976.dtb
else
dtb-$(CONFIG_ARCH_MSM8976) += msm8976-cdp.dtb \
	msm8976-mtp.dtb \
	msm8976-qrd-skun.dtb \
	msm8976-v1.1-cdp.dtb \
	msm8976-v1.1-mtp.dtb \
	msm8976-v1.1-qrd-skun.dtb
endif

always		:= $(dtb-y)
subdir-y	:= $(dts-dirs)
clean-files    := *.dtb *.dtbo
