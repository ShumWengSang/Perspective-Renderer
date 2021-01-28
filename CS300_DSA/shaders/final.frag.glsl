#version 460

#include <common.glsl>
#include <ShaderLocations.h>
#include <CameraUniforms.h>

in vec2 v_uv;

PredefinedUniformBlock(CameraUniformBlock, camera);

PredefinedUniform(sampler2D, u_texture);

PredefinedOutput(vec4, o_color);

void main()
{
    vec3 color = texture(u_texture, v_uv).rgb;

    o_color = vec4(color, 1.0);
}
