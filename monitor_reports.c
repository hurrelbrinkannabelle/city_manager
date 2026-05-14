// #include <stdio.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <signal.h>
// #include <stdlib.h>
// #include <string.h>

// #define PID_FILE ".monitor_pid"

// static void handle_usr1(int sig) {
//     (void)sig;
//     const char msg[] = "EVENT Monitor: New report received (SIGUSR1)\n";
//     write(STDOUT_FILENO, msg, sizeof(msg) - 1);
// }

// /* ---------------- SIGINT handler ---------------- */
// static void handle_int(int sig) {
//     (void)sig;

//     const char msg[] = "\nEXIT Monitor: shutting down (SIGINT)\n";
//     write(STDOUT_FILENO, msg, sizeof(msg) - 1);

//     unlink(PID_FILE);

//     _exit(0);
// }

// int main(void) {

//     int fd = open(PID_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644);
//     if (fd < 0) {
//         perror("open .monitor_pid");
//         exit(EXIT_FAILURE);
//     }

//     /* write PID */
//     char buf[64];
//     int len = snprintf(buf, sizeof(buf), "%d\n", getpid());

//     if (write(fd, buf, len) != len) {
//         perror("write .monitor_pid");
//         close(fd);
//         exit(EXIT_FAILURE);
//     }

//     close(fd);
//     printf("Monitor started. PID = %d\n", getpid());
//     struct sigaction sa_usr1;
//     memset(&sa_usr1, 0, sizeof(sa_usr1));
//     sa_usr1.sa_handler = handle_usr1;
//     sa_usr1.sa_flags = SA_RESTART;

//     if (sigaction(SIGUSR1, &sa_usr1, NULL) < 0) {
//         perror("sigaction SIGUSR1");
//         exit(EXIT_FAILURE);
//     }

//     struct sigaction sa_int;
//     memset(&sa_int, 0, sizeof(sa_int));
//     sa_int.sa_handler = handle_int;
//     sa_int.sa_flags = SA_RESTART;

//     if (sigaction(SIGINT, &sa_int, NULL) < 0) {
//         perror("sigaction SIGINT");
//         exit(EXIT_FAILURE);
//     }

//     while(1){
//         pause();
//     }

//     return 0;
// }

// #include <stdio.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <signal.h>
// #include <stdlib.h>
// #include <string.h>

// #define PID_FILE ".monitor_pid"

// static void handle_usr1(int sig) {
//     (void)sig;
//     const char msg[] = "EVENT Monitor: New report received (SIGUSR1)\n";
//     write(STDOUT_FILENO, msg, sizeof(msg) - 1);
// }

// /* ---------------- SIGINT handler ---------------- */
// static void handle_int(int sig) {
//     (void)sig;

//     const char msg[] = "\nEXIT Monitor: shutting down (SIGINT)\n";
//     write(STDOUT_FILENO, msg, sizeof(msg) - 1);

//     unlink(PID_FILE);

//     _exit(0);
// }

// int main(void) {

//     int fd = open(PID_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644);
//     if (fd < 0) {
//         perror("open .monitor_pid");
//         exit(EXIT_FAILURE);
//     }

//     /* write PID */
//     char buf[64];
//     int len = snprintf(buf, sizeof(buf), "%d\n", getpid());

//     if (write(fd, buf, len) != len) {
//         perror("write .monitor_pid");
//         close(fd);
//         exit(EXIT_FAILURE);
//     }

//     close(fd);
//     printf("Monitor started. PID = %d\n", getpid());
//     struct sigaction sa_usr1;
//     memset(&sa_usr1, 0, sizeof(sa_usr1));
//     sa_usr1.sa_handler = handle_usr1;
//     sa_usr1.sa_flags = SA_RESTART;

//     if (sigaction(SIGUSR1, &sa_usr1, NULL) < 0) {
//         perror("sigaction SIGUSR1");
//         exit(EXIT_FAILURE);
//     }

//     struct sigaction sa_int;
//     memset(&sa_int, 0, sizeof(sa_int));
//     sa_int.sa_handler = handle_int;
//     sa_int.sa_flags = SA_RESTART;

//     if (sigaction(SIGINT, &sa_int, NULL) < 0) {
//         perror("sigaction SIGINT");
//         exit(EXIT_FAILURE);
//     }

//     while(1){
//         pause();
//     }

//     return 0;
// }


#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#define PID_FILE ".monitor_pid"

/* ---------------- SIGUSR1 handler ---------------- */

static void handle_usr1(int sig) {
    (void)sig;

    const char msg[] =
        "EVENT: New report received\n";

    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

/* ---------------- SIGINT handler ---------------- */

static void handle_int(int sig) {
    (void)sig;

    unlink(PID_FILE);

    const char msg[] =
        "EXIT: Monitor shutting down\n";

    write(STDOUT_FILENO, msg, sizeof(msg) - 1);

    _exit(0);
}

int main(void) {

    /* =======================================================
       CHECK IF ANOTHER MONITOR IS ALREADY RUNNING
       ======================================================= */

    int fd_check = open(PID_FILE, O_RDONLY);

    if (fd_check >= 0) {

        char oldpid[64];

        int n = read(fd_check,
                     oldpid,
                     sizeof(oldpid) - 1);

        if (n > 0) {

            oldpid[n] = '\0';

            char errmsg[128];

            int len = snprintf(errmsg,
                               sizeof(errmsg),
                               "ERROR: Monitor already running with PID %s",
                               oldpid);

            write(STDOUT_FILENO,
                  errmsg,
                  len);
        }

        close(fd_check);

        exit(EXIT_FAILURE);
    }

    /* =======================================================
       CREATE PID FILE
       ======================================================= */

    int fd = open(PID_FILE,
                  O_CREAT | O_WRONLY | O_TRUNC,
                  0644);

    if (fd < 0) {
        perror("open .monitor_pid");
        exit(EXIT_FAILURE);
    }

    char buf[64];

    int len = snprintf(buf,
                       sizeof(buf),
                       "%d\n",
                       getpid());

    if (write(fd, buf, len) != len) {
        perror("write .monitor_pid");
        close(fd);
        exit(EXIT_FAILURE);
    }

    close(fd);

    /* =======================================================
       START MESSAGE
       ======================================================= */

    char startmsg[128];

    len = snprintf(startmsg,
                   sizeof(startmsg),
                   "INFO: Monitor started with PID %d\n",
                   getpid());

    write(STDOUT_FILENO,
          startmsg,
          len);

    /* =======================================================
       SIGUSR1
       ======================================================= */

    struct sigaction sa_usr1;

    memset(&sa_usr1, 0, sizeof(sa_usr1));

    sa_usr1.sa_handler = handle_usr1;
    sa_usr1.sa_flags = SA_RESTART;

    if (sigaction(SIGUSR1,
                  &sa_usr1,
                  NULL) < 0) {

        perror("sigaction SIGUSR1");

        exit(EXIT_FAILURE);
    }

    /* =======================================================
       SIGINT
       ======================================================= */

    struct sigaction sa_int;

    memset(&sa_int, 0, sizeof(sa_int));

    sa_int.sa_handler = handle_int;
    sa_int.sa_flags = SA_RESTART;

    if (sigaction(SIGINT,
                  &sa_int,
                  NULL) < 0) {

        perror("sigaction SIGINT");

        exit(EXIT_FAILURE);
    }

    /* =======================================================
       WAIT FOREVER
       ======================================================= */

    while (1) {
        pause();
    }

    return 0;
}