# CMakeList.txt : CMake project for ProcessParallelization, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (ProcessParallelization "Source.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ProcessParallelization PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(ProcessParallelization PRIVATE processppl)
target_include_directories(ProcessParallelization PUBLIC "../include"})