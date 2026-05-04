#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#define PID_FILE ".monitor_pid"

static void handle_usr1(int sig) {
    (void)sig;
    const char msg[] = "Monitor: New report received (SIGUSR1)\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

/* ---------------- SIGINT handler ---------------- */
static void handle_int(int sig) {
    (void)sig;

    const char msg[] = "\nMonitor: shutting down (SIGINT)\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);

    unlink(PID_FILE);

    _exit(0);
}

int main(void) {

    int fd = open(PID_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open .monitor_pid");
        exit(EXIT_FAILURE);
    }

    /* write PID */
    char buf[64];
    int len = snprintf(buf, sizeof(buf), "%d\n", getpid());

    if (write(fd, buf, len) != len) {
        perror("write .monitor_pid");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);

    printf("Monitor started. PID = %d\n", getpid());
    struct sigaction sa_usr1;
    memset(&sa_usr1, 0, sizeof(sa_usr1));
    sa_usr1.sa_handler = handle_usr1;
    sa_usr1.sa_flags = SA_RESTART;

    if (sigaction(SIGUSR1, &sa_usr1, NULL) < 0) {
        perror("sigaction SIGUSR1");
        exit(EXIT_FAILURE);
    }

    struct sigaction sa_int;
    memset(&sa_int, 0, sizeof(sa_int));
    sa_int.sa_handler = handle_int;
    sa_int.sa_flags = SA_RESTART;

    if (sigaction(SIGINT, &sa_int, NULL) < 0) {
        perror("sigaction SIGINT");
        exit(EXIT_FAILURE);
    }

    while (1) {
        pause();
    }

    return 0;
}