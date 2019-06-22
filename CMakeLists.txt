cmake_minimum_required(VERSION 3.9)
project(bigwork)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_C_FLAGS "-O2")

include_directories(.)

add_executable(main1
        build_tree.cpp
        build_tree.h
        CG_debug.cpp
        CG_debug.h
        evaluation.cpp
        evaluation.h
        main1.cpp
        Node.cpp
        Node.h)

add_executable(main2
        build_tree.cpp
        build_tree.h
        CG_debug.cpp
        CG_debug.h
        evaluation.cpp
        evaluation.h
        main1.cpp
        Node.cpp
        Node.h)

add_executable(main3
        build_tree.cpp
        build_tree.h
        CG_debug.cpp
        CG_debug.h
        evaluation.cpp
        evaluation.h
        main3.cpp
        Node.cpp
        Node.h)
