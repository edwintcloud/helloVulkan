cmake_minimum_required(VERSION 3.1.0)

# project name and version
project(helloVulkan VERSION 0.1.0)

# variables
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g3 -D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
if(WIN32 AND CMAKE_BUILD_TYPE MATCHES Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin/win32/debug)
elseif(WIN32)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin/win32/release)
elseif(CMAKE_BUILD_TYPE MATCHES Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin/linux/debug)
else()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin/linux/release)   
endif()

# sources
file(GLOB SOURCES "src/*.cpp")

# includes
include_directories("$ENV{VULKAN_SDK}/include")
link_directories("$ENV{VULKAN_SDK}/lib") 
link_directories("$ENV{VULKAN_SDK}/etc/explicit_layer.d")

# Add GLFW
find_package(glfw3 3.2 REQUIRED)

# debug stuff
include(CTest)
enable_testing()


# executable
add_executable(${PROJECT_NAME} ${SOURCES})

# linker
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} vulkan)

# debug stuff
include(CPack)
