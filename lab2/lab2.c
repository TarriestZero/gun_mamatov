#include "stdio.h"
#include <stdlib.h>

int main (int argc, char * argv [ ])
{
    if (argc != 4)
    {
        printf("Usage: %s old-file new-file \n", argv[0]); exit(-1);
    }
    return copy(argv[1], argv[2], *argv[3]);
}