cmake_minimum_required(VERSION 3.7)

project(nti56acad)

find_package(OPENGL REQUIRED)

add_executable(nti56acad
    application.cpp
    glad.c
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
    main.cpp
    )

target_include_directories(nti56acad
    PUBLIC include deps/arx/inc-x64/ deps/arx/inc/  src/ src/libxhhp/src/
    PUBLIC imgui)

target_compile_features(nti56acad
    PRIVATE cxx_auto_type
    PRIVATE cxx_nullptr
    PRIVATE cxx_range_for
    )

target_link_directories(nti56acad PRIVATE lib deps/arx/lib-x64/)
target_link_libraries(nti56acad
    PRIVATE accore acad acui22 adui22 ac1st22 acdb22 acge22 acgiapi rxapi
        ${OPENGL_LIBRARIES})

target_compile_options(nti56acad
    PUBLIC 
        -DUNICODE
        -D_UNICODE
        -DIMGUI_IMPL_WIN32_DISABLE_GAMEPAD)

if(MINGW)
    target_link_options(nti56acad
        PUBLIC -static -mwindows -municode)
endif()
