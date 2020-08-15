# author hongjun.liao <docici@126.com>
# date 2020/08/15

cmake_minimum_required(VERSION 2.8)
CMAKE_POLICY(SET CMP0015 NEW)

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Options are: None, Debug, Release, RelWithDebInfo, MinSizeRel." FORCE)
endif()


# -DCMAKE_EXPORT_COMPILE_COMMANDS is for lsp/clangd, see
# https://clangd.llvm.org/installation.html
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# show config
message("CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} Options are: Debug|Release")
message("")

# nti56acad 
project(mdm_imcli_server)
add_compile_options(-DHAVE_CONFIG_H -D_GNU_SOURCE -O0 -g3 -Wall -D${XHMDM_BUILD_TYPE} -DCMAKE_EXPORT_COMPILE_COMMANDS)
include_directories(include/ src/ src/libxhhp/src/)
link_directories(lib)

file(GLOB SRCS src/hello.c)
add_library(nti56acad SHARED ${SRCS} )

