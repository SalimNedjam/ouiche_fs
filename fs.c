// SPDX-License-Identifier: GPL-2.0
/*
 * ouiche_fs - a simple educational filesystem for Linux
 *
 * Copyright (C) 2018  Redha Gouicem <redha.gouicem@lip6.fr>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <asm/io.h>
#include <linux/kernel.h>
#include <linux/delay.h>

#include "ouichefs.h"
#include "ioctl_ouichefs.h"


static int major;
dev_t devNo;
struct class *pClass;




/*
 * Mount a ouiche_fs partition
 */
struct dentry *ouichefs_mount(struct file_system_type *fs_type, int flags,
			      const char *dev_name, void *data)
{
	struct dentry *dentry = NULL;

	dentry = mount_bdev(fs_type, flags, dev_name, data,
			    ouichefs_fill_super);
	if (IS_ERR(dentry))
		pr_err("'%s' mount failure\n", dev_name);
	else
		pr_info("'%s' mount success\n", dev_name);

	return dentry;
}

/*
 * Unmount a ouiche_fs partition
 */
void ouichefs_kill_sb(struct super_block *sb)
{
	kill_block_super(sb);

	pr_info("unmounted disk\n");
}

static struct file_system_type ouichefs_file_system_type = {
	.owner = THIS_MODULE,
	.name = "ouichefs",
	.mount = ouichefs_mount,
	.kill_sb = ouichefs_kill_sb,
	.fs_flags = FS_REQUIRES_DEV,
	.next = NULL,
};

static long unlocked_ioctl(struct file *f,
				unsigned int cmd,
				unsigned long arg)
{

	if (_IOC_TYPE(cmd) != IOC_MAGIC)
		return -EINVAL;
	switch (cmd) {
	case QUICK_CLEAN:
		ouichefs_fblocks(root_inode);
		break;
	default:
		return -ENOTTY;
	}
	return -EINVAL;
}


static char *ouichefs_dev_devnode(struct device *dev, umode_t *mode)
{
	if (!mode)
		return NULL;
	if (MAJOR(dev->devt) == major)
		*mode = 0666;
	return NULL;
}
/*
 * Sturcture for standard function :
 */
struct file_operations fops = {

	.unlocked_ioctl = unlocked_ioctl
};

static int __init ouichefs_init(void)
{
	int ret;
	struct device *pDev;

	ret = ouichefs_init_inode_cache();
	if (ret) {
		pr_err("inode cache creation failed\n");
		goto end;
	}

	ret = register_filesystem(&ouichefs_file_system_type);
	if (ret) {
		pr_err("register_filesystem() failed\n");
		goto end;
	}

	major = register_chrdev(0, "ouichefs", &fops);
	if (major < 0) {
		pr_warn("Register device failcd: %d\n", major);
		return major;
	}
	pr_info("Registered !\n");


	pClass = class_create(THIS_MODULE, "ouichefs");
	if (IS_ERR(pClass)) {
		pr_warn("Can't create class\n");
		goto error1;
	}
	pr_info("Class created !\n");

	pClass->devnode = ouichefs_dev_devnode;

	devNo = MKDEV(major, 0);
	pDev = device_create(pClass, NULL, devNo, NULL, "ouichefs");
	if (IS_ERR(pDev)) {
		pr_warn("hello can't create device /dev/ouichefs\n");
		goto error2;
	}
	pr_info("Device created\n");
	pr_info("module loaded\n");
end:
	return ret;

error2:	class_destroy(pClass);
error1:	unregister_chrdev_region(devNo, 1);
	return -1;
}

static void __exit ouichefs_exit(void)
{
	int ret;

	device_destroy(pClass, devNo);
	pr_warn("Device removed\n");

	class_destroy(pClass);
	pr_warn("Class removed\n");

	unregister_chrdev(major, "ouichefs");
	pr_warn("Unregisted Device\n");

	ret = unregister_filesystem(&ouichefs_file_system_type);
	if (ret)
		pr_err("unregister_filesystem() failed\n");

	ouichefs_destroy_inode_cache();

	pr_info("module unloaded\n");
}

module_init(ouichefs_init);
module_exit(ouichefs_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Redha Gouicem, <redha.gouicem@lip6.fr>");
MODULE_DESCRIPTION("ouichefs, a simple educational filesystem for Linux");
