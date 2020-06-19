
#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <unistd.h>
#include <vector>

#define MAX_FD 10                   //FD_SET支持描述符数量

class CSockServer {
private:
    int sockfd;
    int fd_all[MAX_FD];                  //保存所有描述符，用于select调用后，判断哪个可读
    //下面两个备份原因是select调用后，会发生变化，再次调用select前，需要重新赋值
    fd_set fd_read;                      //FD_SET数据备份
    fd_set fd_select;                    //用于select
    int maxfd;
    struct sockaddr_in cli_addr;      //客户端地址
    struct timeval timeout;           //超时时间备份
    struct timeval timeout_select;  //用于select
public:
    CSockServer(int portnum);
    int readdata(int* fd, char* buf, int size);
    int writedata(int fd, char* buf, int size);
};
#endif
