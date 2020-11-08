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
        Camera.h Camera.cpp
        Affine.cpp
        Affine.h
        ImGui/imgui.cpp
        ImGui/imgui_draw.cpp
        ImGui/imgui_widgets.cpp
        ImGui/imgui_demo.cpp
        ImGui/examples/imgui_impl_glfw.cpp
        ImGui/examples/imgui_impl_opengl3.cpp Object.cpp Object.h
        Scene.cpp Scene.h Shader.cpp Shader.h Vertex.cpp Vertex.h VertexArray.cpp VertexArray.h ProgramPipeline.cpp ProgramPipeline.h UniformBuffer.cpp UniformBuffer.h MyUBO.cpp MyUBO.h Texture.cpp Texture.h)

# Set target properties
target_link_libraries( ${TARGET_NAME} PUBLIC ${OPENGL_opengl_LIBRARY} ${GLEW_LIBRARIES} ${GLFW_LIBRARIES} )
target_link_libraries( ${TARGET_NAME} PUBLIC ${OPENGL_glu_LIBRARY} ${OPENGL_glx_LIBRARY})
target_link_libraries( ${TARGET_NAME} PUBLIC GLEW::GLEW)
target_link_libraries( ${TARGET_NAME} PUBLIC glfw)