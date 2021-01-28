#version 460

#include <common.glsl>
#include <ShaderLocations.h>
#include <CameraUniforms.h>
#include <SceneUniforms.h>

#include <ShaderTypes.h>

in vec2 v_uv;
in vec3 v_view_ray;

PredefinedUniformBlock(SceneUniformBlock, scene);
PredefinedUniformBlock(CameraUniformBlock, camera);
PredefinedUniformBlock(DirectionalLightBlock, directionalLight);

PredefinedUniform(sampler2D, u_g_buffer_albedo);
PredefinedUniform(sampler2D, u_g_buffer_material);
PredefinedUniform(sampler2D, u_g_buffer_norm);
PredefinedUniform(sampler2D, u_g_buffer_depth);

PredefinedOutput(vec4, o_color);


void main()
{
    float depth = texture(u_g_buffer_depth, v_uv).x;

    o_color = vec4(1.0);
}
