#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<assert.h>

#define BUF_SIZE 200
#define MAX_EVENTS 10
int main(int argc,char* argv[])
{
    if(argc <= 2){
        printf("too low num");
        exit(0);
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

    int sockfd;
    struct sockaddr_in serveraddr;
    char buff[BUF_SIZE];
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = inet_addr(ip);
    
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    assert(sockfd != -1);

    int res = bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
    assert(res != -1);

    res = listen(sockfd,5);
    assert(res != -1);

    int epoll_fd = epoll_create(MAX_EVENTS);
    assert(epoll_fd != -1);

    struct epoll_event ev;
    struct epoll_event events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;

    res = epoll_ctl(epoll_fd,EPOLL_CTL_ADD,sockfd,&ev);
    assert(res != -1);

    while(1){
        int nfds = epoll_wait(epoll_fd,events,MAX_EVENTS,-1);
        assert(nfds != -1);
        int i;
        for(i=0;i<nfds;i++){
            if(events[i].data.fd == sockfd){
                struct sockaddr_in clientaddr;
                socklen_t clientlen = sizeof(clientlen);
                int connfd = accept(sockfd,(struct sockaddr*)&clientaddr,&clientlen);
                assert(connfd != -1);
                ev.events = EPOLLIN;
                ev.data.fd = connfd;
                int ret = epoll_ctl(epoll_fd,EPOLL_CTL_ADD,connfd,&ev);
                assert(ret != -1);
                printf("accept client %s\n",inet_ntoa(clientaddr.sin_addr));
            }else if(events[i].events & EPOLLIN){
                bzero(&buff,sizeof(buff));
                int len = recv(events[i].data.fd,buff,BUF_SIZE,0);
                if(len <= 0){
                    ev.events = EPOLLIN;
                    ev.data.fd = events[i].data.fd;
                    int rf = epoll_ctl(epoll_fd,EPOLL_CTL_DEL,events[i].data.fd,&ev);
                    assert(rf != -1);
                }else{
                    printf("recvive message:%s\n",buff);
                }
            }
        }
    }


    exit(0);
}
