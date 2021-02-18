#include "stdafx.h"
#include "Logger.h"

void GLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    Log("GL debug message: %s\n", message);
    DEBUG_BREAKPOINT();
}

void GladPostCallback(const char *name, void *funcptr, int argCount, ...)
{
    // Use the "raw" glad_glGetError to avoid an recursive loop
    GLenum errorCode = glad_glGetError();

    if (errorCode != GL_NO_ERROR)
    {
        const char *errorName;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:
                errorName = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                errorName = "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                errorName = "GL_INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                errorName = "GL_OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                errorName = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            default:
                errorName = "UnknownError";
                break;
        }

        LogError("%s error (0x%03x) in '%s'\n", errorName, errorCode, name);
    }
}

void glfwErrorCallback(int code, const char * message)
{
    LogError("GLFW error %d: %s\n", code, message);
}

