
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

#define MAX_FD 10                   //FD_SET֧������������

class CSockServer {
private:
    int sockfd;
    int fd_all[MAX_FD];                  //��������������������select���ú��ж��ĸ��ɶ�
    //������������ԭ����select���ú󣬻ᷢ���仯���ٴε���selectǰ����Ҫ���¸�ֵ
    fd_set fd_read;                      //FD_SET���ݱ���
    fd_set fd_select;                    //����select
    int maxfd;
    struct sockaddr_in cli_addr;      //�ͻ��˵�ַ
    struct timeval timeout;           //��ʱʱ�䱸��
    struct timeval timeout_select;  //����select
public:
    CSockServer(int portnum);
    int readdata(int* fd, char* buf, int size);
    int writedata(int fd, char* buf, int size);
};
#endif
