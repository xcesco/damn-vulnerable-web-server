cmake_minimum_required(VERSION 3.25)
project(damn_vulnerable_web_server CXX)

set(CMAKE_CXX_STANDARD 11)

# Add OpenSSL flags and include directories
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

# Add your source files to the executable
add_executable(damn_vulnerable_web_server
    main.cpp
    mime_type_handler.cpp
    request_logger.cpp
    error_handling.cpp
    authentication.cpp
    base64.cpp
    utils.cpp
    session_manager.cpp
    response_handler.cpp
)

# Only apply flags after target is defined
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_options(damn_vulnerable_web_server PRIVATE -fno-stack-protector -z execstack -no-pie)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_compile_options(damn_vulnerable_web_server PRIVATE -fno-stack-protector)
endif()

target_include_directories(damn_vulnerable_web_server PRIVATE ${OPENSSL_INCLUDE_DIR})

target_link_libraries(damn_vulnerable_web_server PRIVATE OpenSSL::SSL OpenSSL::Crypto)
