cmake_minimum_required(VERSION 3.27.0)
set(CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)


project(socket-io-cpp)

add_executable(server src/server.cpp)
add_executable(client src/client.cpp)
add_executable(http src/http_test.cpp)
add_executable(parse src/parse.cpp)
add_executable(hserv src/http_server.cpp)

include_directories(include)

# Turn on all warning.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")