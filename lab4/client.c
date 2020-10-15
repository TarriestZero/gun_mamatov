#include <sys/types.h> 
#include <sys/stat.h> 
#include <wait.h> 
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXLINE  80


void client(int readfd[][2], int writefd[][2], size_t n, char massive[][20])
{
    ssize_t f;
    char buff[MAXLINE];
    /* запись полного имени в канал IPC */
    for(int i = 0; i < n; i++) write(writefd[i][1], massive[i], strlen(massive[i]));
    /* считывание из канала, вывод в stdout */
    for (int i = 0; i < n; i++){
    while ( (f = read(readfd[i][0], buff, MAXLINE)) > 0) 
        write(STDOUT_FILENO, buff, n);	
    }
}