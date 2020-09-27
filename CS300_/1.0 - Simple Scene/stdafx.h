//
// Created by roland on 9/14/20.
//

#ifndef SIMPLE_SCENE_STDAFX_H
#define SIMPLE_SCENE_STDAFX_H

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>

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



// Macros
#if defined(unix) || defined(__unix__) || defined (__unix)
#define PLATFORM_UNIX
#endif

#ifdef PLATFORM_UNIX
#include <signal.h>
#define DEBUG_BREAKPOINT() raise(SIGINT)
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

#define DEBUG_MODE

#include "ObjLoader.h"
#include "Renderer/Shader.h"
#include "Renderer/Buffer.h"
#include "Renderer/Shader.h"
#include "Renderer/RendererAPI.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"

#endif //SIMPLE_SCENE_STDAFX_H
