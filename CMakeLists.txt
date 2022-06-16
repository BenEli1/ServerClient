cmake_minimum_required(VERSION 3.20)
project(ServerClient C)

set(CMAKE_C_STANDARD 99)

add_executable(ServerClient ex4_srv.c ex4_client.c)
