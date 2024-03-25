cmake_minimum_required(VERSION 3.5)
project(UdpSerialServer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Boost package
find_package(Boost 1.55.0 REQUIRED COMPONENTS system)

# Include directories
include_directories(${Boost_INCLUDE_DIRS})

# Define the executable
add_executable(server server.cpp)

# Link libraries
target_link_libraries(server ${Boost_LIBRARIES})
