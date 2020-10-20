cmake_minimum_required(VERSION 2.8.12)
project(pointcloud_cropper)
cmake_minimum_required(VERSION 3.14)

project(pointcloud_cropper)

add_subdirectory(pybind11)

include(tbb.cmake)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

pybind11_add_module(pointcloud_cropper src/pointcloud_cropper.cpp)
target_include_directories(pointcloud_cropper SYSTEM PRIVATE ${TBB_INCLUDE_DIR})
target_link_directories(pointcloud_cropper PRIVATE ${TBB_LIB_DIR})
target_link_libraries(pointcloud_cropper PRIVATE ${TBB_LIBRARIES})
add_dependencies(pointcloud_cropper ext_tbb)
