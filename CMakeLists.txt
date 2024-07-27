cmake_minimum_required(VERSION 3.5)
project(DeribitClient VERSION 1.0.0)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Required pacakges
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(Ceres REQUIRED)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/external/websocketpp)
include_directories(${Boost_INCLUDE_DIRS})

# Find all executables
file(GLOB project_SRCS src/*.cpp app/*.cpp) #src/*.h

# Add external libraries
add_subdirectory(${PROJECT_SOURCE_DIR}/external/websocketpp)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/poco)

# Add the executable
add_executable(${PROJECT_NAME} ${project_SRCS})

# Specify link libraries (if you have any)
# target_link_libraries(${PROJECT_NAME} someLibrary)
# POCO
target_link_libraries(${PROJECT_NAME} Ceres::ceres Poco::Net Poco::Util Poco::JSON Poco::Foundation OpenSSL::SSL OpenSSL::Crypto ${Boost_LIBRARIES})
