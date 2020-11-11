# author hongjun.liao <docici@126.com>
# date 2020/8/21
# build nti56acad_test.exe, nti56acadmfc.exe and nti56acad.arx

# 1.always use MFC DLL version without DEBUG

cmake_minimum_required(VERSION 3.7)
find_package(OPENGL REQUIRED)

# for nti56acad.arx
project(nti56acad)
# use MFC dll
add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2)
#
set(CMAKE_SHARED_LINKR_FLAGS "${CMAKE_SHARED_LINKR_FLAGS} /NODEFAULTLIB:libc.lib")

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
file(GLOB SRCS ${SRCS} src/libxhhp/src/libxhhp/hp_log.c)

add_library(nti56acad  SHARED ${SRCS})
target_link_directories(nti56acad PRIVATE lib deps/arx/lib-x64/ deps/xlsxio/lib/)
target_link_libraries(nti56acad
    PRIVATE accore acad acui22 adui22 ac1st22 acdb22 acge22 acgiapi rxapi ${OPENGL_LIBRARIES}
	libxlsxio_read libxlsxio_write
	)
target_compile_options(nti56acad PUBLIC
	-Iinclude/ -Ideps/imgui/ -Ideps/imgui/examples/ -Ideps/ImGuiFileDialog/ -Ideps/xlsxio/include -Ideps/dirent/include
	-Ideps/redis/src/ -Ideps/arx/inc-x64/ -Ideps/arx/inc/ -Isrc/libxhhp/src/ -Isrc/
	-D_WIN32 -DUNICODE -D_UNICODE -DHAVE_CONFIG_H -DIMGUI_IMPL_OPENGL_LOADER_GLAD /MD)
target_link_options(nti56acad PUBLIC
	/DEF:../deps/arx/inc/AcRxDefault.def)

# for nti56acadmfc.exe
project(nti56acadmfc)
# use MFC dll
add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2)
#
set(CMAKE_SHARED_LINKR_FLAGS "${CMAKE_SHARED_LINKR_FLAGS} /NODEFAULTLIB:libc.lib")

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
file(GLOB SRCS ${SRCS}
	src/config.h src/stdafx.h src/*.h
	src/stdafx.cpp src/nti56acadmfc.cpp src/nti_imgui.cpp
	src/nti_dockbar.cpp src/test_nti_imgui.cpp
	src/nti_str.cpp src/nti_xlsx.cpp src/test_libxlsxio.c
	src/ChildView.cpp src/MainFrm.cpp
)
file(GLOB SRCS ${SRCS} src/libxhhp/src/libxhhp/hp_log.c)

add_executable(nti56acadmfc ${SRCS})
target_include_directories(nti56acadmfc PRIVATE
	include src/ deps/ deps/imgui deps/imgui/examples/ deps/ImGuiFileDialog deps/xlsxio/include deps/dirent/include
	deps/redis/src/ src/libxhhp/src/ . res/)
target_link_directories(nti56acadmfc PRIVATE lib deps/xlsxio/lib/)
target_link_libraries(nti56acadmfc PRIVATE ${OPENGL_LIBRARIES} libxlsxio_read libxlsxio_write)
target_compile_options(nti56acadmfc PUBLIC
	-DNTI56_WITHOUT_ARX
	-D_WIN32 -DUNICODE -D_UNICODE -DHAVE_CONFIG_H -DIMGUI_IMPL_OPENGL_LOADER_GLAD /MD)
target_link_options(nti56acadmfc PUBLIC
	/SUBSYSTEM:WINDOWS)
