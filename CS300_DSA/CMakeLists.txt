cmake_minimum_required(VERSION 3.17)
project(OpenGLFramework)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Source/deps/)

file(GLOB SOURCES "Source/src/*.c" "Source/src/*.cpp")
# Remove the stdafx from the glob
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Source/src/stdafx.cpp")

# Glob the imple for imgui
file(GLOB EXT_SOURCES "Source/externalSrc/*.c" "Source/externalSrc/*.cpp")

# We sometimes want to compile files as both C++ and GLASL.
# This allows us to access files from the shader directory.
file(GLOB SHADER_CROSS_SOURCES "shaders/*.h")

message("Sources ${SOURCES}")

add_executable(OpenGLFramework ${SOURCES} ${SHADER_CROSS_SOURCES} ${EXT_SOURCES})

source_group("Source" FILES ${SOURCES})
source_group("Cross Shaders" FILES ${SHADER_CROSS_SOURCES})

target_include_directories(OpenGLFramework PRIVATE "Source/src/")
target_include_directories(OpenGLFramework PRIVATE "shaders/")
target_include_directories(OpenGLFramework PRIVATE "Source/externalSrc/")

target_link_libraries(OpenGLFramework PRIVATE glfw)
target_link_libraries(OpenGLFramework PRIVATE glad)
target_link_libraries(OpenGLFramework PRIVATE stb_image)
target_link_libraries(OpenGLFramework PRIVATE dear_imgui)
target_link_libraries(OpenGLFramework PRIVATE glm_static)
target_link_libraries(OpenGLFramework PRIVATE tinyobjloader)


target_precompile_headers(OpenGLFramework PRIVATE "Source/src/stdafx.cpp")

add_custom_command(TARGET OpenGLFramework POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Common $<TARGET_FILE_DIR:OpenGLFramework>/Common
        )
add_custom_command(TARGET OpenGLFramework POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders/
        $<TARGET_FILE_DIR:OpenGLFramework>/shaders
        )