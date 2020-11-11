#ifndef myhead
#define myhead

#include <sys/types.h>
#include <stdio.h>


int client(key_t key, int n, char massive[][30]);
int server(key_t key, int n);
int wait_sim(struct sembuf sops[2], int* rc, int* semid);
void cleanup(int rc, int semid);
int init_sim(struct sembuf sops[2], int* rc, int* semid, int op, int semflag);
void subtraction(struct sembuf sops[2], int* rc, int* semid);


#endif