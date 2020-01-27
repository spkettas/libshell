cmake_minimum_required(VERSION 3.12)
project(libshell)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(include)
include_directories(pb)
include_directories(test)

add_executable(libshell
        include/cli_config.h
        include/cli_public.h
        include/cli_server.h
        include/cli_shell.h
        include/tinyxml2.h
        src/tinyxml2.cpp
        src/cli_config.cpp
        src/cli_server.cpp
        src/cli_shell.cpp
        test/myshell.cpp
        test/parser.cpp
        test/demo.cpp)
