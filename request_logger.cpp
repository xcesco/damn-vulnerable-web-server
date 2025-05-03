#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include "authentication.h" // Assuming extract_query_parameters is defined here

void log_request_response(const std::string& request, const std::string& response) {
    // Get current time
    std::time_t current_time = std::time(nullptr);
    std::tm* time_info = std::localtime(&current_time);

    // Format timestamp
    char timestamp[20];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", time_info);

    // Extract query parameters from the request
    std::map<std::string, std::string> query_params = extract_query_parameters(request);
    std::cerr << "Logging request and response..." << std::endl;

    // Open the log file in append mode using FILE* for fprintf
    FILE* log_file = fopen("/tmp/server.log", "a");
    if (!log_file) {
        std::cerr << "Failed to open log file" << std::endl;
        return;
    }

    // Vulnerable part: using user-controlled value as format string
    fprintf(log_file, "[%s] Request:\n", timestamp);
    for (const auto& [key, value] : query_params) {
        fprintf(log_file, "Key: %s, Value: ", key.c_str());
        fprintf(log_file, value.c_str());  // 💀 Vulnerable to format string attack
        fprintf(log_file, "\n");
    }

    fprintf(log_file, "\n");

    // Log the response (still could be dangerous if response is user-controlled)
    fprintf(log_file, "[%s] Response:\n", timestamp);
    fprintf(log_file, "%s\n", response.c_str());

    fclose(log_file);  // Close the file
}
