cmake_minimum_required(VERSION 2.8.0)
project(wbkb)

add_subdirectory("wiiuse")

find_path(XDO_INCLUDE_DIRS xdo.h)
find_library(XDO_LIBRARIES libxdo)

add_executable(wbkb wbkb.c)

include_directories(${WIIUSE_INCLUDE_DIRS} ${XDO_INCLUDE_DIRS})
target_link_libraries(wbkb ${WIIUSE_LIBRARIES})
target_link_libraries(wbkb xdo)
