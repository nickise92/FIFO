/* Esercizio Bonus:
 * Creare il produttore e consumatore con la fifo che prende in input un Path di un file txt, il consumatore crea
 * la fifo, crea un processo figlio che legge il contenuto dal file passato in input, lo mette al contrario al interno
 * della fifo, e il consumatore lo stampa a video */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define MAX_SIZE 1024

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Error: Argument missing.\nUsage: %s <path_to_file.txt>\n", argv[0]);
        exit(0);
    }
    char * path = argv[1];

    /* Create the fifo */
    char *fname = "tmp/fifo1";
    if (mkfifo(fname, S_IWUSR | S_IRUSR) == -1) {
        perror("mkfifo failed");
        exit(EXIT_FAILURE);
    }

    /* Create the producer */
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_SIZE];
    if (pid == 0) { /* Producer - Child */
        char c;
        /* Open fifo write-end */
        int fifoDesc = open(fname, O_WRONLY);

        /* Open file in read-only mode */
        int fileDesc = open(path, O_RDONLY);

        /* Read from file until EOF & write on FIFO */
        ssize_t readByte = -1;
        ssize_t writtenByte = -1;

        off_t currentOffset = lseek(fileDesc, -1, SEEK_END);
        if (currentOffset == -1) {
            perror("lseek failed");
            exit(1);
        }

        int pos = 0;
        while (currentOffset >= 0) {

            /* Read backwards from file */
            readByte = read(fileDesc, &c, sizeof(char));
            if (readByte != sizeof(char)) {
                perror("read failed");
                exit(1);
            } else {
                /* Write to fifo the buffer */
                buffer[pos] = c;
                pos++;
            }
            currentOffset = lseek(fileDesc, -2, SEEK_CUR);

        }

        if (strlen(buffer) > 0) {
            writtenByte = write(fifoDesc, buffer, strlen(buffer));
            if (writtenByte != readByte) {
                perror("FIFO write failed");
                exit(EXIT_FAILURE);
            }
            strcpy(buffer, "");
        }

        if (close(fileDesc) == -1) {
            perror("FILE close failed");
            exit(1);
        }
        if (close(fifoDesc) == -1) {
            perror("FIFO close failed");
            exit(1);
        }
    }

    if (pid > 0) { /* Parent - Consumer */
        /* Opening fifo read-end */
        int fifoDesc = open(fname, O_RDONLY);
        if (fifoDesc == -1) {
            perror("open failed");
            exit(1);
        }

        /* Read from fifo and print to STDOUT */
        ssize_t readByte = -1;

        readByte = read(fifoDesc, buffer, sizeof(buffer));
        if (readByte == -1) {
            perror("read failed");
            exit(EXIT_FAILURE);
        }

        printf("%s", buffer);

        if (close(fifoDesc) == -1) {
            perror("FIFO close failed");
            exit(1);
        }

        if (unlink(fname) == -1) {
            perror("unlink failed");
            exit(1);
        }
    }

    printf("\n");
    return 0;
}
