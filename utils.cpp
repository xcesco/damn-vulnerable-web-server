#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <sstream>
#include <cctype>
#include "base64.h"

// Extract a header's value from the HTTP request string
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


// Extract query parameters from the request line (e.g., GET /path?param=value HTTP/1.1)
std::map<std::string, std::string> extract_query_parameters(const std::string& request) {
    std::map<std::string, std::string> query_params;

    size_t start = request.find(" ");
    size_t end = request.find(" ", start + 1);
    if (start == std::string::npos || end == std::string::npos) {
        return query_params;
    }

    std::string path_with_query = request.substr(start + 1, end - start - 1);
    size_t query_pos = path_with_query.find("?");

    if (query_pos == std::string::npos) {
        return query_params;
    }

    std::string query_string = path_with_query.substr(query_pos + 1);
    std::stringstream ss(query_string);
    std::string pair;

    while (std::getline(ss, pair, '&')) {
        size_t eq_pos = pair.find('=');
        if (eq_pos != std::string::npos) {
            std::string key = pair.substr(0, eq_pos);
            std::string value = pair.substr(eq_pos + 1);
            query_params[key] = value;
        }
    }

    return query_params;
}
// Extract the username and password from the Authorization header
bool extract_username_password(const std::string& authorization_header, std::string& username, std::string& password) {
    size_t prefix_length = strlen("Basic ");
    std::string encoded_credentials = authorization_header.substr(prefix_length);

    size_t decode_length = 0;
    unsigned char* decoded_credentials = base64_decode(encoded_credentials.c_str(), encoded_credentials.length(), &decode_length);

    if (decoded_credentials == nullptr || decode_length == 0) {
        perror("Failed to decode credentials");
        return false;
    }

    decoded_credentials[decode_length] = '\0';
    char* colon = strchr((char*)decoded_credentials, ':');
    if (colon == nullptr) {
        perror("Invalid credentials format");
        free(decoded_credentials);
        return false;
    }

    *colon = '\0';
    username = std::string(reinterpret_cast<char*>(decoded_credentials));
    password = colon + 1;
    free(decoded_credentials);

    return true;
}
