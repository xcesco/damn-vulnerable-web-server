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

    // Open the log file in append mode
    std::ofstream log_file("/tmp/server.log", std::ios_base::app);
    if (!log_file) {
        std::cerr << "Failed to open log file" << std::endl;
        return;
    }

    // Use printf-style formatting for logging
    log_file << "[" << timestamp << "] Request:\n" << request << "\n\n";
    log_file << "[" << timestamp << "] Response:\n" << response << "\n\n";

    // Log file is automatically closed when ofstream goes out of scope
}
