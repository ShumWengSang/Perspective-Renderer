cmake_minimum_required(VERSION 3.17)
project(Perspective)
option(BUILD_ANIMATION "Build Animation" ON)
set(CMAKE_CXX_STANDARD 17)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")


add_subdirectory(Source/deps/)

file(GLOB SOURCES "Source/src/*.c" "Source/src/*.cpp")
# Remove the stdafx from the glob
list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Source/src/stdafx.cpp")

if(BUILD_ANIMATION)
    add_compile_definitions(ASSIMPLOADER)
    list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Source/src/BSPTree.cpp")
    list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Source/src/Collision.cpp")
    list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Source/src/CollisionTestCases.cpp")
    list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Source/src/CS350.cpp")
    list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Source/src/AssignmentFour.cpp")
    list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Source/src/AssignmentThree.cpp")
    list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Source/src/AssignmentTwo.cpp")
    list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Source/src/OctTree.cpp")
    list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/Source/src/Shapes.cpp")
endif()

# Glob the imple for imgui
file(GLOB EXT_SOURCES "Source/externalSrc/*.c" "Source/externalSrc/*.cpp" "Source/externalSrc/*.hpp")

# We sometimes want to compile files as both C++ and GLASL.
# This allows us to access files from the shader directory.
file(GLOB SHADER_CROSS_SOURCES "shaders/*.h")

message("Sources ${SOURCES}")

add_executable(Perspective ${SOURCES} ${SHADER_CROSS_SOURCES} ${EXT_SOURCES} )


source_group("Source" FILES ${SOURCES})
source_group("Cross Shaders" FILES ${SHADER_CROSS_SOURCES})

target_include_directories(Perspective PRIVATE "Source/src/")
target_include_directories(Perspective PRIVATE "shaders/")
target_include_directories(Perspective PRIVATE "Source/externalSrc/")

target_link_libraries(Perspective PRIVATE glfw)
target_link_libraries(Perspective PRIVATE glad)
target_link_libraries(Perspective PRIVATE stb_image)
target_link_libraries(Perspective PRIVATE dear_imgui)
target_link_libraries(Perspective PRIVATE glm_static)
target_link_libraries(Perspective PRIVATE tinyobjloader)
target_link_libraries(Perspective PRIVATE assimp)

# Set Perspective, assimp, and glfw output to the bin directory
set_target_properties(assimp glfw Perspective PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

################ Macro to retrieve all targets #####################
function(get_all_targets var)
    set(targets)
    get_all_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
    set(${var} ${targets} PARENT_SCOPE)
endfunction()

macro(get_all_targets_recursive targets dir)
    get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirectories})
        get_all_targets_recursive(${targets} ${subdir})
    endforeach()

    get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
    list(APPEND ${targets} ${current_targets})
endmacro()
###############################################################



target_precompile_headers(Perspective PRIVATE "Source/src/stdafx.cpp")

add_custom_command(TARGET Perspective POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Common $<TARGET_FILE_DIR:Perspective>/Common
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders/ $<TARGET_FILE_DIR:Perspective>/shaders
        )

if(MSVC)
    target_link_options(Perspective  PUBLIC /incremental PUBLIC /debug:fastlink)
    set_property(TARGET Perspective PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Perspective)
    
    # Now we can put all targets into nice folders for msvc
    get_all_targets(all_targets)
    message("All targets: ${all_targets}")
    list(REMOVE_ITEM all_targets "Perspective")
    set_target_properties(${all_targets} PROPERTIES FOLDER "External")
endif()
