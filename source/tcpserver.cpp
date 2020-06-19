#include "../include/tcpserver.h"

CSockServer::CSockServer(int portnum)
{
    int err;
    struct sockaddr_in serv_addr;   //服务器地址
    socklen_t serv_len;

    //超时时间设置
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("fail to socket");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(&cli_addr, 0, sizeof(cli_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portnum);
    serv_addr.sin_addr.s_addr = inet_addr("0.0.0.0");

    serv_len = sizeof(serv_addr);

    err = bind(sockfd, (struct sockaddr *)&serv_addr, serv_len);
    if(err < 0)
    {
        perror("fail to bind");
        exit(1);
    }

    err = listen(sockfd, 20);

    if(err < 0)
    {
        perror("fail to listen");
        exit(1);
    }

    //初始化fd_all数组
    memset(&fd_all, -1, sizeof(fd_all));
    fd_all[0] = sockfd;         //第一个为监听套接字
    FD_ZERO(&fd_read);         //清空集合
    FD_SET(sockfd, &fd_read);  //将监听套接字加入fd_set集合

    maxfd = fd_all[0];  //监听的最大套接字描述符

    printf("listen %d ok\n", portnum);
}

int CSockServer::readdata(int* fd, char* buf, int size)
{
    int i;
    int err;
    int num;
    int connfd;
    socklen_t cli_len;

    //每次都需要重新赋值
    fd_select = fd_read;
    timeout_select = timeout;

    err = select(maxfd+1, &fd_select, NULL, NULL, (struct timeval *)&timeout_select);
    if(err < 0)
    {
        perror("fail to select");
        exit(1);
    }

    if(err == 0)
        printf("timeout\n");

    //检测监听套接字是否可读
    if(FD_ISSET(sockfd, &fd_select))
    {
        //可读，证明有新客户端连接服务器
        cli_len = sizeof(cli_addr);
        connfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);
        if(connfd < 0)
        {
            perror("fail to accept");
            exit(1);
        }

        //将新连接套接字加入fd_all及fd_read
        for(i=0; i<MAX_FD; i++)
        {
            if(fd_all[i] != -1)
            {
                continue;
            }
            else
            {
                fd_all[i] = connfd;
                printf("client fd_all[%d] join\n", i);
                break;
            }
        }

        FD_SET(connfd, &fd_read);

        if(maxfd < connfd)
        {
            maxfd = connfd;  //更新maxfd
        }
    }

    //从1开始查看连接套接字是否可读，因为上面已经处理过0（sockfd）
    for(i=1; i<MAX_FD; i++)
    {
        if(fd_all[i] == -1)
        {
            continue;
        }

        if(FD_ISSET(fd_all[i], &fd_read))
        {
            printf("fd_all[%d] is ok\n", i);
            num = read(fd_all[i], buf, size);

            if (num < 0)
            {
                printf("读取失败,返回%d\n", num);
                continue;
            }

            if(num > 0)
            {
#if 0
                if(strncmp("exit", buf, 4) == 0)
                {
                    //客户端退出，关闭套接字，并从监听集合清除
                    printf("client:fd_all[%d] exit\n", i);
                    FD_CLR(fd_all[i], &fd_read);
                    close(fd_all[i]);
                    fd_all[i] = -1;

                    continue;
                }
#endif
                memcpy(fd, &i, 4);

                return num;
                //收到 客户端数据并打印
                buf[num] = '\0';
                printf("receive buf from client fd_all[%d] is: %s\n", i, buf);

                //回复客户端
                num = write(fd_all[i], "ok", sizeof("ok"));
                if(num < 0)
                {
                    perror("fail to write ");
                    exit(1);
                }
                else
                {
                    printf("send reply\n");
                }
            }
            else if (err == 1)  //select 认为有1个自己数据，但是read读不出来，很可能就是断开消息
            {
                printf("client:fd_all[%d] exit\n", i);
                FD_CLR(fd_all[i], &fd_read);
                close(fd_all[i]);
                fd_all[i] = -1;
                maxfd = sockfd;

                for(i=0; i<MAX_FD; i++)
                {
                    if(fd_all[i] != -1)
                    {
                        continue;
                    }

                    if (fd_all[i] > maxfd)
                        maxfd = fd_all[i];
                }
            }
        }
        else
        {
            //printf("no data\n");
        }
    }

    return 0;
}

int CSockServer::writedata(int fd, char* buf, int size)
{
    int num;

    if (fd >= MAX_FD) {
        printf("fd >= %d is error!", MAX_FD);

        return -1;
    }

    if (fd_all[fd] <= 0 || buf == NULL || num < 1) {
        printf("wrong argument: fd is %d, buf is %p, num is %d", fd_all[fd], buf, num);

        return -2;
    }

    //回复客户端
    num = write(fd_all[fd], buf, size);
    if(num < 0)
    {
        printf("client:fd_all[%d] exit\n", fd);
        FD_CLR(fd_all[fd], &fd_read);
        close(fd_all[fd]);
        fd_all[fd] = -1;
        maxfd = sockfd;

        for(int i=0; i<MAX_FD; i++)
        {
            if(fd_all[i] != -1)
            {
                continue;
            }

            if (fd_all[i] > maxfd)
                maxfd = fd_all[i];
        }

        return 0;
    }

    return num;
}

using namespace std;

std::vector<CSockServer*> gServerList;
int gServerNum = 0;

#if 0
int main(void)
{
    int     fd = 0;
    uint8_t buf[1024]= {0};
    CSockServer *server = new CSockServer(6050);

    while (true)
    {
        int len = server->readdata(&fd, buf, 1024);

        if (len <= 0) {
            usleep(10000);

            continue;
        }

        printf("received:");
        for (int i = 0; i < len; i++)
            printf("%02X", buf[i]);
    }

    return 0;
}
#endif

#if 0
extern "C" {
int startserver(int port)
{
    if (gServerList.size() < 100)
    {
        gServerList.resize(100);
    }

    if (gServerNum >= gServerList.size())
    {
        gServerList.resize(gServerNum << 1);
    }

    gServerList[gServerNum] = new CSockServer(port);

    if (gServerList[gServerNum] == NULL)
        return -1;

    return gServerNum++;
}

int serverread(int server, char* fd, char* buf, int size)
{
    if (server >= gServerList.size())
        return -1;

    if (gServerList[server] == NULL)
        return -1;

    return gServerList[server]->readdata(fd, buf, size);
}

int serverwrite(int server, int fd, char* buf, int size)
{
    if (server >= gServerList.size())
        return -1;

    if (gServerList[server] == NULL)
        return -1;

    return gServerList[server]->writedata(fd, buf, size);
}
};
#endif

