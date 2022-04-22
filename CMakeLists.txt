cmake_minimum_required(VERSION 3.0.0)

project(ImageProcessing)

set(CORE_ROOT ./Core)

# Build TwoImageAdd
set(TwoImageAdd_Src ${CORE_ROOT}/TwoImageAdd.cpp)
set(TwoImageAdd_Inc ${CORE_ROOT}/TwoImageAdd.h)
add_library(TwoImageAdd STATIC ${TwoImageAdd_Src} ${TwoImageAdd_Inc} )
target_include_directories(TwoImageAdd PUBLIC ${CORE_ROOT})

# Build ImageInterpolation
set(ImageInterpolation_Src ${CORE_ROOT}/ImageInterpolation.cpp)
set(ImageInterpolation_Inc ${CORE_ROOT}/ImageInterpolation.h)
add_library(ImageInterpolation STATIC ${ImageInterpolation_Src} ${ImageInterpolation_Inc})
target_include_directories(ImageInterpolation PUBLIC ${CORE_ROOT})


add_subdirectory(cmake/example/TwoImageAdd)
add_subdirectory(cmake/example/ImageInterpolation)
