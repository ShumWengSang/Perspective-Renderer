//
// Created by roland on 9/14/20.
//

#if _WIN32
#define _CRT_SECURE_NO_WARNINGS
#pragma warning (disable : 4068 ) /* disable unknown pragma warnings */
#endif

#ifndef SIMPLE_SCENE_STDAFX_H
#define SIMPLE_SCENE_STDAFX_H

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"



#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#endif

// GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>

// STL lib
#include <vector>
#include <optional>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <algorithm>
#include <cmath>
#include <set>
#include <exception>
#include <array>
#include <sstream>
#include <tuple>

#include "Affine.h"

struct KeyCompare
{
    bool operator()(const glm::vec3 & a, const glm::vec3 & b) const
    {
        return a.x < b.x ||
                (Math::near(a.x, b.x) && a.y < b.y)||
                (Math::near(a.x, b.x) && Math::near(a.y, b.y) && a.z < b.z);
    }
};

using Vec3Set = std::set<glm::vec3, KeyCompare>;


// Macros
#if defined(unix) || defined(__unix__) || defined (__unix)
#define PLATFORM_UNIX
#endif

#ifdef PLATFORM_UNIX
#include <signal.h>
#define DEBUG_BREAKPOINT() raise(SIGINT)
#else
#define DEBUG_BREAKPOINT() __debugbreak() 
#endif

template <typename T>
using SharedPtr = std::shared_ptr<T>;
template <typename T, typename ... Args>
constexpr SharedPtr<T> CreateSharedPtr(Args&& ... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T>
using UniquePtr = std::unique_ptr<T>;
template <typename T, typename ... Args>
constexpr UniquePtr<T> CreateUniquePtr(Args&& ... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using WeakPtr = std::weak_ptr<T>;

inline const float PI = 3.14159265359f;

#define DEBUG_MODE
// #define DEBUG_OUT

#endif //SIMPLE_SCENE_STDAFX_H
