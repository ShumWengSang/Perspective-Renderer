#ifndef CAMERA_UNIFORMS_H
#define CAMERA_UNIFORMS_H

struct CameraUniforms
{
    mat4 ViewFromWorldMatrix;
    mat4 ProjectFromViewMatrix;
    vec4 nearFar; // x = near, y = far, z = fov, w = ratio
    vec4 camPos;
};

#endif // CAMERA_UNIFORMS_H
