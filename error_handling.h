// error_handling.h

#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#ifdef __cplusplus
extern "C" {
#endif

void send_error_response(int client_socket, int status_code, const char* status_text, const char* requested_page);

#ifdef __cplusplus
}
#endif

#endif // ERROR_HANDLING_H
