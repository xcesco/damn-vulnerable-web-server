#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <map>

std::string extract_header_value(const std::string& request, const std::string& header_name);
std::map<std::string, std::string> extract_query_parameters(const std::string& request);
bool extract_username_password(const std::string& authorization_header, std::string& username, std::string& password);

#endif
