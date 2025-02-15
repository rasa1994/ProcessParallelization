# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.29)

enable_testing()

set(VCPKG_ROOT "VCPKG_ROOT" CACHE STRING "VCPKG_ROOT")

if (NOT DEFINED VCPKG_ROOT)
  message(FATAL_ERROR "MISSING ROOT FOR VCPKG setting to C:/vcpkg")
  set(VCPKG_ROOT "C:/vcpkg")
endif()

set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

project ("ProcessParallelization")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_TOOLCHAIN_FILE})

add_subdirectory ("ProcessParallelization")
add_subdirectory("include")
add_subdirectory("Testing")