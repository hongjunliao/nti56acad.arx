# author hongjun.liao <docici@126.com>
# date 2020/8/21
# build nti56acad_test.exe, nti56acadmfc.exe and nti56acad.arx

# 1.always use MFC DLL version without DEBUG

cmake_minimum_required(VERSION 3.7)
find_package(OPENGL REQUIRED)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(src/libxhhp)

# for nti56acad.arx
project(nti56acad)
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
	deps/imgui/examples/imgui_impl_win32.cpp
	deps/imgui/examples/imgui_impl_opengl2.cpp
	deps/gbk-utf8/utf8.c
	deps/sds/win32_sds.c
	src/glad.c
	deps/redis/src/adlist.c
	deps/redis/src/zmalloc.c
)
file(GLOB SRCS ${SRCS} deps/ImGuiFileDialog/ImGuiFileDialog/*)
file(GLOB SRCS ${SRCS}
	src/config.h src/stdafx.h src/*.h
	src/stdafx.cpp src/arximpl.cpp src/nti_arx.cpp src/nti_imgui.cpp src/nti_reactor.cpp
	src/nti_dockbar.cpp src/nti_cmd.cpp src/test_nti_imgui.cpp
	src/nti_render.cpp src/nti_str.cpp src/nti_xlsx.cpp src/test_libxlsxio.c
)

add_library(nti56acad  SHARED ${SRCS})
target_link_directories(nti56acad PRIVATE lib deps/arx/lib-x64/ deps/xlsxio/lib/)
target_link_libraries(nti56acad
    PRIVATE accore acad acui22 adui22 ac1st22 acdb22 acge22 acgiapi rxapi ${OPENGL_LIBRARIES}
	libxlsxio_read libxlsxio_write
	)
target_compile_options(nti56acad PUBLIC
	-Iinclude/ -Ideps/imgui/ -Ideps/imgui/examples/ -Ideps/ImGuiFileDialog/ -Ideps/xlsxio/include -Ideps/dirent/include
	-Ideps/redis/src/ -Ideps/arx/inc-x64/ -Ideps/arx/inc/ -Isrc/libxhhp/src/ -Isrc/
	-D_WIN32 -DUNICODE -D_UNICODE -DHAVE_CONFIG_H -DIMGUI_IMPL_OPENGL_LOADER_GLAD /MD
	-D_AFXDLL
	)
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
	deps/redis/src/ src/libxhhp/src/ . res/)
target_link_directories(nti56acadmfc PRIVATE lib deps/xlsxio/lib/)
target_link_libraries(nti56acadmfc PRIVATE ${OPENGL_LIBRARIES} xhhp libxlsxio_read libxlsxio_write)
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
	src/main.cpp
)
add_executable(nti56acad_test ${SRCS})
target_include_directories(nti56acad_test PRIVATE
	include src/ deps/ deps/imgui deps/imgui/examples/ deps/ImGuiFileDialog deps/xlsxio/include deps/dirent/include
	../deps/redis/src/ src/libxhhp/src/ . res/)
target_link_directories(nti56acad_test PRIVATE lib deps/xlsxio/lib/)
target_link_libraries(nti56acad_test PRIVATE ${OPENGL_LIBRARIES} jemalloc xhhp libxlsxio_read libxlsxio_write)
target_compile_options(nti56acad_test PUBLIC
	-Iinclude/ -I../deps/imgui/ -I../deps/imgui/backends/ -I../deps/imgui/examples/ -I../deps/ImGuiFileDialog/ 
	-I../deps/xlsxio/include -I../deps/dirent/include
	-I../deps/redis/src/ -I../deps/arx/inc-x64/ -I../deps/arx/inc/ -Isrc/libxhhp/src/ -Isrc/
	-I../deps/redis/deps/jemalloc-win/include
	-I../deps/libc/
	-DNTI56_WITHOUT_ARX
	-D_WIN32 -DUNICODE -D_UNICODE -DHAVE_CONFIG_H -DIMGUI_IMPL_OPENGL_LOADER_GLAD /MD
	-D_AFXDLL
	-DHAVE_STRING_H
#	-DUSE_JEMALLOC -D_OFF_T_DEFINED -DLACKS_STDLIB_H -DJEMALLOC_MANGLE
	-D_OFF_T_DEFINED -DLACKS_STDLIB_H
	-D_CRT_SECURE_NO_WARNINGS
	)
target_link_options(nti56acad_test PUBLIC
	/SUBSYSTEM:WINDOWS /NODEFAULTLIB:msvcrt.lib
#	/DEF:../src/libxhhp/scripts/libhp.def
	)
