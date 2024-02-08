/* Esercizio 1:
 * Scrivere un’applicazione Client-Server basata su FIFO. Il server crea una FIFO, e attende un messaggio. Il
 * messaggio e’ un vettore di 2 interi [a,b]. Se a < b, il server stampa: “a e’ minore di b”; se a >= b il server
 * stampa: “a e’ maggiore o uguale a b”. Dopo aver stampato a video la stringa, il server rimuove la FIFO,
 * ed infine termina. Il client chiede all’utente due interi, invia i due numeri al server tramite la FIFO ed
 * infine termina. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>


int main(int argc, char *argv[]) {

    int vec[2];

    /* Create the FIFO */
    if (mkfifo("tmp/c2sFifo", S_IRUSR | S_IWUSR ) == -1) {
        perror("mkfifo failed");
        exit(1);
    }

    
    /* Create client */ 
    pid_t pid = fork();
    if (pid == 0) { /* Child */ 
        
        int fifoDesc = open("tmp/c2sFifo", O_WRONLY);
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

        /* Opening fifo */ 
        int fifoDesc = open("tmp/c2sFifo", O_RDONLY);
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

        /* Close the fifo and terminate */ 
        if (close(fifoDesc) == -1) {
            perror("close failed");
            exit(1);
        }

        if (unlink("tmp/c2sFifo") == -1) {
            perror("unlink failed");
            exit(1);
        }

    }
    return 0;
}
