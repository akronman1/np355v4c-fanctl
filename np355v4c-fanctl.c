#define _DEFAULT_SOURCE
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/io.h>

#define EC_DATA_PORT 0x68
#define EC_STATUS_PORT 0x6C
#define EC_COMMAND 0x59
#define EC_FULL_ON 0x77
#define EC_NORMAL 0x76
#define POLL_LIMIT 0x2710

static int wait_write_ready(void) {
    for (unsigned int attempt = 0; attempt < POLL_LIMIT; ++attempt) {
        if ((inb(EC_STATUS_PORT) & 0x02) == 0) {
            return 0;
        }
    }
    return -1;
}

static int send_mode(unsigned char mode) {
    (void)inb(EC_DATA_PORT);

    if (wait_write_ready() != 0) {
        return -1;
    }
    outb(EC_COMMAND, EC_STATUS_PORT);

    if (wait_write_ready() != 0) {
        return -1;
    }
    outb(mode, EC_DATA_PORT);
    return 0;
}

static void print_usage(const char *program) {
    fprintf(stderr, "Usage: %s full-on|normal\n", program);
}

int main(int argc, char **argv) {
    unsigned char mode;
    const char *mode_name;

    if (argc != 2) {
        print_usage(argv[0]);
        return 64;
    }
    if (strcmp(argv[1], "full-on") == 0) {
        mode = EC_FULL_ON;
        mode_name = "full-on";
    } else if (strcmp(argv[1], "normal") == 0) {
        mode = EC_NORMAL;
        mode_name = "normal";
    } else {
        print_usage(argv[0]);
        return 64;
    }

    if (ioperm(EC_DATA_PORT, EC_STATUS_PORT - EC_DATA_PORT + 1, 1) != 0) {
        fprintf(stderr, "ioperm(0x68..0x6C) failed: %s\n", strerror(errno));
        return 77;
    }

    if (send_mode(mode) != 0) {
        fprintf(stderr, "%s was not sent: EC write-ready timeout\n", mode_name);
        ioperm(EC_DATA_PORT, EC_STATUS_PORT - EC_DATA_PORT + 1, 0);
        return 1;
    }

    printf("%s command write completed\n", mode_name);
    ioperm(EC_DATA_PORT, EC_STATUS_PORT - EC_DATA_PORT + 1, 0);
    return 0;
}
