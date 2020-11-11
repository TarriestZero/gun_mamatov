#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <fcntl.h> 
#include <stdlib.h>
#include "myhead.h"

#define MSGSZ     128
#define BUF_SIZE 1024



typedef struct msgbuf {
    long    mtype;
    char    mtext[MSGSZ];
} message_buf;


// получение аргрументов из строки (передаваемой пачки из 3х аргументов)
void get_arguments(char* simvol, char* infile, char* outfile, char buff[MSGSZ],ssize_t n){

    int p, trig = 0;
    buff[n] = '\0';	/* полное имя завершается 0 */
    *simvol = buff[n - 1];
    for (int i = 0; i < n; i++)
    {
        if (((int)buff[i] == ' ') && (trig == 0)){ 
            strncpy(infile,buff,i);
            trig++;
            p = i;
        }

        if ((buff[i+1] == ' ') && (trig == 1))
        {
            memcpy(
                outfile,
                &buff[p + 1],
                (int)(strchr(&buff[p + 1], ' ') - &buff[p + 1])
            );
            trig++;
            p = i;
        }

        if ((buff[i+1] == ' ') && (trig == 2))
        {
            *simvol = buff[i + 2];
            break;
        } 
    }
}

//чтение из файла и запись в другой с измененным содержимым
int write_in_file(char* infile, char* outfile, char simvol){
    int fd, ofd, openFlags;
    char buf[BUF_SIZE];
    ssize_t numRead;
    mode_t filePerms;
    fd = open (infile, O_RDONLY);
    if (fd == -1)
    {
        printf ("Error opening file %s\n", infile);
        exit(-2);
    }
    openFlags = O_CREAT | O_WRONLY | O_TRUNC;
    filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; /* rw - rw - rw - */
    ofd = open (outfile, openFlags, filePerms);
    if (ofd == -1)
    {
        printf ("Error opening file %s\n ", outfile) ; exit(-3);
    }
    
    /* Перемещение данных до достижения конца файла ввода или возникновения ошибки */

    char write_buf[BUF_SIZE];

    while ((numRead = read (fd, buf, BUF_SIZE)) > 0)
    {
        for (size_t i = 0; i < numRead; i++)
        {
            if (buf[i] == simvol) write_buf[i] = ' ';
            else write_buf[i] = buf[i];
        }
        
        if (write (ofd, write_buf, numRead) != numRead)
        {
            printf ("couldn't write whole buffer\n "); exit(-4);
        }
        if (numRead == -1)
        {
            printf ("read error\n "); exit(-5);
        }
    }
    if (close (fd) == -1 )
    {
        printf ("close input error\n"); exit(-6);
    }
    if (close (ofd ) == -1 )
    {
        printf ("close output error\n"); exit(-7);
    }

    close(fd);
    return 0;
}


int server(key_t key, int n)
{
    int msgid;
    msgid = msgget(key, 0666);
    message_buf  rbuf;
    ssize_t l;

    // --- работа c симофором
    int rc = 1;
    int semid = 0;
    struct sembuf sops[2];
    init_sim(sops, &rc, &semid, 1, 0666);
    // ----

    char simvol;
    char infile[30] = "";
    char outfile[30] = "";
    for (int i = 0; i < n; i++)
    {   
        memset(infile, 0, sizeof(infile));
        memset(outfile, 0, sizeof(infile));
        l = msgrcv(msgid, &rbuf, MSGSZ, 1, 0); //прием сообщения
        if (l < 0) { 
            perror("msgrcv");
            exit(1);
        }
        printf("recv %s\n", rbuf.mtext);
        get_arguments(&simvol, infile, outfile, rbuf.mtext, l);
        if (write_in_file(infile, outfile, simvol) < 0){
            printf("error in write_in_file");
            return -1;
        }
    }
    subtraction(sops, &rc, &semid); // сообщаем о заврешении действия
    rc = 0;
    cleanup(rc, semid);

}