#include <linux/module.h>  // MODULE_DESCRIPTION, MODULE_LICENSE, module_init, module_exit
#include <linux/vt_kern.h>  // fg_console
#include <linux/timer.h>  // timer_setup, add_timer, del_timer

MODULE_DESCRIPTION("Keyboard leds control");
MODULE_LICENSE("GPL");

#define BLINK_DELAY HZ  // Задержка между вызовами таймера. HZ - количество прерываний системного таймера в секунду
#define RESTORE_LEDS 0x80  // Если установлен старший бит, индикаторы возвращаются в исходное состояние

static struct timer_list my_timer;
static struct tty_driver *my_driver;
static int led_state = 7;  // Состояние индикаторов для переключения с исходного
static int status = 0;  // Текущее состояние
static struct kobject *my_kobject;

static ssize_t print_value(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%d\n", led_state);
}

static ssize_t store_value(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    sscanf(buf, "%du", &led_state);
    return count;
}

static struct kobj_attribute attr = __ATTR(led_state, 0660, print_value, store_value);

static void my_timer_func(struct timer_list *ptr)
{
    // Новое состояние индикаторов
    if (status == led_state) status = RESTORE_LEDS;
    else status = led_state;

    // Переключение состояния
    (my_driver->ops->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED, status);

    // Продолжение таймера, jiffies - кол-во прерываний системного таймера с момента загрузки системы
    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);
}

static int __init ledsctl_init(void)
{
    // led driver
    int i, error;
    printk(KERN_INFO "ledsctl_init\n");
    printk(KERN_INFO "fgconsole: %x\n", fg_console);
    for (i = 0; i < MAX_NR_CONSOLES; i++)
    {
        if (!vc_cons[i].d) break;
        printk(KERN_INFO "poet_atkm: console[%i/%i] #%i, tty %lx\n",
            i,
            MAX_NR_CONSOLES,
            vc_cons[i].d->vc_num,
            (unsigned long)vc_cons[i].d->port.tty
        );
    }
    my_driver = vc_cons[fg_console].d->port.tty->driver;
    printk(KERN_INFO "tty driver magic %x\n", my_driver->magic);
    timer_setup(&my_timer, my_timer_func, 0);
    my_timer.expires = jiffies + BLINK_DELAY;
    add_timer(&my_timer);

    // sysfs
    my_kobject = kobject_create_and_add("ledsctl", kernel_kobj);
    if (!my_kobject) return 0;
    error = sysfs_create_file(my_kobject, &attr.attr);
    if (error) pr_debug("failed to create file\n");
    return error;
}

static void __exit ledsctl_exit(void)
{
    printk(KERN_INFO "ledsctl_exit\n");
    del_timer(&my_timer);  // Ubuntu виснет намертво, если не поставить
    (my_driver->ops->ioctl) (vc_cons[fg_console].d->port.tty, KDSETLED, RESTORE_LEDS);
    kobject_put(my_kobject);
}

module_init(ledsctl_init);
module_exit(ledsctl_exit);
