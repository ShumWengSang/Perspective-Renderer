#version 460

#include <common.glsl>
#include <ShaderLocations.h>
#include <CameraUniforms.h>

in vec2 v_uv;
in vec3 ray;

PredefinedUniformBlock(CameraUniformBlock, camera);

PredefinedUniform(sampler2D, u_texture);
PredefinedUniform(samplerCube , u_textureCube);
PredefinedUniform(sampler2D, u_g_buffer_depth);

PredefinedOutput(vec4, o_color);

uniform int copyDepthBuffer;
out float gl_FragDepth;

void main()
{
    const vec3 color = texture(u_texture, v_uv).rgb;
    const float depth = texture(u_g_buffer_depth, v_uv).r;
    if(depth == 1.0)
    {
        vec3 viewRay = normalize(ray);
        // Is the -viewRay a hack?
        o_color = texture(u_textureCube, -viewRay);
    }
    else
    {
        o_color = vec4(color, 1.0);
    }
    if(copyDepthBuffer == 1)
        gl_FragDepth = depth;
    else
        gl_FragDepth = 1.0f;
}
