cmake_minimum_required(VERSION 3.0.0)

project(ImageProcessing)

set(CORE_ROOT ./Core)

set(TwoImageAdd_Src ${CORE_ROOT}/TwoImageAdd.cpp)
set(TwoImageAdd_Inc ${CORE_ROOT}/TwoImageAdd.h)

add_library(TwoImageAdd STATIC ${TwoImageAdd_Src}
                               ${TwoImageAdd_Inc}
                               )
target_include_directories(TwoImageAdd PUBLIC ${CORE_ROOT})
                    
add_subdirectory(cmake/example/TwoImageAdd)

