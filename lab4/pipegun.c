#include <sys/types.h> 
#include <sys/stat.h> 
#include <wait.h> 
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "myhead.h"


#define BUF_SIZE 1024
#define MAXLINE  80

int main(int argc, char *argv[]){      
    char lenh = 0;  
    
    if (!(((argc - 1) % 3) == 0) || (argc < 3)) {
        printf("Usage: file textfile1 textfile2 simvol ...\n");
        exit(-1);
    }
    int lenargvs[(argc / 3) + 1];
    lenargvs[0] = 0;
    char mass_of_ukaz[argc / 3][30];   // массив указателей на начало новых 3х входных параметров
    for (size_t g = 0; g < argc / 3; g++) // 
    {
        for (size_t i = 1; i < 4; i++)  // копирование входных переменных в строку.
        {
            strncpy(&mass_of_ukaz[g][lenh], argv[i + (g * 3)], strlen(argv[i + (g * 3)]));
            strncpy(&mass_of_ukaz[g][strlen(argv[i + (g * 3)]) + lenh], " ", 1);
            lenh = strlen(argv[i + (g * 3)]) + lenh + 1;
        }
        lenh = 0;
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


