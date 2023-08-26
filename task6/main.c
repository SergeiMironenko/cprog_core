#include <sys/socket.h>
#include <stdio.h>
#include <linux/netlink.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define NETLINK_USER 31
#define MAX_PAYLOAD 1024

int sock_fd, len;
struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nmh = NULL;
struct iovec iov;
struct msghdr msg;

void main() {

    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_USER);
    if (sock_fd == -1) {
        perror("socket");
    }

    memset(&src_addr, 0, sizeof src_addr);
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();

    bind(sock_fd, (struct sockaddr *)&src_addr, sizeof src_addr);

    memset(&dest_addr, 0, sizeof dest_addr);
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;  // Если ядро, то pid = 0
    dest_addr.nl_groups = 0;  // unicast, битовая маска

    // Выделение максимально возможного заголовка
    len = NLMSG_SPACE(MAX_PAYLOAD);
    nmh = (struct nlmsghdr *)malloc(len);
    memset(nmh, 0, len);
    nmh->nlmsg_len = len;
    nmh->nlmsg_pid = getpid();
    nmh->nlmsg_flags = 0;

    strcpy(NLMSG_DATA(nmh), "Hello from User Space");

    // iovec описывает данные для чтения или записи
    iov.iov_base = (void *)nmh;
    iov.iov_len = nmh->nlmsg_len;

    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // Отправка сообщения ядру
    printf("Sending message to kernel\n");
    sendmsg(sock_fd, &msg, 0);

    // Прием сообщения от ядра
    printf("Waiting for message from kernel\n");
    recvmsg(sock_fd, &msg, 0);
    printf("Received message payload: %s\n", (char *)NLMSG_DATA(nmh));
    close(sock_fd);
}
