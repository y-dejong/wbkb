cmake_minimum_required(VERSION 2.8.0)
project(wbkb)

add_subdirectory("wiiuse")

find_path(XDO_INCLUDE_DIRS xdo.h)
if(XDO_INCLUDE_DIRS STREQUAL XDO_INCLUDE_DIRS-NOTFOUND)
  message(FATAL_ERROR "xdo libraries header files not found")
endif()

find_library(XDO_LIBRARIES xdo)
if(XDO_LIBRARIES STREQUAL XDO_LIBRARIES-NOTFOUND)
  message(FATAL_ERROR "xdo libraries not found")
endif()

add_executable(wbkb wbkb.c)

include_directories(${WIIUSE_INCLUDE_DIRS} ${XDO_INCLUDE_DIRS})
target_link_libraries(wbkb ${WIIUSE_LIBRARIES})
target_link_libraries(wbkb xdo)
