# author hongjun.liao <docici@126.com>
# date 2020/8/21
# build nti56acad_test.exe and nti56acad.arx

cmake_minimum_required(VERSION 3.7)
find_package(OPENGL REQUIRED)

# for nti56acad.arx
project(nti56acad)
file(GLOB_RECURSE SRCS src/* imgui/*.cpp)   
add_library(nti56acad  SHARED ${SRCS} )
target_include_directories(nti56acad PUBLIC include deps/arx/inc-x64/ deps/arx/inc/  src/ src/libxhhp/src/ imgui)
target_link_directories(nti56acad PRIVATE lib deps/arx/lib-x64/)
target_link_libraries(nti56acad
    PRIVATE accore acad acui22 adui22 ac1st22 acdb22 acge22 acgiapi rxapi
        ${OPENGL_LIBRARIES})

target_compile_options(nti56acad PUBLIC -DUNICODE -D_UNICODE -DNDEBUG -DIMGUI_IMPL_WIN32_DISABLE_GAMEPAD)

# for nti56acad_test.exe
project(nti56acad_test)

add_executable(nti56acad_test
    src/application.cpp
    src/glad.c
    imgui/imgui.cpp
    imgui/imgui.h
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_impl_opengl3.cpp
    imgui/imgui_impl_opengl3.h
    imgui/imgui_impl_win32.cpp
    imgui/imgui_impl_win32.h
    imgui/imgui_internal.h
    imgui/imgui_widgets.cpp
    src/main.cpp
    )

target_include_directories(nti56acad_test PUBLIC include src/ src/libxhhp/src/ imgui)
target_compile_features(nti56acad_test
    PRIVATE cxx_auto_type
    PRIVATE cxx_nullptr
    PRIVATE cxx_range_for
    )

target_compile_options(nti56acad_test
    PUBLIC 
        -DUNICODE
        -D_UNICODE
        -DIMGUI_IMPL_WIN32_DISABLE_GAMEPAD)
target_link_libraries(nti56acad_test
    PRIVATE ${OPENGL_LIBRARIES})
if(MINGW)
    target_link_options(nti56acad_test
        PUBLIC -static -mwindows -municode)
endif()
