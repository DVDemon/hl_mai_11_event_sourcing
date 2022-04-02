cmake_minimum_required(VERSION 3.0)

project(event_sourcing)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

if(OPENSSL_FOUND)
    set(HTTPLIB_IS_USING_OPENSSL TRUE)
endif()


add_executable(event_server main_server.cpp)
add_executable(event_client main_client.cpp)

target_link_libraries(event_server ${CMAKE_THREAD_LIBS_INIT}
                                     OpenSSL::SSL
                                     OpenSSL::Crypto)
                                     
target_link_libraries(event_client ${CMAKE_THREAD_LIBS_INIT}
                                     OpenSSL::SSL
                                     OpenSSL::Crypto)