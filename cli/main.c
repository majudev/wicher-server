#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/un.h>
#include <stdbool.h>
#include <unistd.h>

#define SERVER_PATH "/tmp/wicher-server-socket"

void error(const char * msg){
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("wicher-server-cli v1.0 (c) majudev.net 2017\n");
        if(argc > 3){
            printf("Usage: %s [socket-file] [command]\n", argv[0]);
            return 0;
        }else printf("\n");
    }
    int sockfd, n;
    struct sockaddr_un serv_addr;
    
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    
    if(argc >= 2){
        strcpy(serv_addr.sun_path, argv[1]);
    }else strcpy(serv_addr.sun_path, SERVER_PATH);
	
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    char buffer[1024];
    if(argc == 3){
        write(sockfd, argv[2], strlen(argv[2]) + 1);
        
        n = read(sockfd, buffer, 1024);
        if(n < 0) error("ERROR receiving message");
        printf("%s\n", buffer);
        close(sockfd);
        return 0;
    }
    
    size_t pos = 0;
    bool running = true;
    while(running){
        printf("> ");
        while(pos < 1024){
            buffer[pos] = getchar();
            if(buffer[pos] == '\n') break;
            ++pos;
        }
        buffer[pos] = '\0';
        n = write(sockfd, buffer, pos);
        pos = 0;
        if(!strcmp(buffer, "HALT") || !strcmp(buffer, "BYE")) running = false;
        
        n = read(sockfd, buffer, 1024);
        if(n <= 0) error("ERROR receiving message");
        printf("  %s\n", buffer);
        if(!strcmp(buffer, "Unknown command")) running = false;
    }
    close(sockfd);
    return 0;
}