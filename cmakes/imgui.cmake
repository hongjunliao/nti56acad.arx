#############################################################################################
cmake_minimum_required(VERSION 2.8.12)
project(imgui)
set(SOURCE_FILES imgui.cpp
			imgui_demo.cpp
			imgui_widgets.cpp
			imgui_draw.cpp
			backends/imgui_impl_win32.cpp
			backends/imgui_impl_opengl3.cpp
			misc/cpp/imgui_stdlib.cpp)

add_library(imgui STATIC ${SOURCE_FILES})
target_include_directories(imgui PRIVATE . )
target_link_libraries(imgui opengl32) 
