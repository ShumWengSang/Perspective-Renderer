cmake_minimum_required( VERSION 3.9 )
project( Simple_Scene )

set( CMAKE_CXX_STANDARD 17 )
set( OpenGL_GL_PREFERENCE GLVND)

#find_package(PkgConfig REQUIRED)
#pkg_search_module(GLFW REQUIRED glfw3)
find_package( glfw3 REQUIRED )
find_package( OpenGL REQUIRED )
find_package( GLEW REQUIRED )
find_package( glm REQUIRED)




set( TARGET_NAME SimpleScene_OGL4 )

include_directories(ImGui/examples/example_glfw_opengl3 ImGui/examples ImGui ${CMAKE_SOURCE_DIR})

#include_directories( ../Common/Scene.h ../Common/shader.hpp)
#set( CMAKE_CXX_FLAGS "-Wall -g " )

message( "GLEW Include: " ${GLEW_INCLUDE_DIRS})
message( "GLEW Libs: " ${GLEW_LIBRARIES})
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
        ImGui/examples/imgui_impl_opengl3.cpp Object.cpp Object.h OpenGL/OpenGLMesh.cpp OpenGL/OpenGLMesh.h ObjectManager.cpp ObjectManager.h Renderer/Light.cpp Renderer/Light.h
        Scene.cpp Scene.h Renderer/UniformBufferObject.cpp Renderer/UniformBufferObject.h OpenGL/OpenGLUniformBufferObject.cpp OpenGL/OpenGLUniformBufferObject.h)

# Set target properties
target_link_libraries( ${TARGET_NAME} PUBLIC ${OPENGL_opengl_LIBRARY} ${GLEW_LIBRARIES} ${GLFW_LIBRARIES} )
target_link_libraries( ${TARGET_NAME} PUBLIC ${OPENGL_glu_LIBRARY} ${OPENGL_glx_LIBRARY})
target_link_libraries( ${TARGET_NAME} PUBLIC GLEW::GLEW)
target_link_libraries( ${TARGET_NAME} PUBLIC glfw)