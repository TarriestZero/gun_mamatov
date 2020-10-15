#ifndef myhead
#define myhead

#include <stdio.h>

void client(int readfd[][2], int writefd[][2], size_t n, char massive[][20]);
void server(int readfd, int writefd);

#endif