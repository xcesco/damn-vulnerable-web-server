#ifndef MIME_TYPE_HANDLER_H
#define MIME_TYPE_HANDLER_H

#include <cstdio>   // for FILE
#include <cstdbool> // for bool in C++

#ifdef __cplusplus
extern "C" {
#endif

const char* get_content_type(const char* file_path);
bool check_php_file(const char* file_path);
void handle_php_file(FILE* file, int* client_socket, const char* response_header);

#ifdef __cplusplus
}
#endif

#endif // MIME_TYPE_HANDLER_H
