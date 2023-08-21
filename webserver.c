#include <stdio.h>
#include <stdlib.h> 
#include <string.h>  
#include <netdb.h>  
#include <sys/socket.h>  
#include <netinet/in.h> 
#include <unistd.h>  

#define PORT 8080
#define MAX_PENDING 5  
#define SIZE 1024  
#define CONNECTIONS 1000


int server_init(int *listen_sock);
void handle_client_request(int SLOT);

int main() {
    int listen_sock = -1;
    
    if (server_init(&listen_sock) == -1){
      return 0;
    }
    
    printf("Server running on:  http://127.0.0.1:%d\n",PORT );

    while (1) {
        struct sockaddr_in clientaddr;
        socklen_t clientaddr_len = sizeof(clientaddr);

        int client = accept(listen_sock, (struct sockaddr*)&clientaddr, &clientaddr_len); // accept connection and read data
        if ( client< 0) {
            fprintf(stderr, "Failed to accept.\n");
            continue;
        }
        else{
            printf("---- client connected ----\n");
            handle_client_request(client);
            continue;
        }

    }
    close(listen_sock);
    return 0;
}