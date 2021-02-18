#version 460

#include <common.glsl>

#include <ShaderLocations.h>
#include <CameraUniforms.h>

in vec2 v_tex_coord;
in vec3 v_position;
in vec3 v_normal;

in vec4 v_curr_proj_pos;

PredefinedUniformBlock(CameraUniformBlock, camera);

uniform vec3 u_base_color;
uniform float u_roughness;
uniform float u_metallic;

PredefinedOutput(vec4, o_g_buffer_albedo);
PredefinedOutput(vec4, o_g_buffer_material);
PredefinedOutput(vec4, o_g_buffer_norm);

void main()
{
    o_g_buffer_albedo = vec4(u_base_color, 1.0);
    o_g_buffer_material = vec4(u_roughness, u_metallic, 1.0, 1.0);

    vec2 curr01Pos = (v_curr_proj_pos.xy / v_curr_proj_pos.w) * 0.5 + 0.5;

    vec3 normal = normalize(v_normal);
    o_g_buffer_norm = vec4(normal, 0);
}
