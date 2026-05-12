cmake_minimum_required(VERSION 3.28)
project(Project1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Project1
        main.cpp
        menu/menu.cpp
        parser/parser.cpp
        pipeline/pipeline.cpp
        webs/webs.cpp
        graph_builder/graph_builder.cpp
)

target_include_directories(Project1 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)