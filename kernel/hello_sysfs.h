#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/device.h>
extern int __init sysfs_demo_init(struct class *cla);
extern void __exit sysfs_demo_exit(void);
