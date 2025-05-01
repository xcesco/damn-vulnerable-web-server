//
// Created by sams on 11/06/2023.
//

#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <cctype>
#include "base64.h"
#include "authentication.h"

std::string extract_header_value(const std::string& request, const std::string& header_name) {
    size_t header_name_length = header_name.length();
    size_t header_pos = request.find(header_name);

    if (header_pos == std::string::npos) {
        std::cout << "Header not found: " << header_name << std::endl;
        return "";
    }

    size_t value_pos = header_pos + header_name_length;
    while (isspace(request[value_pos])) {
        value_pos++;
    }

    return request.substr(value_pos);
}

// Function to check if the provided username and password are valid
int authenticate(const std::string& username, const std::string& password) {
    // Check if the provided username and password are valid for login
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
                         "Custom-Header: SomeValue\r\n" // Add a custom header
                         "\r\n";

    if (send(client_socket, header, strlen(header), 0) < 0) {
        perror("Failed to send response header");
        return; // Return if failed to send
    }
}

bool extract_username_password(const std::string& authorization_header, std::string& username, std::string& password) {
    // Move the pointer past "Basic "
    size_t prefix_length = strlen("Basic ");
    std::string encoded_credentials = authorization_header.substr(prefix_length);

    // Decode the base64-encoded credentials
    size_t decode_length = 0;
    unsigned char* decoded_credentials = base64_decode(encoded_credentials.c_str(), encoded_credentials.length(), &decode_length);

    if (decoded_credentials == nullptr || decode_length == 0) {
        perror("Failed to decode credentials");
        return false;
    }

    // Null-terminate the decoded credentials
    decoded_credentials[decode_length] = '\0';

    // Extract the username and password from the decoded credentials
    char* colon = strchr((char*)decoded_credentials, ':');
    if (colon == nullptr) {
        perror("Invalid credentials format");
        free(decoded_credentials);
        return false;
    }

    // Null-terminate the username and copy it to the username variable
    *colon = '\0';
    username = std::string(reinterpret_cast<char*>(decoded_credentials));


    // Copy the password to the password variable, excluding the ':' character
    password = colon + 1;

    // Cleanup the decoded credentials
    free(decoded_credentials);

    return true;
}

// Function to perform authentication for a given file path
int perform_authentication(int client_socket, const std::string& file_path, const std::string& original_request) {
    std::string request = original_request;

    size_t filename_pos = file_path.find_last_of('/');
    std::string filename = (filename_pos != std::string::npos) ? file_path.substr(filename_pos + 1) : file_path;

    // Check if the requested file path requires authentication
    if (filename.find("test/echo.php") != std::string::npos || filename.find("g.php") != std::string::npos) {
        // Extract the "Authorization" header value
        std::string authorization_header = extract_header_value(request, "Authorization:");

        if (authorization_header.empty()) {
            // Send authentication prompt
            send_basic_auth_prompt(client_socket);
            return 0; // Authentication failed
        }

        std::string username;
        std::string password;

        if (!extract_username_password(authorization_header, username, password)) {
            // Send authentication prompt
            send_basic_auth_prompt(client_socket);
            return 0; // Authentication failed
        }

        if (authenticate(username, password)) {
            return 1; // Authentication successful
        } else {
            // Send authentication prompt
            send_basic_auth_prompt(client_socket);
            return 0; // Authentication failed
        }
    }

    return 1; // No authentication required for other files
}
