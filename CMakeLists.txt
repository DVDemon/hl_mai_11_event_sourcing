cmake_minimum_required(VERSION 3.0)

project(event_sourcing)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL Data)

if(OPENSSL_FOUND)
    set(HTTPLIB_IS_USING_OPENSSL TRUE)
endif()

include_directories(${Poco_INCLUDE_DIRS})

add_executable(event_server main_server.cpp)

target_link_libraries(event_server ${CMAKE_THREAD_LIBS_INIT}
                                    ${Poco_LIBRARIES}
                                    "PocoDataMySQL"
                                     OpenSSL::SSL
                                     OpenSSL::Crypto)
