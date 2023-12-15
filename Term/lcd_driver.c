#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/delay.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("EMB_SYS_6/Linux");
MODULE_DESCRIPTION("LCD Pannel Driver");

/* Variables for device and device class */
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

#define DRIVER_NAME "lcd_driver"
#define DRIVER_CLASS "LCDModuleClass"

/* LCD char buffer */
static char lcd_buffer[17];

/* Pinout for LCD Display */
unsigned int gpios[] = {
    3, /* Enable */
    2, /* Register Select */
    4, /* Data 0 */
    17, /* Data 1 */
    27, /* Data 2 */
    22, /* Data 3 */
    10, /* Data 4 */
    9, /* Data 5 */
    11, /* Data 6 */
    5 /* Data 7 */
};

/**
 * @brief Generate enable signal
 */
void lcd_enable(void) {
	gpio_set_value(gpios[0], 1);
	msleep(5);
	gpio_set_value(gpios[0], 0);
}


/**
 * @brief Set the 8 bit data
 */
void lcd_send_byte(char data){
	int i;
	for(i = 0; i < 8; i++) {
		// +2 is Enable pin, Select Pin
		gpio_set_value(gpios[i+2], ((data) & (1<<i)) >> i);
	}

	lcd_enable();
	msleep(5);
}

/**
 * @brief Send a data or command to the LCD
 */
void lcd_send(uint8_t data, int command) {
	if (command == 1) {
		gpio_set_value(gpios[1], 0); // RS to Instruction
	}
    else {
		gpio_set_value(gpios[1], 1); // RS to Data
	}
	lcd_send_byte(data);
}

/**
 * @brief Write data to buffer
 */
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
	int to_copy, not_copied, delta, i;

	/* Get amount of data to copy */
	to_copy = min(count, sizeof(lcd_buffer));

	/* Copy data to user */
	not_copied = copy_from_user(&lcd_buffer, user_buffer, to_copy);

    /* Calculate data */
	delta = to_copy - not_copied;

	/* Clear the LCD Display */
	if (strcmp(user_buffer, "CLEAR_LCD") == 0) {
        lcd_send(0x01, 1); // Clear the LCD Display
        return strlen("CLEAR_LCD");
    }

	/* Set the new data to the display */	
	lcd_send(0x01, 1); // Clear the LCD Display
    for (i = 0; i < to_copy; i++) {
        lcd_send(lcd_buffer[i], 0); // send characters to the LCD Display
    }

    return delta;
}


/**
 * @brief This function is called, when the device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance) {
	printk("lcd_display - open was called!\n");
	return 0;
}

/**
 * @brief This function is called, when the device file is opened
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("lcd_display - close was called!\n");
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close,
	.write = driver_write
};

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init ModuleInit(void) {
	int i;
    char *names[] = {"ENABLE_PIN", "REGISTER_SELECT", 
		"DATA_PIN0", "DATA_PIN1", "DATA_PIN2", "DATA_PIN3", "DATA_PIN4", "DATA_PIN5", "DATA_PIN6", "DATA_PIN7"};

    printk("Hello, LCD Device Driver!\n");

    /* Allocate a device nr */
    if(alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
		printk("Device Nr. could not be allocated!\n");
		return -1;
	}

	printk("read_write - Device Nr. Major: %d, Minor : %d was registered!\n", my_device_nr>>20, my_device_nr&0xfffff);

    /* Create device class */
	if((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
		printk("Device class can not e created!\n");
		goto ClassError;
	}

    /* create device file */
	if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
		printk("Can not create device file!\n");
		goto FileError;
	}

    /* Initialize device file */
	cdev_init(&my_device, &fops);

	/* Regisering device to kernel */
	if(cdev_add(&my_device, my_device_nr, 1) == -1) {
		printk("Registering of device to kernel failed!\n");
		goto AddError;
	}

    /* Initialize GPIOs */
	for(i = 0; i < 10; i++){
		if(gpio_request(gpios[i], names[i])) {
			printk("lcd-driver - Error Init GPIO %d\n", gpios[i]);
			goto GpioInitError;
		}
	}

    /* Set GPIOs Directions */
    for(i = 0; i < 10; i++){
		if(gpio_direction_output(gpios[i], 0)) {
			printk("lcd-driver - Error setting GPIO %d to output\n", i);
			goto GpioDirectionError;
		}
	}

    /* Init the Display */
    lcd_send(0x30, 1); // Set the display for 8 bit data interface 
	lcd_send(0x0f, 1);  // Turn display on, turn cursor on, set cursor blinking
	lcd_send(0x01, 1);  // Clear Display

    return 0;

GpioDirectionError:
	i = 9;

GpioInitError:
	for(; i >= 0; i--)
		gpio_free(gpios[i]);

AddError:
	device_destroy(my_class, my_device_nr);

FileError:
	class_destroy(my_class);

ClassError:
	unregister_chrdev(my_device_nr, DRIVER_NAME);
	return -1;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit ModuleExit(void) {
	int i;
    lcd_send(0x01, 1);

	for(i = 0; i < 10; i++) {
		gpio_set_value(gpios[i], 0);
		gpio_free(gpios[i]);
	}

    cdev_del(&my_device);
	device_destroy(my_class, my_device_nr);
	class_destroy(my_class);
	unregister_chrdev(my_device_nr, DRIVER_NAME);
    
	printk("Goodbye, Kernel\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);