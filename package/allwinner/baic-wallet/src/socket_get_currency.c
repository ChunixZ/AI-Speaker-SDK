#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>


#define SOCKET_ADDR "127.0.0.1"
#define PORT 8898
#define BUFFER_SIZE 1024


int main(int argc, char *argv[])
{
    int sock_cli = socket(AF_INET, SOCK_STREAM, 0);
    
    ///定义sockaddr_in
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(SOCKET_ADDR);  ///服务器ip
    servaddr.sin_port = htons(PORT);  ///服务器端口
    
    ///连接服务器，成功返回0，错误返回-1
    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        exit(1);
    }
    
    /*
    char sendbuf[BUFFER_SIZE];
    char recvbuf[BUFFER_SIZE];
    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
    {
        send(sock_cli, sendbuf, strlen(sendbuf), 0); ///发送
        if(strcmp(sendbuf,"exit\n")==0)
            break;
        recv(sock_cli, recvbuf, sizeof(recvbuf), 0); ///接收
        fputs(recvbuf, stdout);

        memset(sendbuf, 0, sizeof(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
    }
    */
    char *sendbuf = "withdraw";
    char recvbuf[BUFFER_SIZE];
    
    memset(recvbuf, 0, sizeof(recvbuf));
    send(sock_cli, sendbuf, strlen(sendbuf), 0); ///发送
    recv(sock_cli, recvbuf, sizeof(recvbuf),0); ///接收
    printf("[Recieve]: %s\n", recvbuf);
    
    close(sock_cli);
    
    return 0;
}
