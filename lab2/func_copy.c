#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#define BUF_SIZE 1024


int copy(char* input, char* output, char character){
    int inputFd, outputFd, openFlags;
    mode_t filePerms;
    ssize_t numRead;
    char buf[BUF_SIZE];
    /* Открытие файлов ввода и вывода */
    inputFd = open (input, O_RDONLY);
    if (inputFd == -1)
    {
        printf ("Error opening file %s\n", input) ; exit(-2);
    }
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /* rw - rw - rw - */
    outputFd = open (output, openFlags, filePerms);
    if (outputFd == -1)
    {
        printf ("Error opening file %s\n ", output) ; exit(-3);
    }
    
    /* Перемещение данных до достижения конца файла ввода или возникновения ошибки */

    char write_buf[BUF_SIZE];

    while ((numRead = read (inputFd, buf, BUF_SIZE)) > 0)
    {
        for (size_t i = 0; i < numRead; i++)
        {
            if (buf[i] == character) write_buf[i] = ' ';
            else write_buf[i] = buf[i];
        }
        
        if (write (outputFd, write_buf, numRead) != numRead)
        {
            printf ("couldn't write whole buffer\n "); exit(-4);
        }
        if (numRead == -1)
        {
            printf ("read error\n "); exit(-5);
        }
    }
    if (close (inputFd ) == -1 )
    {
        printf ("close input error\n"); exit(-6);
    }
    if (close (outputFd ) == -1 )
    {
        printf ("close output error\n"); exit(-7);
    }
        
    exit(EXIT_SUCCESS);
}