#include <sys/types.h>
#include <sys/stat.h>
#include <wait.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    errno = 0;
    int i, pid[argc], status, stat;
    char file1[20], file2[20];
    char* sim;
    // для всех файлов, перечисленных в командной строке 
    if (!(((argc - 1) % 3) == 0) || (argc < 3)) {
        printf("Usage: file textfile1 textfile2 simvol ...\n");
        exit(-1);
        }
    for (i = 1; i <= argc / 3 ; i++) { 
    // запускаем дочерний процесс 
    strcpy(file1,argv[i + (2 * (i - 1))]);
    strcpy(file2,argv[(i + 1) + (2 * (i - 1))]);
    sim = argv[(i + 2) + (2 * (i - 1))];
    pid[i] = fork(); 
    if (pid[i] == 0) {
    // если выполняется дочерний процесс 
    // вызов функции счета количества пробелов в файле

        if (execl("./lab3D","lab3D",file1, file2, sim, NULL)<0) {
        printf("ERROR while start processing file %s\n",argv[i]);
        exit(-2);
        }
        else printf( "processing of file %s started (pid=%d)\n", argv[i],pid[i]);
    }
    // если выполняется родительский процесс
    }
    sleep(1);
    // ожидание окончания выполнения всех запущенных процессов
    for (i = 1; i <= argc / 3; i++) { 
        status=waitpid(pid[i],&stat,0);
        if (pid[i] == status) {
        printf("File %s done,  result=%d\n",argv[i + (2 * (i - 1))],WEXITSTATUS(stat));
        }
    }
    return 0;

 }