# author hongjun.liao <docici@126.com>
# date 2020/8/21
# build nti56acad_test.exe, nti56acadmfc.exe and nti56acad.arx

# 1.always use MFC DLL version without DEBUG

cmake_minimum_required(VERSION 3.7)
find_package(OPENGL REQUIRED)
if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Options are: None, Debug, Release, RelWithDebInfo, MinSizeRel." FORCE)
endif()
# -DCMAKE_EXPORT_COMPILE_COMMANDS is for lsp/clangd, see
# https://clangd.llvm.org/installation.html
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# show config
message("CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} Options are: Debug|Release")
# for nti56acad.arx
project(nti56acad)
# deps
add_subdirectory(deps/libhp)
# use MFC dll
#add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2)
#
#set(CMAKE_SHARED_LINKR_FLAGS "${CMAKE_SHARED_LINKR_FLAGS} /NODEFAULTLIB:libc.lib")

# sources
file(GLOB SRCS
	deps/imgui/imgui.cpp
	deps/imgui/imgui_demo.cpp
	deps/imgui/imgui_widgets.cpp
	deps/imgui/imgui_draw.cpp
	deps/imgui/backends/imgui_impl_win32.cpp
	deps/imgui/backends/imgui_impl_opengl2.cpp
	src/glad.c
	deps/imgui/misc/cpp/imgui_sds.cpp deps/imgui/misc/cpp/imgui_stdlib.cpp
)
file(GLOB SRCS ${SRCS} deps/ImGuiFileDialog/ImGuiFileDialog/*)
file(GLOB SRCS ${SRCS}
	src/config.h src/stdafx.h src/*.h
	src/stdafx.cpp src/arximpl.cpp src/nti_arx.cpp src/nti_imgui.cpp src/nti_imgui_dx9.cpp src/nti_reactor.cpp
	src/nti_dockbar.cpp src/nti_cmd.cpp src/test_nti_imgui.cpp
	src/nti_render.cpp src/nti_str.cpp src/nti_xlsx.cpp src/test_libxlsxio.c
	src/nti_cmn.cpp src/nti_blocksbar.cpp
	src/test_imgui_sds.cpp 
)
# build nti56acad.dll
add_library(nti56acad  SHARED ${SRCS})
set_property(TARGET nti56acad PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
target_compile_options(nti56acad PUBLIC
	-DHAVE_CONFIG_H -D_GNU_SOURCE -DCMAKE_EXPORT_COMPILE_COMMANDS
	-D_WIN32 -DUNICODE -D_UNICODE -DHAVE_CONFIG_H -DIMGUI_IMPL_OPENGL_LOADER_GLAD "/MD"
	-D_AFXDLL -D_CRT_SECURE_NO_WARNINGS -DNTI_USE_OPENGL)
target_include_directories(nti56acad PRIVATE 
	include/ 
	deps/ deps/imgui/ deps/imgui/backends/ deps/imgui/examples/ deps/ImGuiFileDialog/ 
	deps/dirent/include
	deps/arx/inc-x64/ deps/arx/inc/
	deps/libhp/include/
	deps/libhp/deps/ )
target_link_libraries(nti56acad
    PRIVATE accore acad acui22 adui22 ac1st22 acdb22 acge22 acgiapi rxapi ${OPENGL_LIBRARIES}
	libxlsxio_read libxlsxio_write libhp ArxDbg
	)
target_link_directories(nti56acad PRIVATE lib deps/arx/lib-x64/ deps/xlsxio/lib/)
target_link_options(nti56acad PUBLIC
	/DEF:../deps/arx/inc/AcRxDefault.def)

# for nti56acadmfc.exe
project(nti56acadmfc)
# use MFC dll
#add_definitions(-D_AFXDLL)
#set(CMAKE_MFC_FLAG 2)
#
#set(CMAKE_SHARED_LINKR_FLAGS "${CMAKE_SHARED_LINKR_FLAGS} /NODEFAULTLIB:libc.lib")

file(GLOB SRCS
	deps/imgui/imgui.cpp
	deps/imgui/imgui_demo.cpp
	deps/imgui/imgui_widgets.cpp
	deps/imgui/imgui_draw.cpp
	deps/imgui/backends/imgui_impl_win32.cpp
	deps/imgui/backends/imgui_impl_opengl2.cpp
	deps/gbk-utf8/utf8.c
	deps/sds/win32_sds.c
	src/glad.c
	deps/redis/src/adlist.c
	deps/redis/src/zmalloc.c
)
file(GLOB SRCS ${SRCS}
	src/config.h src/stdafx.h src/*.h
	src/stdafx.cpp src/nti56acadmfc.cpp src/nti_imgui.cpp
	src/nti_dockbar.cpp src/test_nti_imgui.cpp
	src/nti_str.cpp src/nti_xlsx.cpp src/test_libxlsxio.c
	src/ChildView.cpp src/MainFrm.cpp
)

add_executable(nti56acadmfc ${SRCS})
target_include_directories(nti56acadmfc PRIVATE
	include src/ deps/ deps/imgui deps/imgui/examples/ deps/ImGuiFileDialog deps/xlsxio/include deps/dirent/include
	deps/redis/src/  . res/)
target_link_directories(nti56acadmfc PRIVATE lib deps/xlsxio/lib/)
target_link_libraries(nti56acadmfc PRIVATE ${OPENGL_LIBRARIES} libhp libxlsxio_read libxlsxio_write)
target_compile_options(nti56acadmfc PUBLIC
	-Iinclude/ -I../deps/imgui/ -I../deps/imgui/backends/ -I../deps/imgui/examples/ -I../deps/ImGuiFileDialog/ 
	-I../deps/xlsxio/include -I../deps/dirent/include
	-I../deps/redis/src/ -I../deps/arx/inc-x64/ -I../deps/arx/inc/ -Isrc/libxhhp/src/ -Isrc/
	-DNTI56_WITHOUT_ARX
	-D_WIN32 -DUNICODE -D_UNICODE -DHAVE_CONFIG_H -DIMGUI_IMPL_OPENGL_LOADER_GLAD /MD
	-D_AFXDLL
	)
target_link_options(nti56acadmfc PUBLIC
	/SUBSYSTEM:WINDOWS)

# for nti56acad_test.exe
project(nti56acad_test)
file(GLOB SRCS 
	src/stdafx.h
	src/main.cpp
)
add_executable(nti56acad_test ${SRCS})
target_include_directories(nti56acad_test PRIVATE 
	include/ 
	deps/ deps/imgui/ deps/imgui/backends/ deps/imgui/examples/ deps/ImGuiFileDialog/ 
	deps/dirent/include
	deps/arx/inc-x64/ deps/arx/inc/
	deps/libhp/include/
	deps/libhp/deps/
)
target_link_directories(nti56acad_test PRIVATE lib deps/xlsxio/lib/)
target_link_libraries(nti56acad_test PRIVATE ${OPENGL_LIBRARIES} libhp)
target_compile_options(nti56acad_test PUBLIC
	-DNTI56_WITHOUT_ARX
	-D_WIN32 -DUNICODE -D_UNICODE -DHAVE_CONFIG_H -DIMGUI_IMPL_OPENGL_LOADER_GLAD /MD
	-D_AFXDLL
	-DHAVE_STRING_H
#	-DUSE_JEMALLOC -D_OFF_T_DEFINED -DLACKS_STDLIB_H -DJEMALLOC_MANGLE
#	-D_OFF_T_DEFINED -DLACKS_STDLIB_H
	-D_CRT_SECURE_NO_WARNINGS
	)
target_link_options(nti56acad_test PUBLIC
	/SUBSYSTEM:WINDOWS
	)
