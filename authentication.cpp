//
// Created by sams on 11/06/2023.
//

#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <sstream>
#include <sys/socket.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <cctype>
#include "base64.h"
#include "authentication.h"
#include "utils.h"

// Function to check if the provided username and password are valid
int authenticate(const std::string& username, const std::string& password) {
    if (username == "admin" && password == "admin") {
        return 1; // Successful login
    } else {
        return 0; // Login failed
    }
}

void send_basic_auth_prompt(int client_socket) {
    const char* header = "HTTP/1.1 401 Unauthorized\r\n"
                         "WWW-Authenticate: Basic realm=\"Restricted\"\r\n"
                         "Content-Length: 0\r\n"
                         "Custom-Header: SomeValue\r\n"
                         "\r\n";

    if (send(client_socket, header, strlen(header), 0) < 0) {
        perror("Failed to send response header");
        return;
    }
}


// Function to perform authentication for a given file path
int perform_authentication(int client_socket, const std::string& file_path, const std::string& original_request) {
    std::string request = original_request;

    size_t filename_pos = file_path.find_last_of('/');
    std::string filename = (filename_pos != std::string::npos) ? file_path.substr(filename_pos + 1) : file_path;

    // Extract and print query parameters
    std::map<std::string, std::string> query_params = extract_query_parameters(original_request);
    if (!query_params.empty()) {
        std::cout << "Query parameters:\n";
        for (const auto& [key, value] : query_params) {
            std::cout << key << " = " << value << "\n";
        }
    }

    if (filename.find("test/echo.php") != std::string::npos || filename.find("g.php") != std::string::npos) {
        std::string authorization_header = extract_header_value(request, "Authorization:");

        if (authorization_header.empty()) {
            send_basic_auth_prompt(client_socket);
            return 0;
        }

        std::string username;
        std::string password;

        if (!extract_username_password(authorization_header, username, password)) {
            send_basic_auth_prompt(client_socket);
            return 0;
        }

        if (authenticate(username, password)) {
            return 1;
        } else {
            send_basic_auth_prompt(client_socket);
            return 0;
        }
    }

    return 1;
}
