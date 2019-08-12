#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/wait.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/types.h>
#include <linux/sched.h>
#define GLOBALMEM_SIZE  0x1000
#define MEM_CLEAR  0x1
#define GLOBALMEM_MAJOR  230
static int globalmem_major = GLOBALMEM_MAJOR;
module_param(globalmem_major, int, S_IRUGO);
struct globalmem_dev {
	struct cdev cdev;
	unsigned int current_len;
	unsigned char mem[GLOBALMEM_SIZE];
	struct mutex mutex;
	wait_queue_head_t r_wait;
	wait_queue_head_t w_wait;
};
struct globalmem_dev *globalmem_devp;
