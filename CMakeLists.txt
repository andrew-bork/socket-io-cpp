cmake_minimum_required(VERSION 3.27.0)
set(CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
project(socket-io-cpp)

# file(GLOB LIBEV_SRC ${CMAKE_SOURCE_DIR}/libev/*.c)
# add_library(ev ${LIBEV_SRC})
# target_include_directories(ev PUBLIC ${CMAKE_SOURCE_DIR}/libev/)

FILE(GLOB NET_SRC src/net/*.cpp)
add_library(net ${NET_SRC})
target_link_libraries(net ${CMAKE_SOURCE_DIR}/libev/.libs/libev.a)

# add_library(llhttp llhttp/build/c/llhttp.c)
FetchContent_Declare(llhttp
  URL "https://github.com/nodejs/llhttp/archive/refs/tags/release/v8.1.0.tar.gz")

set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
set(BUILD_STATIC_LIBS ON CACHE INTERNAL "")
FetchContent_MakeAvailable(llhttp)




FILE(GLOB URL_SRC src/url/*.cpp)
add_library(url ${URL_SRC})

FILE(GLOB HTTP_SRC src/http/*.cpp)
add_library(http ${HTTP_SRC})
target_link_libraries(http net)
target_link_libraries(http url)
# Link with the llhttp_shared target
target_link_libraries(http llhttp_static)


FILE(GLOB WEBSOCKET_SRC src/websocket/*.cpp)
add_library(websocket ${WEBSOCKET_SRC})
target_link_libraries(websocket http)

add_executable(server tests/server.cpp)
target_link_libraries(server net)
add_executable(client tests/client.cpp)
target_link_libraries(client net)
add_executable(http_test tests/http_test.cpp)
target_link_libraries(http_test http)
add_executable(curl tests/curl.cpp)
target_link_libraries(curl http)
add_executable(ws-client tests/ws-client.cpp)
target_link_libraries(ws-client websocket)

# add_executable(parse tests/parse.cpp)
# add_executable(hserv tests/http_server.cpp)

include_directories(include)

# Turn on all warning.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")