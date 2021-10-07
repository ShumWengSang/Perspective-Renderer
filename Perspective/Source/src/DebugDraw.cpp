//
// Created by user on 3/28/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: DebugDraw.cpp
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Perspective
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 3/28/2021
 * End Header --------------------------------------------------------*/
#define DEBUG_DRAW_IMPLEMENTATION

#include "stdafx.h"
#include "DebugDraw.h"
#include "Scene.h"
#include "CameraBase.h"
#include "Logger.h"
#include <debug_draw.hpp>

#define errorF Log   // Empty
// ========================================================
// Minimal shaders we need for the debug primitives:
// ========================================================

const char *DDRenderInterfaceCoreGL::linePointVertShaderSrc = "\n"
                                                              "#version 150\n"
                                                              "\n"
                                                              "in vec3 in_Position;\n"
                                                              "in vec4 in_ColorPointSize;\n"
                                                              "\n"
                                                              "out vec4 v_Color;\n"
                                                              "uniform mat4 u_MvpMatrix;\n"
                                                              "\n"
                                                              "void main()\n"
                                                              "{\n"
                                                              "    gl_Position  = u_MvpMatrix * vec4(in_Position, 1.0);\n"
                                                              "    gl_PointSize = in_ColorPointSize.w;\n"
                                                              "    v_Color      = vec4(in_ColorPointSize.xyz, 1.0);\n"
                                                              "}\n";

const char *DDRenderInterfaceCoreGL::linePointFragShaderSrc = "\n"
                                                              "#version 150\n"
                                                              "\n"
                                                              "in  vec4 v_Color;\n"
                                                              "out vec4 out_FragColor;\n"
                                                              "\n"
                                                              "void main()\n"
                                                              "{\n"
                                                              "    out_FragColor = v_Color;\n"
                                                              "}\n";

const char *DDRenderInterfaceCoreGL::textVertShaderSrc = "\n"
                                                         "#version 150\n"
                                                         "\n"
                                                         "in vec2 in_Position;\n"
                                                         "in vec2 in_TexCoords;\n"
                                                         "in vec3 in_Color;\n"
                                                         "\n"
                                                         "uniform vec2 u_screenDimensions;\n"
                                                         "\n"
                                                         "out vec2 v_TexCoords;\n"
                                                         "out vec4 v_Color;\n"
                                                         "\n"
                                                         "void main()\n"
                                                         "{\n"
                                                         "    // Map to normalized clip coordinates:\n"
                                                         "    float x = ((2.0 * (in_Position.x - 0.5)) / u_screenDimensions.x) - 1.0;\n"
                                                         "    float y = 1.0 - ((2.0 * (in_Position.y - 0.5)) / u_screenDimensions.y);\n"
                                                         "\n"
                                                         "    gl_Position = vec4(x, y, 0.0, 1.0);\n"
                                                         "    v_TexCoords = in_TexCoords;\n"
                                                         "    v_Color     = vec4(in_Color, 1.0);\n"
                                                         "}\n";

const char *DDRenderInterfaceCoreGL::textFragShaderSrc = "\n"
                                                         "#version 150\n"
                                                         "\n"
                                                         "in vec2 v_TexCoords;\n"
                                                         "in vec4 v_Color;\n"
                                                         "\n"
                                                         "uniform sampler2D u_glyphTexture;\n"
                                                         "out vec4 out_FragColor;\n"
                                                         "\n"
                                                         "void main()\n"
                                                         "{\n"
                                                         "    out_FragColor = v_Color;\n"
                                                         "    out_FragColor.a = texture(u_glyphTexture, v_TexCoords).r;\n"
                                                         "}\n";

void DDRenderInterfaceCoreGL::drawPointList(const dd::DrawVertex *points, int count, bool depthEnabled) {
    assert(points != nullptr);
    assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

    glBindVertexArray(linePointVAO);
    glUseProgram(linePointProgram);

    glUniformMatrix4fv(linePointProgram_MvpMatrixLocation,
                       1, GL_FALSE, glm::value_ptr(mvpMatrix));

    if (depthEnabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    // NOTE: Could also use glBufferData to take advantage of the buffer orphaning trick...
    glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), points);

    // Issue the draw call:
    glDrawArrays(GL_POINTS, 0, count);

    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    checkGLError(__FILE__, __LINE__);
}

void DDRenderInterfaceCoreGL::drawLineList(const dd::DrawVertex *lines, int count, bool depthEnabled) {
    assert(lines != nullptr);
    assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

    glBindVertexArray(linePointVAO);
    glUseProgram(linePointProgram);

    glUniformMatrix4fv(linePointProgram_MvpMatrixLocation,
                       1, GL_FALSE, glm::value_ptr(mvpMatrix));

    if (depthEnabled) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }

    // NOTE: Could also use glBufferData to take advantage of the buffer orphaning trick...
    glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), lines);

    // Issue the draw call:
    glDrawArrays(GL_LINES, 0, count);

    glUseProgram(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    checkGLError(__FILE__, __LINE__);
}

DDRenderInterfaceCoreGL::DDRenderInterfaceCoreGL()
        : mvpMatrix(glm::mat4(1)), linePointProgram(0), linePointProgram_MvpMatrixLocation(-1), textProgram(0),
          textProgram_GlyphTextureLocation(-1), textProgram_ScreenDimensions(-1), linePointVAO(0), linePointVBO(0),
          textVAO(0), textVBO(0) {
    std::printf("\n");
    std::printf("GL_VENDOR    : %s\n", glGetString(GL_VENDOR));
    std::printf("GL_RENDERER  : %s\n", glGetString(GL_RENDERER));
    std::printf("GL_VERSION   : %s\n", glGetString(GL_VERSION));
    std::printf("GLSL_VERSION : %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    std::printf("DDRenderInterfaceCoreGL initializing ...\n");

    // Default OpenGL states:
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    // This has to be enabled since the point drawing shader will use gl_PointSize.
    glEnable(GL_PROGRAM_POINT_SIZE);

    setupShaderPrograms();
    setupVertexBuffers();

    std::printf("DDRenderInterfaceCoreGL ready!\n\n");
}

DDRenderInterfaceCoreGL::~DDRenderInterfaceCoreGL() {
    glDeleteProgram(linePointProgram);
    glDeleteProgram(textProgram);

    glDeleteVertexArrays(1, &linePointVAO);
    glDeleteBuffers(1, &linePointVBO);

    glDeleteVertexArrays(1, &textVAO);
    glDeleteBuffers(1, &textVBO);
}

void DDRenderInterfaceCoreGL::setupShaderPrograms() {
    std::printf("> DDRenderInterfaceCoreGL::setupShaderPrograms()\n");

    //
    // Line/point drawing shader:
    //
    {
        GLuint linePointVS = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(linePointVS, 1, &linePointVertShaderSrc, nullptr);
        compileShader(linePointVS);

        GLint linePointFS = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(linePointFS, 1, &linePointFragShaderSrc, nullptr);
        compileShader(linePointFS);

        linePointProgram = glCreateProgram();
        glAttachShader(linePointProgram, linePointVS);
        glAttachShader(linePointProgram, linePointFS);

        glBindAttribLocation(linePointProgram, 0, "in_Position");
        glBindAttribLocation(linePointProgram, 1, "in_ColorPointSize");
        linkProgram(linePointProgram);

        linePointProgram_MvpMatrixLocation = glGetUniformLocation(linePointProgram, "u_MvpMatrix");
        if (linePointProgram_MvpMatrixLocation < 0) {
            errorF("Unable to get u_MvpMatrix uniform location!");
        }
        checkGLError(__FILE__, __LINE__);
    }

    //
    // Text rendering shader:
    //
    {
        GLuint textVS = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(textVS, 1, &textVertShaderSrc, nullptr);
        compileShader(textVS);

        GLint textFS = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(textFS, 1, &textFragShaderSrc, nullptr);
        compileShader(textFS);

        textProgram = glCreateProgram();
        glAttachShader(textProgram, textVS);
        glAttachShader(textProgram, textFS);

        glBindAttribLocation(textProgram, 0, "in_Position");
        glBindAttribLocation(textProgram, 1, "in_TexCoords");
        glBindAttribLocation(textProgram, 2, "in_Color");
        linkProgram(textProgram);

        textProgram_GlyphTextureLocation = glGetUniformLocation(textProgram, "u_glyphTexture");
        if (textProgram_GlyphTextureLocation < 0) {
            errorF("Unable to get u_glyphTexture uniform location!");
        }

        textProgram_ScreenDimensions = glGetUniformLocation(textProgram, "u_screenDimensions");
        if (textProgram_ScreenDimensions < 0) {
            errorF("Unable to get u_screenDimensions uniform location!");
        }

        checkGLError(__FILE__, __LINE__);
    }
}

void DDRenderInterfaceCoreGL::setupVertexBuffers() {
    std::printf("> DDRenderInterfaceCoreGL::setupVertexBuffers()\n");

    //
    // Lines/points vertex buffer:
    //
    {
        glGenVertexArrays(1, &linePointVAO);
        glGenBuffers(1, &linePointVBO);
        checkGLError(__FILE__, __LINE__);

        glBindVertexArray(linePointVAO);
        glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);

        // RenderInterface will never be called with a batch larger than
        // DEBUG_DRAW_VERTEX_BUFFER_SIZE vertexes, so we can allocate the same amount here.
        glBufferData(GL_ARRAY_BUFFER, DEBUG_DRAW_VERTEX_BUFFER_SIZE * sizeof(dd::DrawVertex), nullptr, GL_STREAM_DRAW);
        checkGLError(__FILE__, __LINE__);

        // Set the vertex format expected by 3D points and lines:
        std::size_t offset = 0;

        glEnableVertexAttribArray(0); // in_Position (vec3)
        glVertexAttribPointer(
                /* index     = */ 0,
                /* size      = */ 3,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void *>(offset));
        offset += sizeof(float) * 3;

        glEnableVertexAttribArray(1); // in_ColorPointSize (vec4)
        glVertexAttribPointer(
                /* index     = */ 1,
                /* size      = */ 4,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void *>(offset));

        checkGLError(__FILE__, __LINE__);

        // VAOs can be a pain in the neck if left enabled...
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    //
    // Text rendering vertex buffer:
    //
    {
        glGenVertexArrays(1, &textVAO);
        glGenBuffers(1, &textVBO);
        checkGLError(__FILE__, __LINE__);

        glBindVertexArray(textVAO);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);

        // NOTE: A more optimized implementation might consider combining
        // both the lines/points and text buffers to save some memory!
        glBufferData(GL_ARRAY_BUFFER, DEBUG_DRAW_VERTEX_BUFFER_SIZE * sizeof(dd::DrawVertex), nullptr, GL_STREAM_DRAW);
        checkGLError(__FILE__, __LINE__);

        // Set the vertex format expected by the 2D text:
        std::size_t offset = 0;

        glEnableVertexAttribArray(0); // in_Position (vec2)
        glVertexAttribPointer(
                /* index     = */ 0,
                /* size      = */ 2,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void *>(offset));
        offset += sizeof(float) * 2;

        glEnableVertexAttribArray(1); // in_TexCoords (vec2)
        glVertexAttribPointer(
                /* index     = */ 1,
                /* size      = */ 2,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void *>(offset));
        offset += sizeof(float) * 2;

        glEnableVertexAttribArray(2); // in_Color (vec4)
        glVertexAttribPointer(
                /* index     = */ 2,
                /* size      = */ 4,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void *>(offset));

        checkGLError(__FILE__, __LINE__);

        // Ditto.
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

GLuint DDRenderInterfaceCoreGL::handleToGL(dd::GlyphTextureHandle handle) {
    const std::size_t temp = reinterpret_cast<std::size_t>(handle);
    return static_cast<GLuint>(temp);
}

dd::GlyphTextureHandle DDRenderInterfaceCoreGL::GLToHandle(const GLuint id) {
    const std::size_t temp = static_cast<std::size_t>(id);
    return reinterpret_cast<dd::GlyphTextureHandle>(temp);
}

void DDRenderInterfaceCoreGL::checkGLError(const char *file, const int line) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        errorF("%s(%d) : GL_CORE_ERROR=0x%X - ", file, line, err);
    }
}

void DDRenderInterfaceCoreGL::compileShader(const GLuint shader) {
    glCompileShader(shader);
    checkGLError(__FILE__, __LINE__);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    checkGLError(__FILE__, __LINE__);

    if (status == GL_FALSE) {
        GLchar strInfoLog[1024] = {0};
        glGetShaderInfoLog(shader, sizeof(strInfoLog) - 1, nullptr, strInfoLog);
        errorF("\n>>> Shader compiler errors:\n%s", strInfoLog);
    }
}

void DDRenderInterfaceCoreGL::linkProgram(const GLuint program) {
    glLinkProgram(program);
    checkGLError(__FILE__, __LINE__);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    checkGLError(__FILE__, __LINE__);

    if (status == GL_FALSE) {
        GLchar strInfoLog[1024] = {0};
        glGetProgramInfoLog(program, sizeof(strInfoLog) - 1, nullptr, strInfoLog);
        errorF("\n>>> Program linker errors:\n%s", strInfoLog);
    }
}

void DebugDrawSystem::Init() {
    dd::initialize(&renderInterfaceCoreGl);
}

void DebugDrawSystem::Update(const Scene &scene) {
    this->renderInterfaceCoreGl.mvpMatrix = scene.mainCamera->GetViewProjectionMatrix();
}

void DebugDrawSystem::Destroy() {
    dd::shutdown();
}

void DebugDrawSystem::Render() {
    dd::flush(0);
}
