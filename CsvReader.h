#ifndef CSVREADER_H
#define CSVREADER_H
#include <stdio.h>
#include <stdlib.h>

//Parser for finding pairs of spec_ids in the csv file
HashTable *csvParser(char *filename, HashTable **ht);

#endif