#pragma once

#define BREAKPOINT() __debugbreak()
#define Log(...) fprintf(stdout, __VA_ARGS__)
#define LogError(...) do { fprintf(stderr, __VA_ARGS__); DEBUG_BREAKPOINT();} while(false)

void GladPostCallback(const char *name, void *funcptr, int argCount, ...);
void GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
void glfwErrorCallback(int code, const char * message);