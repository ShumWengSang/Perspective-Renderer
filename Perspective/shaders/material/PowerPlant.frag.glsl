#version 460

#include <common.glsl>

#include <ShaderLocations.h>
#include <CameraUniforms.h>

in vec2 v_tex_coord;
in vec3 v_position;
in vec3 v_normal;

in vec4 v_curr_proj_pos;

PredefinedUniformBlock(CameraUniformBlock, camera);

uniform vec3 u_mat_ambient;
uniform vec3 u_mat_diffuse;
uniform vec3 u_mat_specular;
uniform float u_mat_shininess;


PredefinedOutput(vec3, o_g_buffer_diffuse);
PredefinedOutput(vec3, o_g_buffer_ambient);
PredefinedOutput(vec4, o_g_buffer_specular);
PredefinedOutput(vec3, o_g_buffer_viewPos)
PredefinedOutput(vec3, o_g_buffer_normal);

void main()
{
    o_g_buffer_diffuse = u_mat_diffuse;
    o_g_buffer_ambient = u_mat_ambient;
    o_g_buffer_specular = vec4(u_mat_specular, u_mat_shininess);
    o_g_buffer_viewPos = v_position;
    o_g_buffer_normal = normalize(v_normal);
}
