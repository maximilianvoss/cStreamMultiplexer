cmake_minimum_required(VERSION 2.8)

set(SOURCE_FILES_SERVER server.c)
set(SOURCE_FILES_CLIENT client.c)
add_executable(server ${SOURCE_FILES_SERVER})
add_executable(client ${SOURCE_FILES_CLIENT})

target_link_libraries(server cstreammultiplexer)
target_link_libraries(client cstreammultiplexer)