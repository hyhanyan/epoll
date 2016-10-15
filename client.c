#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>
#include<string.h>

int main(int argc,char* argv[])
{

    if(argc <= 2){
        printf("usage : %s ip_address port_number\n",argv[0]);
        printf("hanyan ----%d\n",argc);
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in address;
    bzero(&address,sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET,ip,&address.sin_addr);
    address.sin_port = htons(port);

    int socket_fd = socket(AF_INET,SOCK_STREAM,0);
    assert(socket_fd != -1);
    int connfd = connect(socket_fd,(struct sockaddr*)&address,sizeof(address));
    if(connfd < 0){
        printf("connection failed\n");
    }else{
        while(1){
            char str[256];
            scanf("%s",str);
            send(socket_fd,str,strlen(str),0);
            if(strcmp(str,"end") == 0){
               return 1;
            }
            bzero(&str,strlen(str));
        }
    }

    close(socket_fd);
    return 0;

}
