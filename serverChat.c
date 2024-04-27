#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "utilities.h"

#define SERVER_PORT 1337
#define MAX_LINE 1024
#define CODE_FOUND "1"
#define CODE_NOT_FOUND "401"
#define DATABASE_FILE "database.txt"
#define EXIT_COMMAND "q"
#define EXIT_MESSAGE "Server has exited. Chat terminated."

struct User {
    char name[MAX_LINE];
    char password[MAX_LINE];
};

void saveUserToFile(struct User user) {
    FILE *file = fopen(DATABASE_FILE, "a");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    } else {
        fprintf(file, "user:%s\npassword:%s\n", user.name, user.password);
        printf("New user added to the database: %s\n", user.name);
        fclose(file);
    }
}

int searchDatabase(struct User user) {
    FILE *file = fopen(DATABASE_FILE, "r");
    struct User temp;

    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    } else {
        printf("[*] Searching database...\n");
        while (fscanf(file, "user:%s\npassword:%s\n", temp.name, temp.password) != EOF) {
            if ((strcmp(user.name, temp.name) == 0) && (strcmp(user.password, temp.password) == 0)) {
                printf("[*] User found in database.\n");
                fclose(file);
                return 1;
            }
        }
    }
    fclose(file);
    return 401;
}

int isDatabaseExist() {
    FILE *file = fopen(DATABASE_FILE, "r");
    if (file == NULL) {
        return 0;
    }
    fclose(file);
    return -1;
}

void handleClient(int connfd) {
    struct User user;
    read(connfd, user.name, MAX_LINE);
    read(connfd, user.password, MAX_LINE);

    if (strcmp(user.name, EXIT_COMMAND) == 0 || strcmp(user.password, EXIT_COMMAND) == 0) {
        printf("Client has exited. Chat terminated.\n");
        close(connfd);
        exit(0);
    }

    if (searchDatabase(user) == 401) {
        memset(&user, 0, sizeof(user));
        printf("New registration.\n");
        send(connfd, CODE_NOT_FOUND, sizeof(CODE_NOT_FOUND), 0);
        read(connfd, user.name, MAX_LINE);
        read(connfd, user.password, MAX_LINE);
        saveUserToFile(user);
        printf("%s has joined.\n", user.name);
    } else {
        send(connfd, CODE_FOUND, sizeof(CODE_FOUND), 0);
        printf("%s has logged in.\n", user.name);
    }

    while (1) {
        char clientSend[MAX_LINE] = {0}, serverResponse[MAX_LINE] = {0};
        read(connfd, clientSend, MAX_LINE);
        printf("%s: %s\n", user.name, clientSend);

        if (strcmp(clientSend, EXIT_COMMAND) == 0) {
            printf("Client has exited. Chat terminated.\n");
            close(connfd);
            exit(0);
        }

        printf("> ");
        fgetstr(serverResponse, sizeof(serverResponse), stdin);
        send(connfd, serverResponse, sizeof(serverResponse), 0);
        printf("[*] Waiting for client response\n");

        if (strcmp(serverResponse, EXIT_COMMAND) == 0) {
            printf("Server has exited. Chat terminated.\n");
            close(connfd);
            exit(0);
        }
    }
}

int main(void) {
    int listenfd, connfd;
    struct sockaddr_in serverAddr;

    printf("Date: %s\n", __DATE__);
    printf("Time: %s\n", __TIME__);
    printf("[*] Press '^C' to exit the chat\n");

    if (isDatabaseExist() == 0) {
        initializeDatabase();
    }

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listenfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    printf("[*] Bind successful\n");
    printf("[*] Server up and running\n");

    if (listen(listenfd, BACKLOG) == 0) {
        printf("[*] Server is listening ...\n");
    }

    while (1) {
        connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);
        printf("[*] Connection accepted\n");

        if (fork() == 0) {
            close(listenfd);
            handleClient(connfd);
            exit(0);
        }

        close(connfd);
    }

    return 0;
}


