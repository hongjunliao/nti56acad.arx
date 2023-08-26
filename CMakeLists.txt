###########################################################################################
# author hongjun.liao <docici@126.com>
# date 2020/8/21
# build nti56acadmfc.exe and nti56acad.arx

# 1.always use MFC DLL version without DEBUG

###########################################################################################
cmake_minimum_required(VERSION 2.8.12)
include(deps/libhp/cmakes/libhp.cmake) # hp_cmake_find_deps

###########################################################################################
# nti56acad 
project(nti56acad)

###########################################################################################
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Options are: None, Debug, Release, RelWithDebInfo, MinSizeRel." FORCE)
endif()
# -DCMAKE_EXPORT_COMPILE_COMMANDS is for lsp/clangd, see
# https://clangd.llvm.org/installation.html
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

###########################################################################################
# sources and deps
if(NOT NTI56_ACADVER)
set(NTI56_ACADVER 2018)
endif()
set(arx arx/arx${NTI56_ACADVER})

set(g_withs 1 1 1 1 1)
set(g_hdrs .nullfilesub.h .nullfilesub.h .nullfilesub.h .nullfilesrc.h .nullfilesub.h)
set(g_incs "deps/${arx}/inc-x64/ deps/${arx}/inc/" "deps/libhp/include/" 
		   "deps/imgui/ deps/imgui/backends/ deps/imgui/examples/" "${PROJECT_BINARY_DIR}/ deps/libhp/deps/" 
		   "deps/${arx}/samples/database/ARXDBG/Inc/")
set(g_deps ${arx} libhp imgui .src ${arx}/samples/database/ARXDBG)
set(g_libs "accore acad acui22 adui22 ac1st22 acdb22 acge22 acgiapi rxapi AcDrawBridge" 
		    libhp imgui "src/*.c src/*.cpp" ArxDbg)

#file(GLOB SRCS ${SRCS} deps/ImGuiFileDialog/ImGuiFileDialog/*)
###########################################################################################
# build nti56acad.arx
set(CMAKE_SHARED_LIBRARY_SUFFIX ".arx")
hp_cmake_find_deps(SRCS NTI56ACAD_WITH_NULL_ deps cmakes g_withs g_hdrs g_incs g_deps g_libs)
set(PCH_HEADER "src/stdafx.h")
set(PCH_SOURCE "src/stdafx.cpp")
add_library(nti56acad SHARED ${SRCS})
target_precompile_headers(nti56acad PRIVATE ${PCH_HEADER} ${PCH_SOURCE})

foreach(dep IN LISTS g_deps  )
	hp_log("nti56acad: dep='${dep}', ${dep}_INCLUDE_DIRS='${${dep}_INCLUDE_DIRS}', ${dep}_LIBRARIES='${${dep}_LIBRARIES}'")
	target_include_directories(nti56acad PRIVATE ${${dep}_INCLUDE_DIRS} )
	target_link_libraries(nti56acad ${${dep}_LIBRARIES} ) 
endforeach() 
target_link_directories(nti56acad PUBLIC deps/${arx}/lib-x64/ lib/)
###########################################################################################

# use MFC dll
#add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2)
set_property(TARGET nti56acad PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

target_compile_options(nti56acad PRIVATE
	-DNTI56_ARX -D_CRT_STDIO_ISO_WIDE_SPECIFIERS=1
	-DHAVE_CONFIG_H -D_GNU_SOURCE -DCMAKE_EXPORT_COMPILE_COMMANDS
	-D_WIN32 -DUNICODE -D_UNICODE -DHAVE_CONFIG_H -DIMGUI_IMPL_OPENGL_LOADER_GLAD "/MD"
	-D_AFXDLL -D_CRT_SECURE_NO_WARNINGS)

target_link_options(nti56acad PUBLIC /DEF:${PROJECT_SOURCE_DIR}/deps/${arx}/inc/AcRxDefault.def)

###########################################################################################
# for nti56acadmfc.exe
#set(g_withs 1 1 1)
#set(g_hdrs .nullfilesub.h .nullfilesub.h .nullfilesrc.h)
#set(g_incs "deps/libhp/include/" "deps/imgui/ deps/imgui/backends/ deps/imgui/examples/" "${PROJECT_BINARY_DIR}/ deps/libhp/deps/")
#set(g_deps libhp imgui .src)
#set(g_libs libhp imgui "src/mfc/*.cpp")
#hp_cmake_find_deps(NTI56ACAD_MFC_SRCS NTI56ACAD_MFC_WITH_NULL_ deps cmakes g_withs g_hdrs g_incs g_deps g_libs)

file(GLOB NTI56ACAD_MFC_SRCS "src/mfc/*.cpp"  src/nti_dockbar.cpp src/nti_str.cpp)

add_executable(nti56acadmfc ${NTI56ACAD_MFC_SRCS} src/mfc/exmaplemfc.rc)
target_precompile_headers(nti56acadmfc PRIVATE ${PCH_HEADER} ${PCH_SOURCE})

target_include_directories(nti56acadmfc PRIVATE ${imgui_INCLUDE_DIRS} ${libhp_INCLUDE_DIRS} ${PROJECT_BINARY_DIR}/ deps/libhp/deps/)
target_link_libraries(nti56acadmfc ${imgui_LIBRARIES} ${libhp_LIBRARIES} ) 

target_compile_options(nti56acadmfc PUBLIC
	-D_WIN32 -DUNICODE -D_UNICODE -DHAVE_CONFIG_H 
	-D_AFXDLL
	-D_CRT_STDIO_ISO_WIDE_SPECIFIERS=1
	)
target_link_options(nti56acadmfc PUBLIC /SUBSYSTEM:WINDOWS)

###########################################################################################
configure_file("config.h.in" "config.h")

