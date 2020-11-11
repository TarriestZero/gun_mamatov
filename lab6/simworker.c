#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>

void cleanup(int rc, int semid){
    if (semid > 0)
        semctl(semid, 1, IPC_RMID);
    exit(rc);
}

//инициализация симафора
int init_sim(struct sembuf sops[2], int* rc, int* semid, int op, int semflag)
{
    key_t keysf = 0;
    keysf = ftok(".", 'a');
    sops[0].sem_num = 0;
    sops[0].sem_op = op;
    sops[0].sem_flg = 0;
    if (keysf < 0) {
        perror("ftok");
        cleanup(*rc, *semid);
    }
    *semid = semget(keysf, 1, semflag);
    if (*semid < 0) {
        perror("semget --- ");
        cleanup(*rc, *semid);
    }
    
}

//функция ожидания семафора
int wait_sim(struct sembuf sops[2], int* rc, int* semid)
{

    printf("client pid is %d\n", getpid());
    printf("client waiting for semaphore zero value\n");
    if (semop(*semid, sops, 1) < 0) {
        perror("semop");
        cleanup(*rc, *semid);
    }
    printf("client detected semaphore zero value\n");
    *rc = 0;
    return *rc;
}

//семафор операция - 1
void subtraction(struct sembuf sops[2], int* rc, int* semid){
    sops[0].sem_op = -1; // decrement semaphore back to zero
    if (semop(*semid, sops, 1) < 0) {
        perror("semop");
        cleanup(*rc, *semid);
    }
}


