/* ----------------------------------------------------------------------- *
 *
 *   Copyright 2000-2008 H. Peter Anvin - All Rights Reserved
 *   Copyright 2009 Intel Corporation; author: H. Peter Anvin
 *   Copyright 2018 Alex Wiens
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 675 Mass Ave, Cambridge MA 02139,
 *   USA; either version 2 of the License, or (at your option) any later
 *   version; incorporated herein by reference.
 *
 * ----------------------------------------------------------------------- */

/*
 * x86 MSR access device
 *
 * This device is accessed by lseek() to the appropriate register number
 * and then read/write in chunks of 8 bytes.  A larger size means multiple
 * reads or writes of the  register.
 *
 * This driver uses /dev/cpu/%d/msr where %d is the minor number, and on
 * an SMP box will direct the access to CPU %d.
 *
 * Differences to the original msr module:
 * - CAP_SYS_RAWIO capability check is removed
 * - reading/writing whitelists with allowed registers is added
 * - dynamic device number is registered
 * - devnode paths are changed to /dev/cpu/?/amsr
 * - devnode mode bits are set to 666 
 */

#include <linux/module.h>

#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/smp.h>
#include <linux/major.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cpu.h>
#include <linux/notifier.h>
#include <linux/uaccess.h>
#include <linux/gfp.h>
#include <linux/security.h>
#include <linux/cdev.h>

#include <asm/cpufeature.h>
#include <asm/msr.h>

static struct class *msr_class;

#define IA32_TIME_STAMP_COUNTER     0x10
#define IA32_MPERF                  0xE7
#define IA32_APERF                  0xE8
#define MSR_TEMPERATURE_TARGET      0x1A2
#define IA32_PACKAGE_THERM_STATUS   0x1B1
#define IA32_THERM_STATUS           0x19C
#define MSR_PKG_RAPL_POWER_LIMIT        0x610

u32 read_whitelist[] = {
MSR_RAPL_POWER_UNIT,
MSR_PKG_ENERGY_STATUS,
MSR_PP0_ENERGY_STATUS,
MSR_PP1_ENERGY_STATUS,
MSR_DRAM_ENERGY_STATUS,
IA32_TIME_STAMP_COUNTER,
IA32_MPERF,
IA32_APERF,
MSR_TEMPERATURE_TARGET,
IA32_PACKAGE_THERM_STATUS,
IA32_THERM_STATUS,
MSR_PKG_RAPL_POWER_LIMIT,
MSR_PKG_PERF_STATUS,
MSR_PKG_POWER_INFO,
MSR_PP0_POWER_LIMIT,
MSR_PP0_POLICY,
MSR_PP0_PERF_STATUS,
MSR_PP1_POWER_LIMIT,
MSR_PP1_POLICY,
MSR_DRAM_POWER_LIMIT,
MSR_DRAM_PERF_STATUS,
MSR_DRAM_POWER_INFO
};

u32 write_whitelist[] = {
MSR_IA32_MPERF,
MSR_IA32_APERF
};

static dev_t device;
static unsigned int major_device = 0;

struct minor_cpu {
	struct cdev cdev;
	u32 loaded; // 0 no, 1 yes
};

struct minor_cpu *minor_devices = NULL;


static ssize_t msr_read(struct file *file, char __user *buf,
			size_t count, loff_t *ppos)
{
	u32 __user *tmp = (u32 __user *) buf;
	u32 data[2];
	u32 reg = *ppos;
	int cpu = iminor(file_inode(file));
	int err = 0;
	ssize_t bytes = 0;

	int wix;
	int whitelist_num = sizeof(read_whitelist) / sizeof(u32);
	for(wix = 0; wix < whitelist_num; ++wix) {
		if (read_whitelist[wix] == reg) {
			break;
		}
	}
	if (wix == whitelist_num) {
		return -EIO;
	}

	if (count % 8)
		return -EINVAL;	/* Invalid chunk size */

	for (; count; count -= 8) {
		err = rdmsr_safe_on_cpu(cpu, reg, &data[0], &data[1]);
		if (err)
			break;
		if (copy_to_user(tmp, &data, 8)) {
			err = -EFAULT;
			break;
		}
		tmp += 2;
		bytes += 8;
	}

	return bytes ? bytes : err;
}

static ssize_t msr_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *ppos)
{
	const u32 __user *tmp = (const u32 __user *)buf;
	u32 data[2];
	u32 reg = *ppos;
	int cpu = iminor(file_inode(file));
	int err = 0;
	ssize_t bytes = 0;

	int wix;
	int whitelist_num = sizeof(write_whitelist) / sizeof(u32);
	for(wix = 0; wix < whitelist_num; ++wix) {
		if (write_whitelist[wix] == reg) {
			break;
		}
	}
	if (wix == whitelist_num) {
		return -EIO;
	}

	if (get_securelevel() > 0)
		return -EPERM;


	if (count % 8)
		return -EINVAL;	/* Invalid chunk size */

	for (; count; count -= 8) {
		if (copy_from_user(&data, tmp, 8)) {
			err = -EFAULT;
			break;
		}
		err = wrmsr_safe_on_cpu(cpu, reg, data[0], data[1]);
		if (err)
			break;
		tmp += 2;
		bytes += 8;
	}

	return bytes ? bytes : err;
}

static long msr_ioctl(struct file *file, unsigned int ioc, unsigned long arg)
{
	u32 __user *uregs = (u32 __user *)arg;
	u32 regs[8];
	int cpu = iminor(file_inode(file));
	int err;

	switch (ioc) {
	case X86_IOC_RDMSR_REGS:
		if (!(file->f_mode & FMODE_READ)) {
			err = -EBADF;
			break;
		}
		if (copy_from_user(&regs, uregs, sizeof regs)) {
			err = -EFAULT;
			break;
		}
		err = rdmsr_safe_regs_on_cpu(cpu, regs);
		if (err)
			break;
		if (copy_to_user(uregs, &regs, sizeof regs))
			err = -EFAULT;
		break;

	case X86_IOC_WRMSR_REGS:
		if (!(file->f_mode & FMODE_WRITE)) {
			err = -EBADF;
			break;
		}
		if (get_securelevel() > 0) {
			err = -EPERM;
			break;
		}
		if (copy_from_user(&regs, uregs, sizeof regs)) {
			err = -EFAULT;
			break;
		}
		err = wrmsr_safe_regs_on_cpu(cpu, regs);
		if (err)
			break;
		if (copy_to_user(uregs, &regs, sizeof regs))
			err = -EFAULT;
		break;

	default:
		err = -ENOTTY;
		break;
	}

	return err;
}

static int msr_open(struct inode *inode, struct file *file)
{
	unsigned int cpu = iminor(file_inode(file));
	struct cpuinfo_x86 *c;

	//if (!capable(CAP_SYS_RAWIO))
	//	return -EPERM;

	if (cpu >= nr_cpu_ids || !cpu_online(cpu))
		return -ENXIO;	/* No such CPU */

	c = &cpu_data(cpu);
	if (!cpu_has(c, X86_FEATURE_MSR))
		return -EIO;	/* MSR not supported */

	return 0;
}

/*
 * File operations we support
 */
static const struct file_operations msr_fops = {
	.owner = THIS_MODULE,
	.llseek = no_seek_end_llseek,
	.read = msr_read,
	.write = msr_write,
	.open = msr_open,
	.unlocked_ioctl = msr_ioctl,
	.compat_ioctl = msr_ioctl,
};

static int msr_device_create(int cpu)
{
	int err = 0;
	struct device *dev;

	cdev_init(&(minor_devices[cpu].cdev), &msr_fops);
	minor_devices[cpu].cdev.owner = THIS_MODULE;

	err = cdev_add(&(minor_devices[cpu].cdev), MKDEV(major_device, cpu), 1);
	if (err < 0) {
		printk("amsr: cdev_add failed\n");
		return err;
	}
	minor_devices[cpu].loaded = 1;

	dev = device_create(msr_class, NULL, MKDEV(major_device, cpu), NULL,
			    "amsr%d", cpu);
	if (IS_ERR(dev)) {
		printk("amsr: device_create failed with %ld\n", PTR_ERR(dev));
		cdev_del(&(minor_devices[cpu].cdev));
		minor_devices[cpu].loaded = 0;
	}
	return IS_ERR(dev) ? PTR_ERR(dev) : 0;
}

static void msr_device_destroy(int cpu)
{
	if (minor_devices[cpu].loaded == 1) {
		device_destroy(msr_class, MKDEV(major_device, cpu));
		cdev_del(&(minor_devices[cpu].cdev));
		minor_devices[cpu].loaded = 0;
	}
}

static int msr_class_cpu_callback(struct notifier_block *nfb,
				  unsigned long action, void *hcpu)
{
	unsigned int cpu = (unsigned long)hcpu;
	int err = 0;

	switch (action) {
	case CPU_UP_PREPARE:
		err = msr_device_create(cpu);
		break;
	case CPU_UP_CANCELED:
	case CPU_UP_CANCELED_FROZEN:
	case CPU_DEAD:
		msr_device_destroy(cpu);
		break;
	}
	return notifier_from_errno(err);
}

static struct notifier_block __refdata msr_class_cpu_notifier = {
	.notifier_call = msr_class_cpu_callback,
};

static char *msr_devnode(struct device *dev, umode_t *mode)
{
	return kasprintf(GFP_KERNEL, "cpu/%u/amsr", MINOR(dev->devt));
}

static int msr_dev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

static int __init msr_init(void)
{
	int i, err = 0;
	i = 0;

	if (alloc_chrdev_region(&device, 0, NR_CPUS, "amsr") < 0) {
		printk("amsr: Cannot allocate device number.\n");
		return -EIO;
	}
	major_device = MAJOR(device);

	msr_class = class_create(THIS_MODULE, "amsr");
	if (IS_ERR(msr_class)) {
		err = PTR_ERR(msr_class);
		printk("amsr: unable to create class\n");
		goto out_chrdev;
	}
	msr_class->devnode = msr_devnode;
    msr_class->dev_uevent = msr_dev_uevent;


//	if (__register_chrdev(MSR_MAJOR, 0, NR_CPUS, "cpu/amsr", &msr_fops)) {
//		printk(KERN_ERR "amsr: unable to get major %d for amsr\n",
//		       MSR_MAJOR);
//		err = -EBUSY;
//		goto out;
//	}
//	msr_class = class_create(THIS_MODULE, "msr");
//	if (IS_ERR(msr_class)) {
//		err = PTR_ERR(msr_class);
//		goto out_chrdev;
//	}
//	msr_class->devnode = msr_devnode;

	minor_devices = kzalloc(sizeof(struct minor_cpu) * NR_CPUS, GFP_KERNEL);
	if (minor_devices == NULL) {
		printk("amsr: unable to allocate memory for cpu devices\n");
		goto out_chrdev;
	}

	cpu_notifier_register_begin();
	for_each_online_cpu(i) {
		err = msr_device_create(i);
		if (err != 0)
			goto out_class;
	}
	__register_hotcpu_notifier(&msr_class_cpu_notifier);
	cpu_notifier_register_done();

	err = 0;
	return 0;

out_class:
	i = 0;
	for_each_online_cpu(i)
		msr_device_destroy(i);
	cpu_notifier_register_done();
	class_destroy(msr_class);
out_chrdev:
	__unregister_chrdev(MSR_MAJOR, 0, NR_CPUS, "cpu/amsr");
	if (minor_devices != NULL) {
		kfree(minor_devices);
		minor_devices = NULL;
	}
	return err;
}

static void __exit msr_exit(void)
{
	int cpu = 0;

	cpu_notifier_register_begin();
	for_each_online_cpu(cpu)
		msr_device_destroy(cpu);
	class_destroy(msr_class);
	//__unregister_chrdev(MSR_MAJOR, 0, NR_CPUS, "cpu/amsr");
	__unregister_hotcpu_notifier(&msr_class_cpu_notifier);
	unregister_chrdev_region(device, NR_CPUS);
	if (minor_devices != NULL) {
		kfree(minor_devices);
		minor_devices = NULL;
	}
	cpu_notifier_register_done();
}

module_init(msr_init);
module_exit(msr_exit)

MODULE_AUTHOR("H. Peter Anvin <hpa@zytor.com>");
MODULE_DESCRIPTION("x86 generic AMSR driver");
MODULE_LICENSE("GPL");
