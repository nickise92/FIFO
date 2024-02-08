/* Esercizio 2:
 * Estendere Ese_1 affinche’ il server continui a leggere messaggi dalla FIFO creata. Il server rimuove la FIFO ed
 * infine termina solo quando i due numeri ricevuti sono uguali, o dall’ultimo messaggio ricevuto sono passati piu’
 * di 30 secondi. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>


/* Global fifo descriptor */
int fifoDesc;

/* Alarm signal handler */
void sigHandler(int sig) {
    printf("30 seconds has been passed without communications. Closing process...\n");

    if (unlink("tmp/c2sFifo") == -1) {
        perror("unlink failed");
        exit(1);
    }

    _exit(0);
}
int main(int argc, char *argv[]) {

    /* Signal handler setup */
    if (signal(SIGALRM, sigHandler) == SIG_ERR) {
        perror("Change signal handler failed");
        exit(EXIT_FAILURE);
    }

    int vec[2];

    /* Create the FIFO */
    if (mkfifo("tmp/c2sFifo", S_IRUSR | S_IWUSR ) == -1) {
        perror("mkfifo failed");
        exit(1);
    }

    do {
        /* Create client */
        pid_t pid = fork();
        if (pid == 0) { /* Child */

            fifoDesc = open("tmp/c2sFifo", O_WRONLY);
            if (fifoDesc == -1) {
                perror("open fifo failed");
                exit(1);
            }

            /* Reading the user input */

            printf("Insert the first number: ");
            scanf("%d", &vec[0]);
            printf("Insert the second number: ");
            scanf("%d", &vec[1]);

            /* Send vector to server with fifo */
            if (write(fifoDesc, vec, sizeof(vec)) == -1) {
                perror("write failed");
                exit(1);
            }

            if (close(fifoDesc) == -1) {
                perror("closing fifo failed");
                exit(1);
            }

            _exit(0);
        }

        if (pid > 0) { /* Parent */

            /* Setting the timer */
            int time = 30;
            alarm(time);

            /* Opening fifo */
            fifoDesc = open("tmp/c2sFifo", O_RDONLY);
            if (fifoDesc == -1) {
                perror("open failed");
                exit(1);
            }


            /* Reading from fifo */
            if (read(fifoDesc, &vec, sizeof(vec)) == -1) {
                perror("read failed");
                exit(1);
            }

            /* Print the result */
            if (vec[0] < vec[1]) {
                printf("A is less than B\n");
            } else {
                printf("A is greater or equal to B\n");
            }
        }
    } while(vec[0] != vec[1]);

    if (close(fifoDesc) == -1) {
        perror("closing fifo failed");
        exit(1);
    }

    if (unlink("tmp/c2sFifo") == -1) {
        perror("unlink failed");
        exit(1);
    }

    return 0;
}
