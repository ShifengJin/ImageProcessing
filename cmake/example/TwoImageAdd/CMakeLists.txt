cmake_minimum_required(VERSION 3.16)

project(TwoImageAdd)

include_directories(./../../../Core)
set(SRC ./../../../example/TwoImageAddTest.cpp 
        ./../../../Core/TwoImageAdd.cpp)

add_executable(${PROJECT_NAME} ${SRC})
