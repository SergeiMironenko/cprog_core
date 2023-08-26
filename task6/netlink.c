#include <linux/module.h>
#include <linux/netlink.h>
#include <net/net_namespace.h>
#include <linux/skbuff.h>
#include <net/sock.h>

MODULE_DESCRIPTION("netlink");
MODULE_LICENSE("GPL");

/**
 * UNIT - номер протокола netlink
 * Доступны от 0 до 31, некоторые используются
 * https://elixir.bootlin.com/linux/v5.15.13/source/include/uapi/linux/netlink.h
 */
#define UNIT 31

static struct sock *nl_sk = NULL;


static void hello_nl_recv_msg(struct sk_buff *skb) {

    struct nlmsghdr *nmh;  // Заголовок в сообщениях netlink
    struct sk_buff *skb_out;  // Буфер сокета для отправки в User Space
    char *msg = "Hello from Kernel Space";
    int msg_size,
        pid,
        res;

    pr_info("Entering %s\n", __FUNCTION__);

    /*
     * Структура sk_buff определяет набор полей данных,
     * которые используются ядром Linux при обработке сетевых пакетов.
     * Принятые сетевыми интерфейсами пакеты помещаются в буферы sk_buff,
     * которые передаются сетевому стеку, использующему буфер в течение всего процесса обработки пакета.
     * Указатель на начало данных в буфере.
     */
    nmh = (struct nlmsghdr *)skb->data;

    // nlmsg_data Возвращает указатель на "полезную" часть сообщения, ассоциированную с nlmsghdr
    // https://www.opennet.ru/cgi-bin/opennet/man.cgi?topic=netlink&category=3
    pr_info("Received from User Space: %s\n", (char *)nlmsg_data(nmh));

    msg_size = strlen(msg);
    pid = nmh->nlmsg_pid;

    skb_out = nlmsg_new(msg_size, 0);

    if (!skb_out) {
        pr_err("Failed to allocate new skb\n");
    }

    // Второй аргумент - u32 portid - не влияет на отправку
    nmh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    // NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
    strncpy(nlmsg_data(nmh), msg, msg_size);

    // Одноадресная передача сообщения в один сокет
    res = nlmsg_unicast(nl_sk, skb_out, pid);
    if (res < 0) {
        pr_info("Error while sending to User Space\n");
    }
}

static struct netlink_kernel_cfg cfg = {
    // .groups = 1,
    .input = hello_nl_recv_msg,
};

static int __init netlink_init(void) {
    pr_info("Entering %s\n", __FUNCTION__);

    nl_sk = netlink_kernel_create(&init_net, UNIT, &cfg);
    if (!nl_sk) {
        pr_alert("Error creating socket\n");
        return -10;
    }
    return 0;
}

static void __exit netlink_exit(void) {
    pr_info("Entering %s\n", __FUNCTION__);
    netlink_kernel_release(nl_sk);
}

module_init(netlink_init);
module_exit(netlink_exit);
