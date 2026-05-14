#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void start_monitor() {
    int pipefd[2];
    pipe(pipefd);

    pid_t hub_mon = fork();

    if (hub_mon == 0) {

        pid_t mon = fork();

        if (mon == 0) {

            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);

            execl("./monitor_reports", "monitor_reports", NULL);
            perror("exec");
            exit(1);
        }

        close(pipefd[1]);

        char buf[256];
        int n;

        while ((n = read(pipefd[0], buf, sizeof(buf) - 1)) > 0) {
            buf[n] = 0;
            printf("[MONITOR] %s", buf);
        }

        if (n == 0) {
            printf("[HUB] Monitor ended\n");
        }

        close(pipefd[0]);
        exit(0);
    }

    if (hub_mon > 0) {
        printf("[HUB] Monitor started (hub_mon=%d)\n", hub_mon);
    }
}


void calculate_scores(char *districts[], int count) {
    printf("\n[HUB] WORKLOAD REPORT START\n");

    for (int i = 0; i < count; i++) {

        int pipefd[2];
        pipe(pipefd);

        pid_t pid = fork();

        if (pid == 0) {

            close(pipefd[0]);

            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);

            execl("./scorer", "scorer", districts[i], NULL);

            perror("exec scorer");
            exit(1);
        }

        // PARENT

        close(pipefd[1]);

        char buffer[256];
        int n;

        // IMPORTANT: read FIRST, then wait
        while ((n = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[n] = '\0';
            printf("%s", buffer);
        }

        close(pipefd[0]);

        waitpid(pid, NULL, 0);
    }

    printf("[HUB] WORKLOAD REPORT END\n\n");
}

int main() {

    char command[128];

    while (1) {

        printf("city_hub> ");
        fflush(stdout);

        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }

        command[strcspn(command, "\n")] = '\0';

        /* START MONITOR */
        if (strcmp(command, "start_monitor") == 0) {
            start_monitor();
        }
        else if (strncmp(command, "calculate_scores", 16) == 0) {

            printf("DEBUG: entered calculate_scores block\n");

            char copy[256];
            strncpy(copy, command, sizeof(copy));
            copy[sizeof(copy) - 1] = '\0';

            char *args[20];
            int count = 0;

            char *token = strtok(copy, " ");
            token = strtok(NULL, " ");

            while (token != NULL) {
                args[count++] = token;
                token = strtok(NULL, " ");
            }

             printf("DEBUG: count = %d\n", count);

            if (count == 0) {
                 printf("[HUB] No districts provided\n");
            } else {
                calculate_scores(args, count);
            }
        }

        /* EXIT */
        else if (strcmp(command, "exit") == 0) {
            printf("[HUB] Shutting down\n");
            break;
        }

        
    }

    return 0;
}