cmake_minimum_required( VERSION 3.9 )
project( Simple_Scene )

include( ../Common/build_files/Common_packages.cmake )

set( TARGET_NAME SimpleScene_OGL4 )
set( CMAKE_CXX_FLAGS "-Wall -g " )

include_directories(ImGui/examples/example_glfw_opengl3 ImGui/examples ImGui)

#include_directories( ../Common/Scene.h ../Common/shader.hpp)

add_executable( ${TARGET_NAME}
        SimpleScene_Quad.cpp
        SimpleScene_Quad.h
        GLApplication.cpp
        ObjLoader.cpp
        ObjLoader.h
        stdafx.h
        Renderer/Buffer.cpp
        Renderer/Buffer.h
        OpenGL/OpenGLBuffer.cpp
        OpenGL/OpenGLBuffer.h
        OpenGL/OpenGLShader.h
        OpenGL/OpenGLShader.cpp
        Renderer/Shader.cpp
        Renderer/Shader.h
        Renderer/RendererAPI.cpp
        Renderer/RendererAPI.h
        OpenGL/OpenGLRendererAPI.cpp
        OpenGL/OpenGLRendererAPI.h
        Renderer/Renderer.cpp
        Renderer/Renderer.h
        Renderer/RenderCommand.cpp
        Renderer/RenderCommand.h
        Renderer/Camera.cpp
        Renderer/Camera.h
        Affine.cpp
        Affine.h
        ImGui/imgui.cpp
        ImGui/imgui_draw.cpp
        ImGui/imgui_widgets.cpp
        ImGui/imgui_demo.cpp
        ImGui/examples/imgui_impl_glfw.cpp
        ImGui/examples/imgui_impl_opengl3.cpp Object.cpp Object.h OpenGL/OpenGLMesh.cpp OpenGL/OpenGLMesh.h ObjectManager.cpp ObjectManager.h Renderer/Light.cpp Renderer/Light.h)

# Set target properties
include( ../Common/build_files/Common_target.cmake )