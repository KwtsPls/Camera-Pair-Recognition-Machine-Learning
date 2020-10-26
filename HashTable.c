#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Hash function for this hash table -> I used DJB2
unsigned long h2(char *str,int buckets)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c;

    return hash % buckets;
}
