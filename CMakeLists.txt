cmake_minimum_required(VERSION 3.5)
project(DeribitClient VERSION 1.0.0)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Required packages
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

# Enable Qt automoc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Ceres REQUIRED)
find_package(Qt6 COMPONENTS Widgets Charts REQUIRED)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/external/websocketpp)
include_directories(${Boost_INCLUDE_DIRS})

# Find all source files
file(GLOB project_SRCS 
    src/*.cpp 
    app/*.cpp
    include/*.h
)

# Add external libraries
add_subdirectory(${PROJECT_SOURCE_DIR}/external/websocketpp)
add_subdirectory(${PROJECT_SOURCE_DIR}/external/poco)

# Add the executable
add_executable(${PROJECT_NAME} ${project_SRCS})

# Specify link libraries
target_link_libraries(${PROJECT_NAME} 
    Qt6::Widgets 
    Qt6::Charts 
    Ceres::ceres 
    Poco::Net 
    Poco::Util 
    Poco::JSON 
    Poco::Foundation 
    OpenSSL::SSL 
    OpenSSL::Crypto 
    ${Boost_LIBRARIES}
)
