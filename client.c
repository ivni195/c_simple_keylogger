#include "client.h"
#include <arpa/inet.h>
#include <string.h>

int is_socket_connected(int sockfd, int *error_code){
    socklen_t error_code_len = sizeof(error_code);
    return getsockopt(sockfd, SOL_SOCKET, SO_ERROR, error_code, &error_code_len);
}

int create_socket(void){
    return socket(AF_INET, SOCK_STREAM, 0);
}

int connect_socket(int sockfd, const char *addr, uint16_t port) {
    struct sockaddr_in servaddr;

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(addr);
    servaddr.sin_port = htons(port);

    return connect(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr));
}
