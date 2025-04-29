#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dirent.h>
#include "authentication.h"
#include "mime_type_handler.h"
#include "request_logger.h"
#define MAX_REQUEST_SIZE 1024

#include "error_handling.h"

char SERVER_DIR[200];

void send_authentication_required_response(int client_socket, const char* file_path, const char* request) {
    std::cout << "Requested filepath: " << file_path << std::endl;

    FILE* file = fopen(file_path, "r");
    if (file == nullptr) {
        perror("Failed to open file");
        send_error_response(client_socket, 404, "Not Found", file_path);
        return;
    }

    std::cout << "Serving file: " << file_path << std::endl;

    if (check_php_file(file_path)) {
        char response_header[200];
        snprintf(response_header, sizeof(response_header), "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
        handle_php_file(file, &client_socket, response_header);
    } else {
        const char* content_type = get_content_type(file_path);

        char response_header[200];
        sprintf(response_header, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n", content_type);
        log_request_response(request, response_header);

        if (send(client_socket, response_header, strlen(response_header), 0) < 0) {
            perror("Failed to send response header");
            fclose(file);
            return;
        }

        char file_buffer[1024];
        size_t bytes_read;
        while ((bytes_read = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0) {
            if (send(client_socket, file_buffer, bytes_read, 0) < 0) {
                perror("Failed to send file");
                fclose(file);
                return;
            }
        }

        fclose(file);
    }

    close(client_socket);
}

void handle_request(int client_socket, const char* request) {
    char* request_copy = strdup(request);
    char* path_start = strstr(request_copy, "GET /");
    if (path_start == nullptr) {
        perror("Invalid request");
        close(client_socket);
        free(request_copy);
        return;
    }

    char* path_end = strstr(path_start, " HTTP/");
    if (path_end == nullptr) {
        perror("Invalid request");
        close(client_socket);
        free(request_copy);
        return;
    }

    *path_end = '\0';
    char* path = path_start + 5;

    char file_path[200];
    strcpy(file_path, SERVER_DIR);
    strcat(file_path, path);

    int authentication_result = perform_authentication(client_socket, file_path, request);
    free(request_copy);

    if (authentication_result == 0) {
        close(client_socket);
        std::cout << "Auth Failed" << std::endl;
        return;
    }

    std::cout << "perform_authentication completed" << std::endl;
    send_authentication_required_response(client_socket, file_path, request);
    close(client_socket);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " SERVER_DIR PORT" << std::endl;
        std::cout << "Please specify server directory and port number" << std::endl;
        return 1;
    }

    strcpy(SERVER_DIR, argv[1]);
    int port = atoi(argv[2]);

    int server_socket, client_socket;
    struct sockaddr_in server_address{}, client_address{};
    socklen_t client_address_len = sizeof(client_address);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Failed to create socket");
        return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Failed to bind socket");
        return 1;
    }

    if (listen(server_socket, 10) < 0) {
        perror("Failed to listen for connections");
        return 1;
    }

    std::cout << "Server started on port " << port << std::endl;

    while (true) {
        std::cout << "Waiting for request..." << std::endl;
        client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
        if (client_socket < 0) {
            perror("Failed to accept connection");
            continue;
        }

        char request[MAX_REQUEST_SIZE];
        memset(request, 0, sizeof(request));

        int recv_result = recv(client_socket, request, sizeof(request), 0);
        if (recv_result == 0) {
            std::cout << "Client closed the connection." << std::endl;
            close(client_socket);
            continue;
        } else if (recv_result < 0) {
            perror("Failed to receive request");
            close(client_socket);
            continue;
        }

        handle_request(client_socket, request);
    }

    close(server_socket);
    return 0;
}
