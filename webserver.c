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

int clients[CONNECTIONS];

int server_init(int *listen_sock);
void handle_client_request(int SLOT);
void check_for_parameters(char *route);
void handle_post_request(int client_socket, char *buffer,   int bytes_received);
void handle_get_request(int client_sock, char *file_name);
void serve_error_file(int client_sock);
void error(int client_sock);

int main() {
    int listen_sock = -1;
    
    if (server_init(&listen_sock) == -1){
      return 0;
    }
    
    printf("Server running on:  http://127.0.0.1:%d\n",PORT );

    int i;
    int SLOT = 0;
    socklen_t ADDRLen;

    for (i = 0; i < CONNECTIONS; i++)
        clients[i] = -1;

    while (1) {
        struct sockaddr_in clientaddr;
        socklen_t clientaddr_len = sizeof(clientaddr);

        clients[SLOT] = accept(listen_sock, (struct sockaddr*)&clientaddr, &clientaddr_len); // accept connection and read data
        if ( clients[SLOT] < 0) {
            fprintf(stderr, "Failed to accept.\n");
            continue;
        }
        else if (fork() == 0){
            printf("---- client connected ----\n");
            printf("Client Number: %d \n", SLOT);
            handle_client_request(SLOT);
            continue;
        }

        while (clients[SLOT] != -1){
          SLOT = (SLOT + 1) % CONNECTIONS;
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

//To handle all types of client requests
void handle_client_request(int SLOT){
    char buffer[SIZE]; 
    int is_read = read(clients[SLOT], buffer, SIZE);

    if (is_read < 0) {
        error(clients[SLOT]);
        return;
    } 

    char method[10], endpoint[100], *post_data;
    sscanf(buffer, "%s %s", method, endpoint);
    printf("Request Type: %s \nEndpoint:%s \n\n", method, endpoint);

    check_for_parameters(endpoint);

    if(strcmp(method, "POST") == 0) {
        if (strcmp(endpoint, "/api/submitForm") == 0) {
          handle_post_request(clients[SLOT], buffer, is_read);
        } 
        else if (strcmp(endpoint, "/api/update") == 0) {
          handle_post_request(clients[SLOT], buffer, is_read);
        } 
        else {
          serve_error_file(clients[SLOT]);
        } 
  }
    
  else if (strcmp(method, "GET") == 0) {
      if (strcmp(endpoint, "/api/getAll") == 0) {
        handle_get_request(clients[SLOT], endpoint);
      } 
      else if (strcmp(endpoint, "/api/getNames") == 0) {
        handle_get_request(clients[SLOT], endpoint);
      } 
      else if (strcmp(endpoint, "/api/getUsers") == 0) {
        handle_get_request(clients[SLOT], endpoint);
      } 
      else if (strcmp(endpoint, "/api/getImage") == 0) {
        handle_get_request(clients[SLOT], endpoint);
      } 
      else if (strcmp(endpoint, "/api/getText") == 0) {
        handle_get_request(clients[SLOT], endpoint);
      } 
      else if (strcmp(endpoint, "/api/getPDF") == 0) {
        handle_get_request(clients[SLOT], endpoint);
      } 
      else if (strcmp(endpoint, "/") == 0) {
        handle_get_request(clients[SLOT], endpoint);
      } 
      else {
        serve_error_file(clients[SLOT]);
      } 
  } 

  else {
      serve_error_file(clients[SLOT]);
  }
  close(clients[SLOT]);
}

//To check the parameter in the request endpoint
void check_for_parameters(char *route){
  char *is_param = strrchr(route, '?');
  if (is_param){
    *is_param = '\0';
  }
}

//To send the 404 error message to the client
void error(int client_sock) {
  char response[SIZE];
  sprintf(response, "HTTP/1.1 505 Internal Server Error %s\r\nContent-Type: text/html\r\n\n");
  send(client_sock, response, strlen(response), 0);
}

//To serve the 404 file 
void serve_error_file(int client_sock) {
  FILE *fp = fopen("common/error.html", "r");

  fseek(fp, 0, SEEK_END);
  long fsize = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char* response_data = malloc(sizeof(char) * (fsize+1));
  char ch;
  int i = 0;
  while((ch = fgetc(fp)) != EOF) {
      response_data[i] = ch;
      i++;
  }
  fclose(fp);

  char http_header[4096] = "HTTP/1.1 404 Not Found\r\n\r\n";
  strcat(http_header, response_data);
  strcat(http_header, "\r\n\r\n");

  send(client_sock, http_header, sizeof(http_header), 0);
  close(client_sock);
  free(response_data);
}

//To serve the GET Requests
void handle_get_request(int client_sock, char *route) {
  char file_path[100];
  int is_index_file = -1;
  char *endpoint_token;

  if (strcmp(route, "/") == 0 ){
    strcpy(file_path, "GET_api/index.html");
    is_index_file = 1;
  }
  else{
    endpoint_token = strtok(route, "/"); 
    endpoint_token = strtok(NULL, "/");
    strcpy(file_path, "GET_api/");
    strcat(file_path, endpoint_token);
    is_index_file = -1;
  }

  if(is_index_file < 0 && strcmp(endpoint_token, "getImage") == 0){
    FILE *fp = fopen("public/sample.jpg", "rb");
    if (fp == NULL) {
      printf("Could not open file \n");
      serve_error_file(client_sock);
      return;
    }

    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char http_header[8192];
    sprintf(http_header,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: image/jpeg\r\n"
            "Content-Length: %d\r\n\r\n",
            file_size);

    write(client_sock, http_header, strlen(http_header));

    char buffer[8192];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
      write(client_sock, buffer, bytesRead);
    }

    fclose(fp);
  }
  else if(is_index_file < 0 && strcmp(endpoint_token, "getPDF") == 0){
    FILE *fp = fopen("public/sample.pdf", "rb");
    if (fp == NULL) {
      printf("Could not open file \n");
      serve_error_file(client_sock);
      return;
    }

    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char http_header[8192];
    sprintf(http_header,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/pdf\r\n"
            "Content-Length: %d\r\n\r\n",
            file_size);

    write(client_sock, http_header, strlen(http_header));

    char buffer[8192];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
      write(client_sock, buffer, bytesRead);
    }
    fclose(fp);
  }
  else if(is_index_file < 0 && strcmp(endpoint_token, "getText") == 0){
    FILE *fp = fopen("public/sample.txt", "rb");
    if (fp == NULL) {
      printf("Could not open file \n");
      serve_error_file(client_sock);
      return;
    }

    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char http_header[8192];
    sprintf(http_header,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: %d\r\n\r\n",
            file_size);

    write(client_sock, http_header, strlen(http_header));

    char buffer[8192];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
      write(client_sock, buffer, bytesRead);
    }
    fclose(fp);
  }
  else{
    const char *is_dot = strrchr(file_path, '.');
    if (!is_dot || is_dot == file_path){
      strcat(file_path, ".html");
    }

    FILE *fp = fopen(file_path, "r");

    if (fp == NULL) {
      printf("Could not open file \n");
      serve_error_file(client_sock);
      return;
    }

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* response_data = malloc(sizeof(char) * (fsize+1));
    char ch;
    int i = 0;
    while((ch = fgetc(fp)) != EOF) {
        response_data[i] = ch;
        i++;
    }

    fclose(fp);

    char http_header[4096] = " ";
    sprintf(http_header,
          "HTTP/1.1 200 OK\r\n"
          "Content-Type: text/html\r\n\n");

    strcat(http_header, response_data);
    strcat(http_header, "\r\n\r\n");

    send(client_sock, http_header, sizeof(http_header), 0);
    free(response_data);
  }
  close(client_sock);
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