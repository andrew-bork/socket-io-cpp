cmake_minimum_required(VERSION 3.27.0)
set(CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)


project(socket-io-cpp)

add_library(net src/net/socket.cpp src/net/server.cpp src/net/net.cpp)
target_link_libraries(net ${CMAKE_SOURCE_DIR}/lib/libev.a)

FILE(GLOB HTTP_SRC src/http/*.cpp)
add_library(http ${HTTP_SRC})
target_link_libraries(http net)


add_executable(server tests/server.cpp)
target_link_libraries(server net)
# add_executable(client tests/client.cpp)
# target_link_libraries(client net)
# add_executable(http tests/http_test.cpp)
# add_executable(parse tests/parse.cpp)
# add_executable(hserv tests/http_server.cpp)

include_directories(include)

# Turn on all warning.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")