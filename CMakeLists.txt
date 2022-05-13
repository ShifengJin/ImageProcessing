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

# Build ImageRotate
set(ImageRotate_Src ${CORE_ROOT}/ImageRotate.cpp)
set(ImageRotate_Inc ${CORE_ROOT}/ImageRotate.h)
add_library(ImageRotate STATIC ${ImageRotate_Src} ${ImageRotate_Inc})
target_include_directories(ImageRotate PUBLIC ${CORE_ROOT})

# Build ImageBlur
set(ImageBlur_Src ${CORE_ROOT}/ImageBlur.cpp)
set(ImageBlur_Inc ${CORE_ROOT}/ImageBlur.h)
add_library(ImageBlur STATIC ${ImageBlur_Src} ${ImageBlur_Inc})
target_include_directories(ImageBlur PUBLIC ${CORE_ROOT})

# Build All
aux_source_directory(${CORE_ROOT} SRC)
add_library(ImageProcessing STATIC ${SRC})
target_include_directories(ImageProcessing PUBLIC ${CORE_ROOT})

add_subdirectory(cmake/example/TwoImageAdd)
add_subdirectory(cmake/example/ImageInterpolation)
add_subdirectory(cmake/example/ImageRotate)
add_subdirectory(cmake/example/ImageBlur)
add_subdirectory(cmake/example/ImageHarrisCorner)
add_subdirectory(cmake/example/ImageSusanCorner)
add_subdirectory(cmake/example/ImageFastCorner)