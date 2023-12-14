cmake_minimum_required(VERSION 3.11)

project(bridgecontrol  CXX)
set(CMAKE_CXX_STANDARD 17)

if (CMAKE_SYSTEM_NAME MATCHES "^MINGW")
    set(SYSTEM_LIBS -lstdc++)
else()
    set(SYSTEM_LIBS)
endif()


add_executable(bridgecontrol main.cpp netlink.cpp bridge.h netlink.h ${SYSTEM_LIBS}) 
