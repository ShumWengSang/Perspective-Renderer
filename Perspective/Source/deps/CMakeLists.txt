
# stb_image
add_subdirectory(stb_image/)

# tinyobjloader
add_subdirectory(tinyobjloader/)

# dear imgui
add_subdirectory(dear_imgui/)

# glad
add_subdirectory(glad/)

# glm
add_subdirectory(glm/)

add_subdirectory(rapidjson/)

# GLFW
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw/)
