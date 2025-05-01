#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>

void log_request_response(const std::string& request, const std::string& response) {
    // Get current time
    std::time_t current_time = std::time(nullptr);
    std::tm* time_info = std::localtime(&current_time);

    // Format timestamp
    char timestamp[20];
    std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", time_info);

    // Open the log file in append mode using FILE* for fprintf
    FILE* log_file = fopen("/tmp/server.log", "a");
    if (!log_file) {
        std::cerr << "Failed to open log file" << std::endl;
        return;
    }

    // Vulnerable part: using fprintf with user-controlled data (request/response)
    fprintf(log_file, "[%s] Request:\n", timestamp);
    fprintf(log_file, request.c_str());  // Vulnerable line - format string injection possible
    fprintf(log_file, "\n\n");

    fprintf(log_file, "[%s] Response:\n", timestamp);
    fprintf(log_file, response.c_str());  // Vulnerable line - format string injection possible
    fprintf(log_file, "\n\n");

    fclose(log_file);  // Close the file
}
