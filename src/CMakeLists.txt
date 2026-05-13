cmake_minimum_required(VERSION 3.28)
project(Project2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Project2
        main.cpp
        menu/menu.cpp
        parser/parser.cpp
        pipeline/pipeline.cpp
        webs/webs.cpp
        graph_builder/graph_builder.cpp
        register_allocation/basic_allocation.cpp
        register_allocation/basic_allocation.h
        register_allocation/spilling_allocation.cpp
        register_allocation/spilling_allocation.h
        register_allocation/splitting_allocation.cpp
        register_allocation/splitting_allocation.h
        output/output.cpp
        output/output.h
)

target_include_directories(Project2 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)