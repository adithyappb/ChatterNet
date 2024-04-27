#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "utilities.h"
#include <termios.h>

#define SERVER_PORT 1337
#define MAX_LINE 1024
#define CODE_NOT_FOUND "401"
#define EXIT_COMMAND "q"
#define EXIT_MESSAGE "Client has exited. Chat terminated."

// Function to hide user input for password
void hidePassword(char *password) {
    struct termios term, oldterm;
    tcgetattr(STDIN_FILENO, &oldterm);
    term = oldterm;
    term.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    printf("\nEnter your password: ");
    fflush(stdout);
    fgets(password, MAX_LINE, stdin);
    password[strlen(password) - 1] = '\0'; // Removing newline character
    tcsetattr(STDIN_FILENO, TCSANOW, &oldterm);
    printf("\n");
}

// Function to get username and password separately
void getUserCredentials(char *username, char *password) {
    printf("Enter your username: ");
    fgetstr(username, MAX_LINE, stdin);
    hidePassword(password);
}

int main(int argc, char *argv[]) {
    int sockfd, authenticated = 0, userChoice = 0;
    char code[MAX_LINE] = {0}, username[MAX_LINE] = {0}, password[MAX_LINE] = {0};
    struct sockaddr_in serverAddr;

    printf("Date: %s\n", __DATE__);
    printf("Time: %s\n", __TIME__);

    if (argc != 2) {
        printf("Usage: %s <server_ip>\n", argv[0]);
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    do {
        getUserCredentials(username, password);

        send(sockfd, username, sizeof(username), 0);
        send(sockfd, password, sizeof(password), 0);

        printf("[*] Sending authentication information\n");
        read(sockfd, code, MAX_LINE);

        if (strcmp(code, CODE_NOT_FOUND) == 0) {
            memset(username, 0, sizeof(username));
            memset(password, 0, sizeof(password));
            printf("User not found. Do you want to create an account? (Yes: 1 / No: Any other key): ");
            scanf("%d", &userChoice);
            fflush(stdin);

            if (userChoice == 1) {
                printf("Creating an account...\n");
                getUserCredentials(username, password);

                send(sockfd, username, sizeof(username), 0);
                send(sockfd, password, sizeof(password), 0);
                printf("**** Account created successfully ****\n");
            } else {
                printf("Exiting...\n");
                send(sockfd, EXIT_COMMAND, sizeof(EXIT_COMMAND), 0); // Send exit command to server
                exit(1);
            }
        }
        authenticated = 1;
    } while (authenticated == 0);

    printf("[*] Press 'q' to exit the chat\n");
    printf("[*] Welcome to Lamb Chat!!!\n\n");

    // Main loop for the client to chat
    while (1) {
        char userInput[MAX_LINE] = {0}, serverResponse[MAX_LINE] = {0};
        printf("%s > ", username);
        fgetstr(userInput, sizeof(userInput), stdin);

        send(sockfd, userInput, sizeof(userInput), 0);
        if (strcmp(userInput, EXIT_COMMAND) == 0) {
            send(sockfd, EXIT_COMMAND, sizeof(EXIT_COMMAND), 0); // Send exit command to server
            printf("%s\n", EXIT_MESSAGE);
            break;
        }
        
        printf("[*] Waiting for server response\n");
        read(sockfd, serverResponse, MAX_LINE);
        printf("Server > %s\n", serverResponse);
        if (strcmp(serverResponse, EXIT_COMMAND) == 0) {
            printf("Server has exited. Chat terminated.\n");
            send(sockfd, EXIT_COMMAND, sizeof(EXIT_COMMAND), 0); // Send exit command to server
            break;
        }
    }

    printf("Goodbye!\n");
    close(sockfd);
    return 0;
}

