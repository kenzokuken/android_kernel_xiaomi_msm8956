/*
   BlueZ - Bluetooth protocol stack for Linux

   Copyright (C) 2014 Intel Corporation

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2 as
   published by the Free Software Foundation;

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
   IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) AND AUTHOR(S) BE LIABLE FOR ANY
   CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

   ALL LIABILITY, INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PATENTS,
   COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS, RELATING TO USE OF THIS
   SOFTWARE IS DISCLAIMED.
*/

#include <linux/debugfs.h>

#include <net/bluetooth/bluetooth.h>
#include <net/bluetooth/hci_core.h>

#include "ecdh_helper.h"
#include "smp.h"
#include "selftest.h"

#if IS_ENABLED(CONFIG_BT_SELFTEST_ECDH)

static const u8 priv_a_1[32] __initconst = {
	0xbd, 0x1a, 0x3c, 0xcd, 0xa6, 0xb8, 0x99, 0x58,
	0x99, 0xb7, 0x40, 0xeb, 0x7b, 0x60, 0xff, 0x4a,
	0x50, 0x3f, 0x10, 0xd2, 0xe3, 0xb3, 0xc9, 0x74,
	0x38, 0x5f, 0xc5, 0xa3, 0xd4, 0xf6, 0x49, 0x3f,
};
static const u8 priv_b_1[32] __initconst = {
	0xfd, 0xc5, 0x7f, 0xf4, 0x49, 0xdd, 0x4f, 0x6b,
	0xfb, 0x7c, 0x9d, 0xf1, 0xc2, 0x9a, 0xcb, 0x59,
	0x2a, 0xe7, 0xd4, 0xee, 0xfb, 0xfc, 0x0a, 0x90,
	0x9a, 0xbb, 0xf6, 0x32, 0x3d, 0x8b, 0x18, 0x55,
};
static const u8 pub_a_1[64] __initconst = {
	0xe6, 0x9d, 0x35, 0x0e, 0x48, 0x01, 0x03, 0xcc,
	0xdb, 0xfd, 0xf4, 0xac, 0x11, 0x91, 0xf4, 0xef,
	0xb9, 0xa5, 0xf9, 0xe9, 0xa7, 0x83, 0x2c, 0x5e,
	0x2c, 0xbe, 0x97, 0xf2, 0xd2, 0x03, 0xb0, 0x20,

	0x8b, 0xd2, 0x89, 0x15, 0xd0, 0x8e, 0x1c, 0x74,
	0x24, 0x30, 0xed, 0x8f, 0xc2, 0x45, 0x63, 0x76,
	0x5c, 0x15, 0x52, 0x5a, 0xbf, 0x9a, 0x32, 0x63,
	0x6d, 0xeb, 0x2a, 0x65, 0x49, 0x9c, 0x80, 0xdc,
};
static const u8 pub_b_1[64] __initconst = {
	0x90, 0xa1, 0xaa, 0x2f, 0xb2, 0x77, 0x90, 0x55,
	0x9f, 0xa6, 0x15, 0x86, 0xfd, 0x8a, 0xb5, 0x47,
	0x00, 0x4c, 0x9e, 0xf1, 0x84, 0x22, 0x59, 0x09,
	0x96, 0x1d, 0xaf, 0x1f, 0xf0, 0xf0, 0xa1, 0x1e,

	0x4a, 0x21, 0xb1, 0x15, 0xf9, 0xaf, 0x89, 0x5f,
	0x76, 0x36, 0x8e, 0xe2, 0x30, 0x11, 0x2d, 0x47,
	0x60, 0x51, 0xb8, 0x9a, 0x3a, 0x70, 0x56, 0x73,
	0x37, 0xad, 0x9d, 0x42, 0x3e, 0xf3, 0x55, 0x4c,
};
static const u8 dhkey_1[32] __initconst = {
	0x98, 0xa6, 0xbf, 0x73, 0xf3, 0x34, 0x8d, 0x86,
	0xf1, 0x66, 0xf8, 0xb4, 0x13, 0x6b, 0x79, 0x99,
	0x9b, 0x7d, 0x39, 0x0a, 0xa6, 0x10, 0x10, 0x34,
	0x05, 0xad, 0xc8, 0x57, 0xa3, 0x34, 0x02, 0xec,
};

static const u8 priv_a_2[32] __initconst = {
	0x63, 0x76, 0x45, 0xd0, 0xf7, 0x73, 0xac, 0xb7,
	0xff, 0xdd, 0x03, 0x72, 0xb9, 0x72, 0x85, 0xb4,
	0x41, 0xb6, 0x5d, 0x0c, 0x5d, 0x54, 0x84, 0x60,
	0x1a, 0xa3, 0x9a, 0x3c, 0x69, 0x16, 0xa5, 0x06,
};
static const u8 priv_b_2[32] __initconst = {
	0xba, 0x30, 0x55, 0x50, 0x19, 0xa2, 0xca, 0xa3,
	0xa5, 0x29, 0x08, 0xc6, 0xb5, 0x03, 0x88, 0x7e,
	0x03, 0x2b, 0x50, 0x73, 0xd4, 0x2e, 0x50, 0x97,
	0x64, 0xcd, 0x72, 0x0d, 0x67, 0xa0, 0x9a, 0x52,
};
static const u8 pub_a_2[64] __initconst = {
	0xdd, 0x78, 0x5c, 0x74, 0x03, 0x9b, 0x7e, 0x98,
	0xcb, 0x94, 0x87, 0x4a, 0xad, 0xfa, 0xf8, 0xd5,
	0x43, 0x3e, 0x5c, 0xaf, 0xea, 0xb5, 0x4c, 0xf4,
	0x9e, 0x80, 0x79, 0x57, 0x7b, 0xa4, 0x31, 0x2c,

	0x4f, 0x5d, 0x71, 0x43, 0x77, 0x43, 0xf8, 0xea,
	0xd4, 0x3e, 0xbd, 0x17, 0x91, 0x10, 0x21, 0xd0,
	0x1f, 0x87, 0x43, 0x8e, 0x40, 0xe2, 0x52, 0xcd,
	0xbe, 0xdf, 0x98, 0x38, 0x18, 0x12, 0x95, 0x91,
};
static const u8 pub_b_2[64] __initconst = {
	0xcc, 0x00, 0x65, 0xe1, 0xf5, 0x6c, 0x0d, 0xcf,
	0xec, 0x96, 0x47, 0x20, 0x66, 0xc9, 0xdb, 0x84,
	0x81, 0x75, 0xa8, 0x4d, 0xc0, 0xdf, 0xc7, 0x9d,
	0x1b, 0x3f, 0x3d, 0xf2, 0x3f, 0xe4, 0x65, 0xf4,

	0x79, 0xb2, 0xec, 0xd8, 0xca, 0x55, 0xa1, 0xa8,
	0x43, 0x4d, 0x6b, 0xca, 0x10, 0xb0, 0xc2, 0x01,
	0xc2, 0x33, 0x4e, 0x16, 0x24, 0xc4, 0xef, 0xee,
	0x99, 0xd8, 0xbb, 0xbc, 0x48, 0xd0, 0x01, 0x02,
};
static const u8 dhkey_2[32] __initconst = {
	0x69, 0xeb, 0x21, 0x32, 0xf2, 0xc6, 0x05, 0x41,
	0x60, 0x19, 0xcd, 0x5e, 0x94, 0xe1, 0xe6, 0x5f,
	0x33, 0x07, 0xe3, 0x38, 0x4b, 0x68, 0xe5, 0x62,
	0x3f, 0x88, 0x6d, 0x2f, 0x3a, 0x84, 0x85, 0xab,
};

static const u8 priv_a_3[32] __initconst = {
	0xbd, 0x1a, 0x3c, 0xcd, 0xa6, 0xb8, 0x99, 0x58,
	0x99, 0xb7, 0x40, 0xeb, 0x7b, 0x60, 0xff, 0x4a,
	0x50, 0x3f, 0x10, 0xd2, 0xe3, 0xb3, 0xc9, 0x74,
	0x38, 0x5f, 0xc5, 0xa3, 0xd4, 0xf6, 0x49, 0x3f,
};
static const u8 pub_a_3[64] __initconst = {
	0xe6, 0x9d, 0x35, 0x0e, 0x48, 0x01, 0x03, 0xcc,
	0xdb, 0xfd, 0xf4, 0xac, 0x11, 0x91, 0xf4, 0xef,
	0xb9, 0xa5, 0xf9, 0xe9, 0xa7, 0x83, 0x2c, 0x5e,
	0x2c, 0xbe, 0x97, 0xf2, 0xd2, 0x03, 0xb0, 0x20,

	0x8b, 0xd2, 0x89, 0x15, 0xd0, 0x8e, 0x1c, 0x74,
	0x24, 0x30, 0xed, 0x8f, 0xc2, 0x45, 0x63, 0x76,
	0x5c, 0x15, 0x52, 0x5a, 0xbf, 0x9a, 0x32, 0x63,
	0x6d, 0xeb, 0x2a, 0x65, 0x49, 0x9c, 0x80, 0xdc,
};
static const u8 dhkey_3[32] __initconst = {
	0x2d, 0xab, 0x00, 0x48, 0xcb, 0xb3, 0x7b, 0xda,
	0x55, 0x7b, 0x8b, 0x72, 0xa8, 0x57, 0x87, 0xc3,
	0x87, 0x27, 0x99, 0x32, 0xfc, 0x79, 0x5f, 0xae,
	0x7c, 0x1c, 0xf9, 0x49, 0xe6, 0xd7, 0xaa, 0x70,
};

static int __init test_ecdh_sample(struct crypto_kpp *tfm, const u8 priv_a[32],
				   const u8 priv_b[32], const u8 pub_a[64],
				   const u8 pub_b[64], const u8 dhkey[32])
{
	u8 *tmp, *dhkey_a, *dhkey_b;
	int ret;

	tmp = kmalloc(64, GFP_KERNEL);
	if (!tmp)
		return -EINVAL;

	dhkey_a = &tmp[0];
	dhkey_b = &tmp[32];

	ret = set_ecdh_privkey(tfm, priv_a);
	if (ret)
		goto out;

	ret = compute_ecdh_secret(tfm, pub_b, dhkey_a);
	if (ret)
		goto out;

	if (memcmp(dhkey_a, dhkey, 32)) {
		ret = -EINVAL;
		goto out;
	}

	ret = set_ecdh_privkey(tfm, priv_b);
	if (ret)
		goto out;

	ret = compute_ecdh_secret(tfm, pub_a, dhkey_b);
	if (ret)
		goto out;

	if (memcmp(dhkey_b, dhkey, 32))
		ret = -EINVAL;
	/* fall through*/
out:
	kfree(tmp);
	return ret;
}

static char test_ecdh_buffer[32];

static ssize_t test_ecdh_read(struct file *file, char __user *user_buf,
			      size_t count, loff_t *ppos)
{
	return simple_read_from_buffer(user_buf, count, ppos, test_ecdh_buffer,
				       strlen(test_ecdh_buffer));
}

static const struct file_operations test_ecdh_fops = {
	.open		= simple_open,
	.read		= test_ecdh_read,
	.llseek		= default_llseek,
};

static int __init test_ecdh(void)
{
	struct crypto_kpp *tfm;
	ktime_t calltime, delta, rettime;
	unsigned long long duration = 0;
	int err;

	calltime = ktime_get();

	tfm = crypto_alloc_kpp("ecdh-nist-p256", 0, 0);
	if (IS_ERR(tfm)) {
		BT_ERR("Unable to create ECDH crypto context");
		err = PTR_ERR(tfm);
		goto done;
	}

	err = test_ecdh_sample(tfm, priv_a_1, priv_b_1, pub_a_1, pub_b_1,
			       dhkey_1);
	if (err) {
		BT_ERR("ECDH sample 1 failed");
		goto done;
	}

	err = test_ecdh_sample(tfm, priv_a_2, priv_b_2, pub_a_2, pub_b_2,
			       dhkey_2);
	if (err) {
		BT_ERR("ECDH sample 2 failed");
		goto done;
	}

	err = test_ecdh_sample(tfm, priv_a_3, priv_a_3, pub_a_3, pub_a_3,
			       dhkey_3);
	if (err) {
		BT_ERR("ECDH sample 3 failed");
		goto done;
	}

	crypto_free_kpp(tfm);

	rettime = ktime_get();
	delta = ktime_sub(rettime, calltime);
	duration = (unsigned long long) ktime_to_ns(delta) >> 10;

	BT_INFO("ECDH test passed in %llu usecs", duration);

done:
	if (!err)
		snprintf(test_ecdh_buffer, sizeof(test_ecdh_buffer),
			 "PASS (%llu usecs)\n", duration);
	else
		snprintf(test_ecdh_buffer, sizeof(test_ecdh_buffer), "FAIL\n");

	debugfs_create_file("selftest_ecdh", 0444, bt_debugfs, NULL,
			    &test_ecdh_fops);

	return err;
}

#else

static inline int test_ecdh(void)
{
	return 0;
}

#endif

static int __init run_selftest(void)
{
	int err;

	BT_INFO("Starting self testing");

	err = test_ecdh();
	if (err)
		goto done;

	err = bt_selftest_smp();

done:
	BT_INFO("Finished self testing");

	return err;
}

#if IS_MODULE(CONFIG_BT)

/* This is run when CONFIG_BT_SELFTEST=y and CONFIG_BT=m and is just a
 * wrapper to allow running this at module init.
 *
 * If CONFIG_BT_SELFTEST=n, then this code is not compiled at all.
 */
int __init bt_selftest(void)
{
	return run_selftest();
}

#else

/* This is run when CONFIG_BT_SELFTEST=y and CONFIG_BT=y and is run
 * via late_initcall() as last item in the initialization sequence.
 *
 * If CONFIG_BT_SELFTEST=n, then this code is not compiled at all.
 */
static int __init bt_selftest_init(void)
{
	return run_selftest();
}
late_initcall(bt_selftest_init);

#endif
