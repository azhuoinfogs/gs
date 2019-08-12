#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <linux/seq_file.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/cdev.h>
#define DEMO_DEV_MAX 255
#define DEMO_DEVICE_NAME_SIZE 32
struct demo_data
{
	unsigned long text_data;
	/* something else */
};

struct demo_device
{
	struct device dev;
	struct module *owner;
	int id;
	char name[DEMO_DEVICE_NAME_SIZE];
	const struct demo_class_ops *ops;
	struct mutex ops_lock;
	struct cdev char_dev;
	unsigned long flags;
	unsigned long irq_data;
	spinlock_t irq_lock;
	wait_queue_head_t irq_queue;
	struct fasync_struct *async_queue;
	/* some demo data */
	struct demo_data demo_data;
};

struct demo_class_ops {
	int (*open)(struct device *);
	void (*release)(struct device *);
	int (*ioctl)(struct device *, unsigned int, unsigned long);
	//int (*set_data)(struct device *, struct demo_ctl_data *);
	//int (*get_data)(struct device *, struct demo_ctl_data *);
	int (*proc)(struct device *, struct seq_file *);
	int (*read_callback)(struct device *, int data);
};
