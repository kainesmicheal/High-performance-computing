#include<netdb.h>
#include<netinet/in.h>
#include<stdio.h>
#include<strings.h>
#include<stdlib.h>
#include<unistd.h>

#define SERVER_ADDR "localhost"
#define SERVER_PORT 8888

int main(int argc, char** argv){
    int socket_fd = 0;
    struct sockaddr server_socket_addr;

    struct hostent* he = gethostbyname(SERVER_ADDR);
    unsigned long server_addr_nho = *(unsigned long*)(he->h_addr_list[0]);

    if(0 > (socket_fd = socket(AF_INET, SOCK_STREAM, 0))){
        fprintf(stderr, "client failed to create socket\n");
        exit(1);
    }
    bzero(&server_socket_addr, sizeof(server_socket_addr));
    server_socket_addr.sin_family = AF_INET;
    server_socket_addr.sin_port = htons(SERVER_PORT);
    server_socket_addr.sin_addr.s_addr = server_addr_nho;

    if(0>connect(socket_fd,(struct sockaddr*)&server_socket_addr,sizeof(server_socket_addr))){
        fprintf(stderr, "client failed to connect to %s:%d!\n",SERVER_ADDR,SERVER_PORT);
        close(socket_fd);
        exit(1);
    } else {
        fprintf(stdout,"client connected to %s:%d!\n",SERVER_ADDR, SERVER_PORT);
    }
    close(socket_fd);
    return 0;

}
