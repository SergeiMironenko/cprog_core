#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("desc");

#define DEVICE_NAME "chdev"
#define BUF_LEN 80

static int counter = 0;
static int major;
enum
{
    CDEV_NOT_USED = 0,
    CDEV_EXCLUSIVE_OPEN = 1,
};

static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);
static char msg[BUF_LEN];
static struct class *cls;

static ssize_t device_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset)
{
    int bytes_read = 0;
    const char *msg_ptr = msg;

    // Если достигли конца
    if (!*(msg_ptr + *offset))
    {
        *offset = 0;
        return 0;
    }
    msg_ptr += *offset;

    // Перемещение данных в буфер
    while (length && *msg_ptr)
    {
        /*
        * Буфер находится в пространстве пользователя (в сегменте данных), 
        * а не в пространстве ядра, поэтому простое присваивание здесь недопустимо. 
        * Для того, чтобы скопировать данные, мы используем функцию put_user, 
        * которая перенесет данные из пространства ядра в пространство пользователя. 
        */
        put_user(*(msg_ptr++), buffer++);
        length--;
        bytes_read++;
    }
    *offset += bytes_read;

    // Вернуть количество байт, записанных в буфер.
    return bytes_read;
}

static ssize_t device_write(struct file *filp, const char __user *buff, size_t len, loff_t *off)
{
    pr_alert("Sorry, this operation is not supported.\n");
    return -EINVAL;
}

static int device_open(struct inode *inode, struct file *file)
{
    printk("device_open\n");

    // atomic_cmpxchg
    // Первый аргумент имеет значение old, второй cmp, третий val
    // (old == cmp) ? val : old - результат операции поместится в первый аргумент
    // функция вернет old
    // В данном случае already_open просто переключает значения
    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN))
        return -EBUSY;  // -16
    sprintf(msg, "I already told you %d times Hello world!\n", counter++);
    // try_module_get(THIS_MODULE);
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    atomic_set(&already_open, CDEV_NOT_USED);
    // module_put(THIS_MODULE);
    return 0;
}

static struct file_operations chardev_fops =
{
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
};

static int __init chardev_init(void)
{
    printk("chardev_init\n");
    major = register_chrdev(0, DEVICE_NAME, &chardev_fops);
    if (major < 0)
    {
        pr_alert("Registering char device failed with %d\n", major);
        return major;
    }
    pr_info("major number: %d\n", major);
    cls = class_create(THIS_MODULE, DEVICE_NAME);
    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    pr_info("device created on /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit chardev_exit(void)
{
    printk("chardev_exit\n");
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
    unregister_chrdev(major, DEVICE_NAME);
}

module_init(chardev_init);
module_exit(chardev_exit);
