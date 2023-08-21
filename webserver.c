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
void handle_post_request(int client_socket, char *buffer, int bytes_received);

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

//To initiate the server
int server_init(int *listen_sock){
    struct sockaddr_in servaddr; 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PORT);

    *listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(*listen_sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));

    int is_bind = bind(*listen_sock, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if ( is_bind < 0) {
        fprintf(stderr, "Failed to bind.\n");
        close(*listen_sock);
        return -1;
    }

    int is_listening = listen(*listen_sock, MAX_PENDING);
    if ( is_listening < 0) {
        fprintf(stderr, "Failed to listen.\n");
        close(*listen_sock);
        return -1;
    }
    return 0; 
}

//To serve the POST Requests
void handle_post_request(int client_sock, char *buffer, int bytes_received) {
   char *value = malloc(sizeof(char) * bytes_received);
  value = buffer;

  printf("-------- Request Body -------- \n");
  for (int i = 0; i < bytes_received; i++) {
    value = buffer + i;
    printf("%c", *value);
  }

  char response[1024];
  char *content = "HTTP/1.1 200 OK \n";
  
  sprintf(response,
          "HTTP/1.1 200 OK\r\n"
          "Content-Type: text/html\r\n"
          "Content-Length: %d\r\n"
          "\r\n\n",
          strlen(content));

  send(client_sock, response, strlen(response), 0);
  send(client_sock, content, strlen(content), 0);
  close(client_sock);
  free(value);
}