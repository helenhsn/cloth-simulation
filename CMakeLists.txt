cmake_minimum_required(VERSION 3.2)

project(cloth_sim LANGUAGES CXX C)

find_package(CUDA REQUIRED)

# set compiler flags/options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O0 -std=c++11 ")
set(cuda_additional_flags "--std=c++11" "-O0" "-g" "-G")

# external lib
add_subdirectory(vendors/glfw) # contains a cmake file
add_subdirectory(vendors/glm) # contains a cmakefile

include_directories(cloth_sim vendors/imgui)
include_directories(cloth_sim vendors/glad)
include_directories(cloth_sim vendors/tinyply)

cuda_include_directories(cloth_sim vendors/imgui)
cuda_include_directories(cloth_sim vendors/glad)
cuda_include_directories(cloth_sim vendors/tinyply)

set(GLAD_SRC_DIR "${PROJECT_SOURCE_DIR}/vendors/glad")
set(IMGUI_SRC_DIR "${PROJECT_SOURCE_DIR}/vendors/imgui")
set(TINYPLY_SRC_DIR "${PROJECT_SOURCE_DIR}/vendors/tinyply")

set(CUDA_VERBOSE_BUILD ON)
set(CUDA_SEPARABLE_COMPILATION ON) # Allow multiple CUDA files compilation


set (PROJECT_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
set (PROJECT_SRC_DIR      "${PROJECT_SOURCE_DIR}/src")

file(GLOB_RECURSE SOURCES "${PROJECT_SRC_DIR}/*.cpp" "${PROJECT_SRC_DIR}/*.c" "${GLAD_SRC_DIR}/*.c" "${IMGUI_SRC_DIR}/*.cpp" "${TINYPLY_SRC_DIR}/*.hpp")
file(GLOB_RECURSE SOURCES_CUDA "${PROJECT_SRC_DIR}/*.cu" "${PROJECT_INCLUDE_DIRS}/*.hcu")

include_directories(${CUDA_INCLUDE_DIRS})
include_directories("${PROJECT_INCLUDE_DIRS}")
cuda_include_directories("${PROJECT_INCLUDE_DIRS}")


CUDA_WRAP_SRCS(cloth_sim PTX CUDA_PTX_FILES ${SOURCES_CUDA}
               OPTIONS ${cuda_additional_flags})

cuda_add_executable(cloth_sim ${SOURCES} ${SOURCES_CUDA} ${CUDA_PTX_FILES}
                    OPTIONS ${cuda_additional_flags})

target_compile_options(cloth_sim PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:-G>)
target_compile_options(cloth_sim PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:-g>)
target_compile_options(cloth_sim PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:-O0>)


target_link_libraries(cloth_sim
                    ${CUDA_LIBRARIES}
                    ${CUDA_CUDA_LIBRARY}
                    ${CUDA_curand_LIBRARY}
                    glfw
                    glm
                    )