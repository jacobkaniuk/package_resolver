# CMakeList.txt : CMake project for package_resolver, include source and define
# project specific logic here.
cmake_minimum_required(VERSION 3.20)
project(package_resolver_project)

#set(CMAKE_CXX_COMPILER "C:\\Program Files\\LLVM\\bin\\clang++")
#set(CMAKE_TOOLCHAIN_FILE "C:\\dev\\c++\\package_resolve\\clang-toolchain.cmake")

#list(APPEND CMAKE_PREFIX_PATH "C:\\dev\\lib\\yaml-cpp-0.8.0\\out\\install\\x64-Release\\lib\\cmake")
list(APPEND CMAKE_PREFIX_PATH "C:\\dev\\lib\\yaml-cpp-0.8.0\\out\\install\\x64-Debug\\lib\\cmake")
list(APPEND CMAKE_PREFIX_PATH "C:\\dev\\lib\\oneapi-tbb-2021.11.0\\lib\\cmake\\tbb")
#set(yaml-cpp_DIR "C:\\dev\\lib\\yaml-cpp-0.8.0\\out\\install\\x64-Debug\\lib\\cmake")
#set(CMAKE_CXX_FLAGS "-Dlogging=1")
#set(TBB_DIR "C:\\dev\\lib\\oneapi-tbb-2021.11.0\\lib\\cmake\\tbb")
#set(TBB_INCLUDE_DIR "C:\\dev\\lib\\oneapi-tbb-2021.11.0\\include")
find_package(yaml-cpp CONFIG REQUIRED)
find_package(TBB CONFIG REQUIRED)

if(yaml-cpp_FOUND)
  message(STATUS "YAML-CPP found!")
else()
  message(FATAL_ERROR "YAML-CPP not found")
endif()

if(TBB_FOUND)
  message(STATUS "TBB found!")
else()
  message(FATAL_ERROR "TBB not found")
endif()

message(STATUS "YAML-CPP include dir: ${YAML_CPP_INCLUDE_DIR}")
message(STATUS "YAML-CPP libraries: ${YAML_CPP_LIBRARIES}")

message(STATUS TBB ${TBB_INCLUDE_DIR})

# Add source to this project's executable.
add_executable (package_resolver "main.cpp" "package.cpp" "loader.cpp" "version_parser.cpp" "threadpool.h" )
set_target_properties(package_resolver PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(package_resolver PRIVATE ${CMAKE_SOURCE_DIR}/include ${YAML_CPP_INCLUDE_DIR} ${TBB_INCLUDE_DIR})
target_link_libraries(package_resolver PRIVATE yaml-cpp::yaml-cpp TBB::tbb)
link_directories(${TBB_LIBRARY_DIR})
set_property(TARGET package_resolver PROPERTY CXX_STANDARD 20)

if(TARGET TBB::tbb)
  message(STATUS "TBB found and linked!")
else()
  message(FATAL_ERROR "TBB target not found")
endif()

# TODO: Add tests and install targets if needed.
