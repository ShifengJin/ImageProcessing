cmake_minimum_required(VERSION 3.16)

project(TwoImageAddTest)

set(workpath ./../../../)

set(CORE_INCLUDE_DIR ${workpath}Core)

set(TwoImageAddLib TwoImageAdd)

find_package(OpenCV REQUIRED)

include_directories(
        ${CORE_INCLUDE_DIR} 
        ${OpenCV_INCLUDE_DIRS}
        )

set(SRC ${workpath}example/TwoImageAddTest.cpp)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS}
                                      ${TwoImageAddLib}
                                      )

