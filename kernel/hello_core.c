#include "hello.h"
#include "hello_sysfs.h"
#ifndef SLEEP_MILLI_SEC
#define SLEEP_MILLI_SEC(nMilliSec)                                             \
    do {                                                                       \
        long timeout = (nMilliSec) * HZ / 1000;                                \
        while (timeout > 0) {                                                  \
            timeout = schedule_timeout(timeout);                               \
        }                                                                      \
    } while (0);
#endif

#define MODULE_VERS "1.0"
#define MODULE_NAME "procfs_example"
#define FOOBAR_LEN 8

static struct task_struct *test_task = NULL;
struct class *demo_class;
dev_t demo_devt;
static int hello_proc_show(struct seq_file *m, void *v) {
    seq_printf(m, "Hello proc!\n");
    return 0;
}
static int hello_proc_open(struct inode *inode, struct file *file) {
    return single_open(file, hello_proc_show, NULL);
}

static const struct file_operations hello_proc_fops = { .owner = THIS_MODULE,
                                                        .open = hello_proc_open,
                                                        .read = seq_read,
                                                        .llseek = seq_lseek,
                                                        .release = single_release, };

void __init demo_dev_init(void) {
    int err;
    err = alloc_chrdev_region(&demo_devt, 0, DEMO_DEV_MAX, "demo");
    if (err < 0)
        pr_err("failed to allocate char dev region\n");
}
void __exit demo_dev_exit(void) {
    unregister_chrdev_region(demo_devt, DEMO_DEV_MAX);
}

static int __init demo_proc_init(void) {
    proc_create("demo_proc", 0, NULL, &hello_proc_fops);
    return 0;
}
static void __exit demo_proc_exit(void) {
    remove_proc_entry("hello_proc", NULL);
}
/*
void __init demo_sysfs_init(struct class *demo_class)
{
        demo_class->dev_groups = demo_groups;
}
*/

static int MyPrintk(void *data) {
    char *mydata = kmalloc(strlen(data) + 1, GFP_KERNEL);
    memset(mydata, '\0', strlen(data) + 1);
    strncpy(mydata, data, strlen(data));
    while (!kthread_should_stop()) {
        SLEEP_MILLI_SEC(1000);
        printk("%s\n", mydata);
    }
    kfree(mydata);
    return 0;
}
static int __init init_kthread(void) {
    test_task = kthread_run(MyPrintk, "hello world", "mythread");
    return 0;
}
static void __exit exit_kthread(void) {
    if (test_task) {
        printk("stop MyThread\n");
        kthread_stop(test_task);
    }
}
/*
//设置初始化入口函数
static int __init hello_sysfs_init(void) {
    int ret = 0;
    kst = kset_create_and_add("test_kset", NULL, kernel_kobj->parent);
    if (!kst) {
        printk(KERN_ALERT "Create kset failed\n");
        kset_put(kst);
    }
    kob = kzalloc(sizeof(*kob), GFP_KERNEL);
    if (IS_ERR(kob)) {
        printk(KERN_ALERT "alloc failed!!\n");
        return -ENOMEM;
    }
    ret = kobject_init_and_add(kob, NULL, NULL, "%s", "test_obj");
    if (ret) {
        kobject_put(kob);
        kset_unregister(kst);
    }
    printk(KERN_DEBUG "kobj test project!!!\n");
    return 0;
}

//设置出口函数
static void __exit hello_sysfs_exit(void)
{
        kobject_put(kob);
        kset_unregister(kst);
        printk(KERN_DEBUG "goodbye !!!\n");
}

static int init_module_hello(void) {
        printk("Hello World!\n");
        init_kthread();
//	hello_proc_init();
        sysfs_demo_init(demo_class);
        return 0;
}

static void cleanup_module_hello(void) {
        exit_kthread();
//	hello_proc_exit();
        sysfs_demo_exit();
        printk("Goodbye!\n");

}
*/

static int __init demo_core_init(void) {
    /* 创建 demo class */
    demo_class = class_create(THIS_MODULE, "demo");
    if (IS_ERR(demo_class)) {
        pr_err("couldn't create class\n");
        return PTR_ERR(demo_class);
    }
    /* demo 设备驱动初始化 */
    demo_dev_init();
    /* demo proc初始化 */
    demo_proc_init();
    /* demo sysfs初始化 */
	init_kthread();
//    sysfs_demo_init(demo_class);
    pr_info("demo subsys init success\n");
    return 0;
}

static void __exit demo_core_exit(void) {
    demo_proc_exit();
    demo_dev_exit();
    class_destroy(demo_class);
	exit_kthread();
    pr_info("demo subsys exit success\n");
}

module_init(demo_core_init);
module_exit(demo_core_exit);

MODULE_LICENSE("Dual BSD/GPL");
