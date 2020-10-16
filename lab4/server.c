#include <sys/types.h> 
#include <sys/stat.h> 
#include <wait.h> 
#include <fcntl.h> 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define BUF_SIZE 1024
#define MAXLINE  80


void server(int readfd, int writefd)
{
    int fd, ofd, openFlags;
    ssize_t n, numRead;
    mode_t filePerms;
    char buff[MAXLINE+1];
    char simvol;
    char infile[30] = "";
    char outfile[30] = "";
    char buf[BUF_SIZE];
    /* получение полного имени из канала IPC */
    if ((n = read(readfd, buff, MAXLINE)) == 0)
    {
        printf("end-of-file while reading pathname");
        exit(1);
    }

    int p, trig = 0;
    buff[n] = '\0';	/* полное имя завершается 0 */
    simvol = buff[n - 1];
    for (int i = 0; i < n; i++)
    {
        if (((int)buff[i] == ' ') && (trig == 0)){ 
            strncpy(infile,buff,i);
            trig++;
            p = i;
        }

        if ((buff[i+1] == ' ') && (trig == 1))
        {
            memccpy(outfile,&buff[p + 1], ' ', 15);
            trig++;
            p = i;
        }

        if ((buff[i+1] == ' ') && (trig == 2))
        {
            simvol = buff[i + 2];
            printf("simvol -- %c\n", simvol);
            break;
        }
        
    }

    fd = open (infile, O_RDONLY);
    if (fd == -1)
    {
        snprintf(infile, sizeof(buff)-n,": can't open  %s\n",strerror(errno));
        n = strlen(buff);
        write(writefd, buff, n);
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

    /* файл успешно открыт и копируется в канал */
    while ( (n = read(fd, buff, MAXLINE)) > 0)
        write(writefd, buff, n);

    close(fd);
}