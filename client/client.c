#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

FILE *file;
#define MAX_SEND 256

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char *buffer;
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    unsigned int file_length;
    if ((file = fopen("image.jpg", "rb")) == NULL) {
        printf("Error! Can not read file!\n");
        exit(1);
    } else {
        fseek(file, 0, SEEK_END);
        file_length = ftell(file);
        buffer = calloc(file_length, sizeof(char));
        unsigned int i = 0x0;
        fseek(file, 0, SEEK_SET);
        while(!feof(file) && i<file_length)
            buffer[i++] = fgetc(file);
        buffer[i] = 0;
        fclose(file);
    }
    unsigned int i;
    char *buf = calloc(MAX_SEND, sizeof(char));
    buf[0x0] = 0x5A;
    buf[0x1] = 0x0;
    char read_buf = 0x0;
    for (i = 0x0; i < file_length/(MAX_SEND - 2); i++) {
        memcpy((void*)(buf + 2), (void*)(buffer + i*(MAX_SEND - 2)), (MAX_SEND - 2));
        if ( ( i == (file_length/(MAX_SEND - 2)) ) && ( ( (file_length%(MAX_SEND - 2)) == 0 ) )) {
            buf[0x1] = 0xFF;
        }
        do {
            read_buf = 0x0;
            n = write(sockfd, buf, MAX_SEND);
            if (n < 0)
                error("ERROR writing to socket");
            n = read(sockfd, (void*)&read_buf, 1);
            if (n < 0)
                error("ERROR reading from socket");
        } while (read_buf != 0x67);
    }
    if ( (file_length%(MAX_SEND - 2)) > 0 ) {
        memcpy((void*)(buf + 2), (void*)(buffer + i*(MAX_SEND - 2)), (file_length%(MAX_SEND - 2)));
        buf[0x1] = 0xFF;
        do {
            read_buf = 0x0;
            n = write(sockfd, buf, 2 + (file_length%(MAX_SEND - 2)));
            if (n < 0)
                error("ERROR writing to socket");
            n = read(sockfd, (void*)&read_buf, 1);
            if (n < 0)
                error("ERROR reading from socket");
        } while (read_buf != 0x67);
    }
    close(sockfd);
    free(buffer);
    free(buf);
    return 0;
}
