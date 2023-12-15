#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/pwm.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EMB_SYS_6/Linux");
MODULE_DESCRIPTION("RPI PWM Driver");

static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

struct pwm_device *pwm0 = NULL;
u32 pwm_on_time = 500000000;

#define DRIVER_NAME "pwm_driver"
#define DRIVER_CLASS "PWMModuleClass"

static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
    int to_copy, not_copied, delta;
    char value;

    to_copy = min(count, sizeof(value));

    not_copied = copy_from_user(&value, user_buffer, to_copy);

    delta = to_copy - not_copied;

    printk("Test Driver Write Function\n");
    printk("%c == %d\n", value, 100000000 * (value - '0'));

    if (value < '1' || value > ':')
        printk("Invalid Value\n");
    else
        pwm_config(pwm0, 100000000 * (value - '0'), 100000000);
    
    return delta;
}

static int driver_open(struct inode *device_file, struct file *instance) {
    printk("dev_nr - open was called\n");
    return 0;
}

static int driver_close(struct inode *device_file, struct file *instance) {
    printk("dev_nr - close was called\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.write = driver_write
};

static int __init ModuleInit(void) {
    printk("Hello, PWM Device Driver!\n");

    if(alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
        printk("Device Nr. could not be allocated!\n");
        return -1;
    }

    printk("read_write - Device Nr. Major: %d, Minor : %d was registered!\n", my_device_nr>>20, my_device_nr&0xfffff);

    if((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		printk("Device class can not created!\n");
		goto ClassError;
	}

    if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
		printk("Can not create device file!\n");
		goto FileError;
	}

	cdev_init(&my_device, &fops);

	if(cdev_add(&my_device, my_device_nr, 1) == -1) {
		printk("Registering of device to kernel failed!\n");
		goto AddError;
	}

    pwm0 = pwm_request(0, "rpi-pwm0");

	if(pwm0 == NULL) {
		printk("Could not get PWM0!\n");
		goto AddError;
	}

	pwm_config(pwm0, pwm_on_time, 1000000000);
	pwm_enable(pwm0);

	return 0;

AddError:
	device_destroy(my_class, my_device_nr);
FileError:
	class_destroy(my_class);
ClassError:
	unregister_chrdev(my_device_nr, DRIVER_NAME);
	return -1;
}

static void __exit ModuleExit(void) {
    pwm_disable(pwm0);
	pwm_free(pwm0);

	cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev(my_device_nr, DRIVER_NAME);	
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);  
module_exit(ModuleExit);