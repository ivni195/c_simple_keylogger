#include <stdint-gcc.h>

#ifndef C_SIMPLE_KEYLOGGER_CLIENT_H
#define C_SIMPLE_KEYLOGGER_CLIENT_H

int is_socket_connected(int sockfd, int *error_code);
int create_socket(void);
int connect_socket(int sockfd, const char *addr, uint16_t port);

#endif //C_SIMPLE_KEYLOGGER_CLIENT_H
