#ifndef UNTITLED11_AUTHENTICATION_H
#define UNTITLED11_AUTHENTICATION_H

#include <string>

int authenticate(const std::string& username, const std::string& password);
int perform_authentication(int client_socket, const std::string& file_path, const std::string& request);
void send_basic_auth_prompt(int client_socket);
std::string extract_header_value(const std::string& request, const std::string& header_name);

#endif //UNTITLED11_AUTHENTICATION_H
