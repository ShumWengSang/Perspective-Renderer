cmake_minimum_required(VERSION 3.0)
project(DearImgui)

add_library(dear_imgui STATIC src/imgui.cpp src/imgui_draw.cpp src/imgui_demo.cpp
        src/imgui_tables.cpp
        src/imgui_widgets.cpp)
target_include_directories(dear_imgui PUBLIC include/)
target_link_libraries(dear_imgui PRIVATE glfw)
target_include_directories(dear_imgui PRIVATE "../glfw/include")