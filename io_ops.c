#include "io_ops.h"
#include "header.h"

int read_a_char(int fd) {
    char ch;
    read(fd, &ch, 1);

    return ch;
}

int read_line(int fd, char *buf, int size) {
    int i;
    for (i = 0; i < size - 1; i++) {
    again:
        if (read(fd, buf + i, 1) < 0)
            if (errno == EINTR)
                goto again;
        if (*(buf + i) == '\n')
            break;
    }
    if (*(buf + i) != '\n')
        return -1;
    *(buf + i) = '\0';
    return i;
}

int getline_fd(int fd, char *buf) {
    int cnt;
    for (cnt = 0; (*(buf + cnt) = read_a_char(fd)) 
        != '\n'; cnt++) ;
    *(buf + cnt) = '\0';
    return cnt;
}

int init_epoll(int rset[], int rnum, int wset[], int wnum) {
    int epfd;

    if ((epfd = epoll_create(rnum + wnum)) < 0)
        error_dump("fail to create epoll.");
    for (int i = 0; i < rnum; i++)
        if (add_epoll_read_event(epfd, rset[i]) < 0) {
            close(epfd);
            error_dump("fail to add epoll read.");
        }
    for (int i = 0; i < wnum; i++)
        if (add_epoll_write_event(epfd, wset[i]) < 0) {
            close(epfd);
            error_dump("fail to add epoll write.");
        }
    return epfd;
}

int add_epoll_read_event(int epfd, int fd) {
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;

    return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
}

int add_epoll_write_event(int epfd, int fd) {
    struct epoll_event event;
    event.events = EPOLLOUT;
    event.data.fd = fd;

    return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
}

int delete_epoll_read_event(int epfd, int fd) {
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;

    return epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &event);
}

int delete_epoll_write_event(int epfd, int fd) {
    struct epoll_event event;
    event.events = EPOLLOUT;
    event.data.fd = fd;

    return epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &event);
}

int modify_epoll_to_read_event(int epfd, int fd) {
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;

    return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);
}

int modify_epoll_to_write_event(int epfd, int fd) {
    struct epoll_event event;
    event.events = EPOLLOUT;
    event.data.fd = fd;

    return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);
}