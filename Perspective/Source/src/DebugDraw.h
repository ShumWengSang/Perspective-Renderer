//
// Created by user on 3/28/2021.
//

/* Start Header -------------------------------------------------------
 * Copyright (C) 2020 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the prior
 * written consent of DigiPen Institute of Technology is prohibited.
 * File Name: DebugDraw.h
 * Purpose: Lorem ipsum dolor sit amet, consectetur adipiscing elit.
 * Language: C++, G++
 * Platform: g++ (Ubuntu 9.3.0-10ubuntu2) 9.3, ThinkPad T430u, Nvidia GT 620M,
 *           OpenGL version string: 4.6.0 NVIDIA 390.138
 * Project: Perspective
 * Author: Roland Shum, roland.shum@digipen.edu
 * Creation date: 3/28/2021
 * End Header --------------------------------------------------------*/


#ifndef OPENGLFRAMEWORK_DEBUGDRAW_H
#define OPENGLFRAMEWORK_DEBUGDRAW_H

class DDRenderInterfaceCoreGL final
        : public dd::RenderInterface {
public:

    //
    // dd::RenderInterface overrides:
    //

    void drawPointList(const dd::DrawVertex *points, int count, bool depthEnabled) override;

    void drawLineList(const dd::DrawVertex *lines, int count, bool depthEnabled) override;

    // These two can also be implemented to perform GL render
    // state setup/cleanup, but we don't use them in this sample.
    //void beginDraw() override { }
    //void endDraw()   override { }

    //
    // Local methods:
    //

    DDRenderInterfaceCoreGL();

    ~DDRenderInterfaceCoreGL() override;

    void setupShaderPrograms();

    void setupVertexBuffers();

    static GLuint handleToGL(dd::GlyphTextureHandle handle);

    static dd::GlyphTextureHandle GLToHandle(const GLuint id);

    static void checkGLError(const char *file, const int line);

    static void compileShader(const GLuint shader);

    static void linkProgram(const GLuint program);

    // The "model-view-projection" matrix for the scene.
    // In this demo, it consists of the camera's view and projection matrices only.
    glm::mat4 mvpMatrix;

private:

    GLuint linePointProgram;
    GLint linePointProgram_MvpMatrixLocation;

    GLuint textProgram;
    GLint textProgram_GlyphTextureLocation;
    GLint textProgram_ScreenDimensions;

    GLuint linePointVAO;
    GLuint linePointVBO;

    GLuint textVAO;
    GLuint textVBO;

    static const char *linePointVertShaderSrc;
    static const char *linePointFragShaderSrc;

    static const char *textVertShaderSrc;
    static const char *textFragShaderSrc;

}; // class DDRenderInterfaceCoreGL

class Scene;

class DebugDrawSystem {
public:
    static DebugDrawSystem &getInstance() {
        static DebugDrawSystem instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    void Init();

    // Must be called before every frame
    void Update(Scene const &scene);

    void Render();

    void Destroy();

private:
    DDRenderInterfaceCoreGL renderInterfaceCoreGl;
};


#endif //OPENGLFRAMEWORK_DEBUGDRAW_H
