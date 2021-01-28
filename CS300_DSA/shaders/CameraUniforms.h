#ifndef CAMERA_UNIFORMS_H
#define CAMERA_UNIFORMS_H

struct CameraUniforms
{
    mat4 ViewFromWorldMatrix;
    mat4 ProjectFromViewMatrix;
    vec4 nearFar;
};

#endif // CAMERA_UNIFORMS_H
