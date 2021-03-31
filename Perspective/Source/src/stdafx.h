#pragma once
#define _CRT_SECURE_NO_WARNINGS

// Imgui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Glad, GLFW, and OpenGL
#include <glad/glad.h>
#include <GLFW/glfw3.h>


// GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/hash.hpp>

// Tiny obj loader
#include <tiny_obj_loader.h>

#include <stb_image.h>

// Todo fix this
#include "../deps/rapidjson/include/rapidjson/rapidjson.h"

// STL libs
#include <string>
#include <fstream>
#include <utility>
#include <sstream>
#include <iomanip>
#include <unordered_set>
#include <filesystem>
#include <vector>
#include <functional>
#include <map>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <array>
#include <cassert>
#include <atomic>
#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <thread>
#include <iostream>
#include <mutex>
#include <istream>
#include <type_traits>
#include <condition_variable>
#include <tuple>
#include <type_traits>

#include <debug_draw.hpp>

// Macros for defining platform
#if defined(unix) || defined(__unix__) || defined (__unix)
#define PLATFORM_UNIX
#endif

#ifdef PLATFORM_UNIX
#include <signal.h>
#define DEBUG_BREAKPOINT() raise(SIGINT)
#else
#define DEBUG_BREAKPOINT() __debugbreak()
#endif

// Our own classes
#include "Queue.h"