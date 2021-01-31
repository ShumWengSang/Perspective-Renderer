#ifndef CAMERA_UNIFORMS_H
#define CAMERA_UNIFORMS_H

#ifdef __cplusplus
namespace ShaderStruct {
    using namespace glm;
#endif
    struct CameraUniforms {
        mat4 ViewFromWorldMatrix;
        mat4 ProjectFromViewMatrix;
        vec4 nearFar; // x = near, y = far, z = fov, w = ratio
        vec4 camPos;
    };
#ifdef __cplusplus
}
using namespace ShaderStruct;
#endif
#endif // CAMERA_UNIFORMS_H
