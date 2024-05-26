### ChatterNet: Lightweight Chat Application in C

This is a simple chat application implemented in C using socket programming. It consists of a server and a client.

### How to Compile-

## Server:
gcc serverChat.c utilities.c -o serverChat

## Client:
gcc clientChat.c utilities.c -o clientChat


### How to Run-

## Server: 
./serverChat

## Client:
./clientChat <server_ip>

Replace <server_ip> with the IP address of the server.

Usage:
-> Run the server application.
-> Run the client application and enter username/password.
-> Start chatting with other clients.
-> Type 'q' to exit the chat.

Features:
-> User authentication
-> Command-driven interface
-> Support for multiple clients
-> Real-time messaging

