cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_STANDARD 17)
project(webserv VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(Threads REQUIRED)

include_directories(inc)

# 将src的所有 CPP 源文件添加到 SOURCES 变量中
file(GLOB SOURCES "src/*.cpp")

# 编译这些源文件成一个库
add_executable(webserv main.cpp ${SOURCE})



target_link_libraries(webserv Threads::Threads)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

