cmake_minimum_required(VERSION 3.0.0)

project(ImageEdgeTest)

set(workpath ./../../../)

set(CORE_INCLUDE_DIR ${workpath}Core)

set(libImageProcessing ImageProcessing)

include_directories(
        ${CORE_INCLUDE_DIR} 
        ${workpath}ThirdLibrary/stb_image
        )

set(SRC ${workpath}example/ImageEdgeTest.cpp)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${libImageProcessing})

