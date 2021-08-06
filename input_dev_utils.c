#include "input_dev_utils.h"
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>

#define DEV_INPUT_PATH "/dev/input/"
#define BITS_PER_BYTE 8

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1)/(d))
#define BITS_TO_LONGS(nr) DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))
#define GET_ABSOLUTE_PATH(dirpath, filename, out) (snprintf(out, sizeof(out), "%s%s", dirpath, filename))

typedef unsigned long word_t;
enum {BITS_PER_WORD = sizeof(word_t) * BITS_PER_BYTE};
#define WORD_OFFSET(b) ((b) / BITS_PER_WORD)
#define BIT_OFFSET(b) ((b) % BITS_PER_WORD)

static int get_bit(const word_t *bitmap, int n){
    word_t bit = bitmap[WORD_OFFSET(n)] & (1 << BIT_OFFSET(n));
    return bit != 0;
}

static int char_device_selector(const struct dirent *file) {
    struct stat filestat;
    char filename[512];
    int err;

    GET_ABSOLUTE_PATH(DEV_INPUT_PATH, file->d_name, filename);

    err = stat(filename, &filestat);
    if (err) {
        return 0;
    }

    if (strstr(filename, "event") == NULL) {
        return 0;
    }

    return S_ISCHR(filestat.st_mode);
}

char **get_keyboard_event_files(void) {
    struct dirent **namelist;
    char **keyboards = calloc(5, sizeof(char *));
    int n, fd, is_keyboard, no_keyboards = 0;
    int32_t keys[] = {KEY_A, KEY_B, KEY_C, KEY_ESC, KEY_F1, KEY_0, KEY_9};
    int32_t event_bits = 0;
    unsigned long keybit[BITS_TO_LONGS(KEY_CNT)];
    char filename[512];

    n = scandir(DEV_INPUT_PATH, &namelist, &char_device_selector, alphasort);
    if (n < 0) {
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        event_bits = 0;
        GET_ABSOLUTE_PATH(DEV_INPUT_PATH, namelist[i]->d_name, filename);
        fd = open(filename, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "Failed to open %s.\n", filename);
            continue;
        }

//        get event bits
        ioctl(fd, EVIOCGBIT(0, sizeof(event_bits)), &event_bits);

//        check if EV_KEY bit is set
        if ((EV_KEY & event_bits) == EV_KEY) {
//            get keybits
            ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keybit)), &keybit);
            is_keyboard = 1;
//            check if device supports some standard keys; if it does, it probably is a keyboard
            for (int j = 0; j < sizeof(keys) / sizeof(keys[0]); ++j) {
                is_keyboard &= get_bit(keybit, keys[j]);
            }
            if(is_keyboard && no_keyboards < 5){
                keyboards[no_keyboards++] = strdup(filename);
            }
        }
        close(fd);
    }

//    cleanup heap
    for (int i = 0; i < n; i++) {
        free(namelist[i]);
    }
    free(namelist);

    return keyboards;
}
