#include "unp.h"
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[])
{
    int listenfd, connfd;
    socklen_t len;
    struct sockaddr_in servaddr, cliaddr;
    char buff[MAXLINE];
    time_t ticks;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(13);

    Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    while (1)
    {
        pid_t pid;
        len = sizeof(cliaddr);
        connfd = Accept(listenfd, (SA *)&cliaddr, &len);
        if ((pid = fork()) == 0)
        {
            Close(listenfd);
            printf("connection from %s, port %d\n",
                   inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
                   ntohs(cliaddr.sin_port));

            ticks = time(NULL);
            snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
            Write(connfd, buff, strlen(buff));
            Close(connfd);
            //这句exit(0)非常重要, 确保子进程在完成任务之后准确的退出
            //不然会导致重复Close(connfd);
            exit(0);
        }
        Close(connfd);
    }
}
