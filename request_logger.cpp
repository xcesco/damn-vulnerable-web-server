#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include "authentication.h" // Assuming extract_query_parameters is defined here
#include <sys/socket.h>
#include <cstring>       // for strerror()
#include <sstream>       // for stringstream
#include "utils.h"
 
void log_request_response(const std::string& request, const std::string& response) {
    // Get current time
    std::time_t current_time = std::time(nullptr);
    std::tm* time_info = std::localtime(&current_time);

    // Format timestamp
    char timestamp[20];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", time_info);

    // Extract query parameters from the request
    std::map<std::string, std::string> query_params = extract_query_parameters(request);
    // std::cerr << "Logging request and response..." << std::endl;

    // Log to file
    FILE* log_file = fopen("/tmp/server.log", "a");
    if (!log_file) {
        std::cerr << "Failed to open log file" << std::endl;
        return;
    }

    // Logging request data
    fprintf(log_file, "[%s] Request:\n", timestamp);
    for (const auto& [key, value] : query_params) {
        fprintf(log_file, "Key: %s, Value: ", key.c_str());
        fprintf(log_file, value.c_str());  // format string issue
        fprintf(log_file, "\n");
    }

    // Logging response data
    fprintf(log_file, "[%s] Response:\n", timestamp);
    fprintf(log_file, "%s\n", response.c_str());

    fclose(log_file);
}

// Log Viewer with Command Injection Vulnerability
void handle_log_viewer(int client_socket, const std::string& request) {
    // Extract query parameters
    auto query_params = extract_query_parameters(request);
    std::string filter = query_params["filter"];  // User-controlled filter parameter

    // URL-decode the filter string
    std::string decoded_filter = url_decode(filter);

    // Construct the command to execute. This will allow the attacker to inject commands
    std::string command = "sh -c \"grep " + decoded_filter + " /tmp/server.log;\"";

    // Debug: Print the command to check if it looks correct
    // std::cout << "Command: " << command << std::endl;

    // Execute the command using popen
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::string error_msg = "HTTP/1.1 500 Internal Server Error\r\n\r\nFailed to read logs.\n";
        send(client_socket, error_msg.c_str(), error_msg.length(), 0);
        return;
    }

    char buffer[256];
    std::stringstream result_stream;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result_stream << buffer;
    }

    pclose(pipe);
    std::string result = result_stream.str();

    // Debug: Print the result of the command execution
    std::cout << "Command output: " << result << std::endl;

    std::string response;
    if (result.empty()) {
        response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nNo matching log entries found.\n";
    } else {
        response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n" + result;
    }

    send(client_socket, response.c_str(), response.length(), 0);
}
