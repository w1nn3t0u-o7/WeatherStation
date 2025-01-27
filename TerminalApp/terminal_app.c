#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXLINE 1024
#define PORT 12345

int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_in servaddr;
    char recvline[MAXLINE + 1];
    int n;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Server IP>\n", argv[0]);
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT); /* Port number should match the server */

    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        fprintf(stderr, "inet_pton error for %s\n", argv[1]);
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        fprintf(stderr, "connect error: %s\n", strerror(errno));
        exit(1);
    }

    while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0; /* Null terminate the string */
        if (fputs(recvline, stdout) == EOF) {
            fprintf(stderr, "fputs error\n");
            exit(1);
        }
    }

    if (n < 0) {
        fprintf(stderr, "read error: %s\n", strerror(errno));
        exit(1);
    }

    close(sockfd);
    return 0;
}