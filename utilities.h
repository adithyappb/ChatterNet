#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BACKLOG 10
#define DATABASE_FILE "database.txt"

void initializeDatabase();
char *ceasarEncrypt(int key, char *string);
char *ceasarDecrypt(int key, char *string);
char *fgetstr(char *string, int n, FILE *stream);

#endif /* UTILITIES_H */
