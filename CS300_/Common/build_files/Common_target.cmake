
target_include_directories( ${TARGET_NAME} PUBLIC ${OPENGL_INCLUDE_DIR} )
target_include_directories( ${TARGET_NAME} PUBLIC ${GLFW_INCLUDE_DIRS} )
target_include_directories( ${TARGET_NAME} PUBLIC "../Common/" )
target_include_directories( ${TARGET_NAME} PUBLIC /usr/local/include/)


add_library( common_lib
        ../Common/Scene.cpp
        ../Common/Scene.h)


target_include_directories( common_lib PUBLIC /usr/local/include/eigen3/ )

target_link_libraries( ${TARGET_NAME} PUBLIC ${OPENGL_opengl_LIBRARY} ${GLEW_LIBRARIES}
        ${GLFW_LIBRARIES} )
target_link_libraries( ${TARGET_NAME} PUBLIC ${OPENGL_glu_LIBRARY} ${OPENGL_glx_LIBRARY})
target_link_libraries( ${TARGET_NAME} PUBLIC GLEW::GLEW)

target_link_libraries( ${TARGET_NAME} PUBLIC common_lib )

message( "GLFW Include : " ${GLFW_INCLUDE_DIRS} )
message( "GLFW Libs : " ${GLFW_LIBRARIES} )
message( "GLEW Include: " ${GLEW_INCLUDE_DIRS})
message( "GLEW Libs: " ${GLEW_LIBRARIES})
message( "Target: ${TARGET_NAME}")
