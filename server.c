#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// socket headers
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define byte uint8_t
#define MAX_SIZE 1024
#define PORT 8080

void setzero(void *ptr, size_t size) {
    for (int i = 0; i < size; i++)
        ((byte *)ptr)[i] = 0;
}

void cleanup(int n, ...) {
    va_list args;
    va_start(args, n);
    for (int i = 0; i < n; i++)
        free(va_arg(args, void *));
    va_end(args);
}

int main(void) {
    printf("Hello, world!\n");
    int listenfd, connectionfd;  // note fd means file descriptor
    int status;                  // error status
    struct sockaddr_in server_socket;
    // allocate the send buffer and response buffer on the heap
    byte *sendbuf = malloc(MAX_SIZE);
    byte *resposebuf = malloc(MAX_SIZE);

    // create the socket
    status = socket(AF_INET, SOCK_STREAM, 0);
    if (status < 0) {  // negative numbers mean errors
        perror("Error with creating socket");
        cleanup(2, sendbuf, resposebuf);
        exit(1);
    } else
        listenfd = status;  // if we were successful, the file descriptor is returned

    // setup address stuff
    // set the struct to 0
    setzero(&server_socket, sizeof(server_socket));
    server_socket.sin_family = AF_INET;                 // set the family to addresses from the internet
    server_socket.sin_addr.s_addr = htonl(INADDR_ANY);  // set the server input address to any address
    server_socket.sin_port = htons(PORT);               // set the port to PORT

    // bind and listen to the socket
    status = bind(listenfd, (struct sockaddr *)&server_socket, sizeof(server_socket));
    if (status < 0) {
        perror("Error with binding to the socket");
        cleanup(2, sendbuf, resposebuf);
        exit(1);
    }

    status = listen(listenfd, 2);  // 2 is the max number of connections
    if (status < 0) {
        perror("Error with listening to the socket");
        cleanup(2, sendbuf, resposebuf);
        exit(1);
    }

    for (;;) {
        struct sockaddr_in addr;
        socklen_t addr_len = sizeof(addr);

        printf("Waiting for connection on port %d...\n", PORT);
        fflush(stdout);
        connectionfd = accept(listenfd, NULL, NULL);  // null, null accepts any connection

        setzero(resposebuf, MAX_SIZE);
        // read the data from the socket
        char c;
        while ((c = read(connectionfd, resposebuf, MAX_SIZE - 1)) > 0) {
            if (resposebuf[c - 1] == '\n')
                break;
        }
        if (c < 0) {
            perror("Error reading from socket");
            cleanup(2, sendbuf, resposebuf);
            exit(1);
        }

        printf("Received: %s\n", resposebuf);
        setzero(resposebuf, MAX_SIZE);

        // respond
        printf("Sending response...\n");
        snprintf(resposebuf, MAX_SIZE, "HTTP/1.1 200 OK\r\n\r\n<h1>Hello, World!</h1>");
        write(connectionfd, resposebuf, MAX_SIZE);
        close(connectionfd);
    }

    cleanup(2, sendbuf, resposebuf);
    return 0;
}