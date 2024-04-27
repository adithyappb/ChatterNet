#include "utilities.h"

void initializeDatabase() {
    FILE *file = fopen(DATABASE_FILE, "w");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }
    fclose(file);
}

char *ceasarEncrypt(int key, char *string) {
    for (int i = 0; string[i] != '\0'; ++i) {
        char ch = string[i];
        if (ch >= 'a' && ch <= 'z') {
            ch = 'a' + (ch - 'a' + key) % 26;
        } else if (ch >= 'A' && ch <= 'Z') {
            ch = 'A' + (ch - 'A' + key) % 26;
        }
        string[i] = ch;
    }
    return string;
}

char *ceasarDecrypt(int key, char *string) {
    return ceasarEncrypt(26 - key, string); // Decrypting is just encrypting with the inverse key
}

char *fgetstr(char *string, int n, FILE *stream) {
    char *result = fgets(string, n, stream);
    if (!result) {
        return result;
    }
    if (string[strlen(string) - 1] == '\n') {
        string[strlen(string) - 1] = '\0';
    }
    return string;
}

