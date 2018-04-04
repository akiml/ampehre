/*
 * driver_measure.c
 * 
 * Copyright (C) 2015, Achim Lösch <achim.loesch@upb.de>, Christoph Knorr <cknorr@mail.uni-paderborn.de>
 * All rights reserved.
 * 
 * This software may be modified and distributed under the terms
 * of the BSD license or GPL license. See the LICENSE_BSD and
 * LICENSE_GPL files for details.
 * 
 * encoding: UTF-8
 * tab size: 4
 * 
 * author: Achim Lösch (achim.loesch@upb.de)
 * author: Christoph Knorr (cknorr@mail.upb.de)
 * created: 8/27/14
 * version: 0.1.6 - add an measuring system driver for cpu stat readings
 *          0.2.0 - add support for msr registers to the measure driver
 *          0.2.1 - add support for IPMI to the measure driver
 *          0.3.3 - add cpu memory info to measure driver
 *          0.5.7 - add automatic detection of ipmi device to measure driver
 *          0.6.0 - add ioctl for the ipmi timeout, new parameters to skip certain measurements 
 *                  and to select between the full or light library.
 */

#include "driver_measure.h"

#define BUILD_DRIVER
#include "../include/ms_version.h"

MODULE_AUTHOR("Achim Loesch (achim.loesch@upb.de) and Christoph Knorr (cknorr@mail.upb.de)");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Driver used by the Ampehre measuring framework library libmeasure.");
MODULE_SUPPORTED_DEVICE("none");
MODULE_VERSION(MS_VERSION_STRING);

static DEFINE_MUTEX(ipmi_mutex);
static DEFINE_MUTEX(ioctl_mutex);

//static struct file* proc_meminfo_file = NULL;

static IPMI_DATA *ipmi_data = NULL;

//count how many times the driver is opened
static int opened = 0;

static unsigned long ipmi_timeout = IPMI_MAX_TIMEOUT;
static int ipmi_timeout_locked = 0;

static int ipmi_supported = 0;

static struct ipmi_user_hndl ipmi_hndlrs =
{
	.ipmi_recv_hndl	= ipmi_msg_handler, 
};

/*
 * DRIVER ACCESS
 */
static struct file_operations fileops = {
	.owner		= THIS_MODULE,
	.llseek		= driver_lseek,
	.read		= driver_read,
	.write		= driver_write,
	.open		= driver_open,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
	.ioctl		= driver_ioctl,
#else
	.unlocked_ioctl = driver_ioctl,
#endif
	.release	= driver_close
};

static loff_t driver_lseek(struct file *fileptr, loff_t offset, int whence) {
	loff_t position;
	
	struct inode *inode = fileptr->f_mapping->host;
	mutex_lock(&inode->i_mutex);
	switch(whence) {
		case 2: /* SEEK_END: IPMI */
			position = IPMI | offset;
			break;
		default:
			mutex_unlock(&inode->i_mutex);
			return -EINVAL;
	}
	 mutex_unlock(&inode->i_mutex);
	
	if (position >= 0) {
		fileptr->f_pos = position;
		return position;
	}
	
	return -EINVAL;
}

/*Check the offset for ipmi and return 0 if it's correct*/
static int check_ipmi_offset(loff_t offset){
	loff_t whence_ipmi = (WHENCE_MASK & offset) >> WHENCE_POS;
	loff_t cmd = 0;
	
	if(whence_ipmi != ipmi)
		return -EINVAL;
	cmd = (CPU_MASK & offset) >> CPU_NO_POS;
	if(cmd < 0 || cmd > 4)
		return -EINVAL;
	
	return 0;
}

/*Send an IPMI request the response is in data->datarc*/
static int ipmi_request(int netfn, int cmd, unsigned char * msgdata, int size){
	struct kernel_ipmi_msg msg;
	int rv;
	mutex_lock(&ipmi_mutex);
	ipmi_data ->datarc_length = 0;
	
	init_completion(&ipmi_data->read_complete);

	msg.netfn = netfn;
	msg.cmd = cmd;
	msg.data_len = size;
	msg.data = msgdata;
	
	rv = ipmi_validate_addr(&ipmi_data->address, sizeof(ipmi_data->address));
	if(rv){
		printk("Measure: Bad IPMI address!\n");
		mutex_unlock(&ipmi_mutex);
		return -rv;
	}
	rv = ipmi_request_settime(ipmi_data->user, &(ipmi_data->address), ipmi_data->msgid, &msg, ipmi_data, 0, -1, 0);
	if(rv){
		printk("Measure: Error in IPMI request!\n");
		mutex_unlock(&ipmi_mutex);
		return -rv;
	}
	//The timeout value is in jiffies
	rv = wait_for_completion_timeout(&ipmi_data->read_complete, msecs_to_jiffies(ipmi_timeout));
	if(rv == 0){
		printk("Measure: IPMI request time out.\n");
		ipmi_data->msgid++;
		mutex_unlock(&ipmi_mutex);
		return -ETIMEDOUT;
	}
	ipmi_data->msgid++;
	mutex_unlock(&ipmi_mutex);	
	return 0;
	
}

/*Read data dependent on the previous lseek */
static ssize_t driver_read(struct file *fileptr, char __user *user_buffer, size_t user_buffer_size, loff_t *offset) {
	int err = 0;
	unsigned char ipmi_msg_data[IPMI_MAX_MSG_LENGTH];
	int msg_data_size;
	int ipmi_mode = 0;
	int ipmi_read_id = 0;
	int ipmi_netfn;
	int ipmi_cmd;
	if(!check_ipmi_offset(*offset) && ipmi_supported){
		ipmi_mode = (int)((CPU_MASK & *offset) >> CPU_NO_POS);
		ipmi_read_id = *offset & RECORD_ID_MASK;
		if(ipmi_mode == IPMI_GET_SDR){
			ipmi_netfn = STORAGE_NETFN;
			ipmi_cmd = CMD_GET_SDR;
			ipmi_msg_data[0] = 0x00;
			ipmi_msg_data[1] = 0x00;
			ipmi_msg_data[2] = ipmi_read_id & 0xff;
			ipmi_msg_data[3] = (ipmi_read_id & 0xff00) >> 8;
			ipmi_msg_data[4] = 0x00;
			ipmi_msg_data[5] = READ_FULL_RECORD;
			msg_data_size = 6;
		} else if(ipmi_mode == (int)(IPMI_GET_SENSOR_READING >> CPU_NO_POS)){
			ipmi_netfn = SENSOR_NETFN;
			ipmi_cmd = CMD_GET_SENSOR_READING;
			ipmi_msg_data[0] = ipmi_read_id & 0xff;
			msg_data_size = 1;
		} else if(ipmi_mode == (int)(IPMI_DELL_RESET_ENERGY >> CPU_NO_POS)){
			ipmi_netfn = IPMI_NET_FN_OEM_DELL_GENERIC_RQ;
			ipmi_cmd = IPMI_CMD_OEM_DELL_RESET_POWER_CONSUMPTION_DATA;
			ipmi_msg_data[0] = IPMI_ENTITY_ID_SYSTEM_BOARD;
			ipmi_msg_data[1] = 0x01;
			ipmi_msg_data[2] = IPMI_OEM_DELL_RESET_POWER_CONSUMPTION_DATA_CUMULATIVE;
			msg_data_size = 3;
		}else if(ipmi_mode == (int)(IPMI_DELL_CUMULATIVE_ENERGY >> CPU_NO_POS)){
			ipmi_netfn = IPMI_NET_FN_OEM_DELL_GENERIC_RQ;
			ipmi_cmd = IPMI_CMD_OEM_DELL_GET_POWER_CONSUMPTION_DATA;
			ipmi_msg_data[0] = IPMI_ENTITY_ID_SYSTEM_BOARD;
			ipmi_msg_data[1] = 0x01;
			msg_data_size = 2;
		}else if(ipmi_mode == (int)(IPMI_DELL_CURRENT_POWER >> CPU_NO_POS)){
			ipmi_netfn = IPMI_NET_FN_OEM_DELL_GENERIC_RQ;
			ipmi_cmd = IPMI_CMD_OEM_DELL_POWER_CONSUMPTION;
			ipmi_msg_data[0] = IPMI_ENTITY_ID_POWER_SUPPLY;
			ipmi_msg_data[1] = IPMI_OEM_DELL_POWER_CONSUMPTION_ENTITY_INSTANCE_ALL;
			msg_data_size = 2;
		} else {
			return -EFAULT;
		}
		err = ipmi_request(ipmi_netfn, ipmi_cmd, ipmi_msg_data, msg_data_size);
		if(err){
			printk("Measure: Error in IPMI Request: %d\n", err);
			return err;
		}
		err = ipmi_data->datarc_length;
				
		if (copy_to_user(user_buffer, ipmi_data->datarc, err) != 0) {
			printk("Measure: Cannot copy all data.\n");
			return -EFAULT;
		}
	}
	else{
		printk("Measure: Cannot read! Check the offset provided with lseek!\n");
		return -EINVAL;
	}
	
	return err;
}

/*Write to msr registers*/
static ssize_t driver_write(struct file *fileptr, const char __user *user_buffer, size_t user_buffer_size, loff_t *offset){
	printk("Measure: Cannot write! Check the offset provided with lseek!\n");
	return -EINVAL;
}

/*All incoming ipmi messages get passed to this handler*/
static void ipmi_msg_handler(struct ipmi_recv_msg *msg, void *user_msg_data){
	IPMI_DATA *user_data = user_msg_data;

	if(msg->msgid != user_data->msgid){
		printk("Measure: Missmatch between received msgid %li and transmitted msgid %li!\n", msg->msgid, user_data->msgid);
		user_data->datarc_length = 0;
	} else {
		user_data->datarc_length = 1;
		if (msg->msg.data_len > 0){
			user_data->datarc[0] = msg->msg.data[0];
		}else{
			user_data->datarc[0] = IPMI_UNKNOWN_ERR_COMPLETION_CODE;
		}
		if(msg->msg.data_len > 1 && msg->msg.data_len < IPMI_MAX_MSG_LENGTH){
			user_data->datarc_length = msg -> msg.data_len;
			memcpy(user_data->datarc+1, msg->msg.data + 1, user_data->datarc_length-1);
		}
		complete(&user_data->read_complete);
	}
	ipmi_free_recv_msg(msg);	
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int driver_ioctl(struct inode *inode, struct file *file, unsigned int request, unsigned long arg)
#else
static long driver_ioctl(struct file *f, unsigned int request, unsigned long arg)
#endif
{	
	unsigned long new_timeout;
	if(!ipmi_supported){
		printk("Measure: ERROR no ioctl calls are supported!\n");
		return -EINVAL;
	}
	
	switch(request){
		case IOC_GET_IPMI_TIMEOUT:
			if (copy_to_user((unsigned long*)arg, &ipmi_timeout, sizeof(unsigned long))){
				return -EACCES;
			}
			break;
		case IOC_SET_IPMI_TIMEOUT:
			if(ipmi_timeout_locked){
				return ERROR_IPMI_TIMEOUT_LOCKED;
			}
			mutex_lock(&ioctl_mutex);
			if (copy_from_user(&new_timeout, (unsigned long *)arg, sizeof(unsigned long))) {
				mutex_unlock(&ioctl_mutex);
				return -EACCES;
			}
			if(new_timeout > IPMI_MAX_TIMEOUT || new_timeout == 0){
				ipmi_timeout = IPMI_MAX_TIMEOUT;
				return ERROR_IPMI_TIMEOUT_MAX;
				mutex_unlock(&ioctl_mutex);
			} else {
				ipmi_timeout = new_timeout;
			}
			break;
		case IOC_SET_AND_LOCK_IPMI_TIMEOUT:
			mutex_lock(&ioctl_mutex);
			ipmi_timeout_locked = 1;
			if (copy_from_user(&new_timeout, (unsigned long *)arg, sizeof(unsigned long))) {
				mutex_unlock(&ioctl_mutex);
				return -EACCES;
			}
			if(new_timeout > IPMI_MAX_TIMEOUT || new_timeout == 0){
				ipmi_timeout = IPMI_MAX_TIMEOUT;
				mutex_unlock(&ioctl_mutex);
				return ERROR_IPMI_TIMEOUT_MAX;
			} else {
				ipmi_timeout = new_timeout;
			}
			break;
		default:
			printk("Measure: Error not supported ioctl request code!\n");
			return -EINVAL;
	}
	mutex_unlock(&ioctl_mutex);
	
	return 0;
}


static int driver_open(struct inode *device, struct file *fileptr) {
	int if_num = iminor(device);
	int rv;
	
	mutex_lock(&ipmi_mutex);
	if(opened !=0){
		printk("Measure: Device is already opened!\n");
		opened++;
		mutex_unlock(&ipmi_mutex);
		return 0;
	}
	if(ipmi_supported){
		ipmi_data = kmalloc(sizeof(*ipmi_data), GFP_KERNEL);
		if(!ipmi_data){
			mutex_unlock(&ipmi_mutex);
			return -ENOMEM;
		}
		ipmi_data->msgid = 1;
		rv = ipmi_create_user(if_num, &ipmi_hndlrs, ipmi_data , &(ipmi_data->user));
		if (rv) {
			kfree(ipmi_data);
			mutex_unlock(&ipmi_mutex);
			return rv;
		}
	}
	opened++;
	mutex_unlock(&ipmi_mutex);
	
	if(ipmi_supported){
		ipmi_data->address.addr_type = IPMI_SYSTEM_INTERFACE_ADDR_TYPE;
		ipmi_data->address.channel = IPMI_BMC_CHANNEL;
		ipmi_data->address.data[0] = 0;
		if(!ipmi_timeout_locked){
 			ipmi_timeout = IPMI_MAX_TIMEOUT;
		}
	}
	
	
	printk("Measure: Opened driver.\n");
	return 0;
}

static int driver_close(struct inode *device, struct file *fileptr) {
	int rv;
	if(opened <= 1){
		if(ipmi_supported){
			rv = ipmi_destroy_user(ipmi_data->user);
			if (rv)
				return rv;
			kfree(ipmi_data);
			ipmi_data = NULL;
			ipmi_timeout_locked = 0;
		}		
	}
	printk("Measure: Closed driver.\n");
	opened--;
	return 0;
}

/*
 * MODULE/DEVICE MANAGEMENT
 */
static dev_t device_number;
static struct cdev *driver_object;
static struct class *device_class;
static struct device *device;

static int __init mod_init(void) {
	const struct dmi_device *dev = NULL;
	
	if (alloc_chrdev_region(&device_number, 0, 1, "measure") < 0) {
		printk("Measure: Cannot load module.\n");
		return -EIO;
	}
	
	driver_object = cdev_alloc();
	if (NULL == driver_object) {
		printk("Measure: Cannot load module.\n");
		unregister_chrdev_region(device_number, 1);
		return -EIO;
	}
	
	driver_object->owner	= THIS_MODULE;
	driver_object->ops		= &fileops;
	
	if (cdev_add(driver_object, device_number, 1)) {
		printk("Measure: Cannot load module.\n");
		kobject_put(&driver_object->kobj);
		unregister_chrdev_region(device_number, 1);
		return -EIO;
	}
	
	device_class = class_create(THIS_MODULE, "measure");
	if (IS_ERR(device_class)) {
		printk("Measure: No udev support\n");
		kobject_put(&driver_object->kobj);
		unregister_chrdev_region(device_number, 1);
		return -EIO;
	}
	
	if(dmi_find_device(DMI_DEV_TYPE_IPMI, NULL, dev)){
		printk("Measure: Detected IPMI device.\n");
		ipmi_supported=1;
	}
	
	device = device_create(device_class, NULL, device_number, NULL, "%s", "measure");
	printk("Measure: Module loaded successfully.\n");
	
	return 0;
}

static void __exit mod_exit(void) {
	device_destroy(device_class, device_number);
	class_destroy(device_class);
	cdev_del(driver_object);
	unregister_chrdev_region(device_number, 1);
	
	printk("Measure: Module unloaded successfully.\n");
}

module_init( mod_init );
module_exit( mod_exit );
