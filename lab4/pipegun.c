#include <sys/types.h> 
#include <sys/stat.h> 
#include <wait.h> 
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define BUF_SIZE 1024
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
        }
    }

    printf("-----%s\n", outfile);
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

int main(int argc, char *argv[]){      
    char bufarg[argc * 20], lenh = 0;  
    
    if (!(((argc - 1) % 3) == 0) || (argc < 3)) {
        printf("Usage: file textfile1 textfile2 simvol ...\n");
        exit(-1);
    }
    int lenargvs[(argc / 3) + 1];
    lenargvs[0] = 0;
    char mass_of_ukaz[argc / 3][20];   // массив указателей на начало новых 3х входных параметров
    for (size_t g = 0; g < argc / 3; g++) // 
    {
        for (size_t i = 1; i < 4; i++)  // копирование входных переменных в строку.
        {
            strcpy(&bufarg[lenh], argv[i + (g * 3)]);
            strcpy(&bufarg[strlen(argv[i + (g * 3)]) + lenh], " ");
            lenh = strlen(argv[i + (g * 3)]) + lenh + 1;
        }
        lenargvs[g + 1] = lenh; // длина каждых 3х аргументов
    }

    for (int f = 0; f < argc / 3; f++)
    {
        strncpy(mass_of_ukaz[f],&bufarg[lenargvs[f]],lenargvs[f + 1] - lenargvs[f] - 1);
    }
    
    int P_pipe[argc / 3][2]; // каналы для родителя к дочерним
    int pipes[argc / 3][2]; // каналы дочерних
    pid_t childpid[argc / 3];
    for (int g = 0; g < argc / 3; g++){
    pipe(P_pipe[g]);
    pipe(pipes[g]);
    }
    for (int g = 0; g < argc / 3; g++){
        if ((childpid[g] = fork()) == 0) { /* child */
            close(P_pipe[g][1]);
            close(pipes[g][0]);
            server(P_pipe[g][0], pipes[g][1]);
            exit(0);
        }
    }
    /* родитель */
    for (int g = 0; g < argc / 3; g++) {close(pipes[g][1]); close(P_pipe[g][0]); }
    client(pipes, P_pipe, argc / 3, mass_of_ukaz);
    for (int g = 0; g < argc / 3; g++) 
        waitpid(childpid[g], NULL, 0); 
    /*ожидание завершения дочернего процесса */
    exit(0);
}


