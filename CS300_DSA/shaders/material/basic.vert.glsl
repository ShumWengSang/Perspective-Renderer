#version 460

#include <ShaderLocations.h>
#include <CameraUniforms.h>

PredefinedAttribute(vec3, a_position);
PredefinedAttribute(vec3, a_normal);
PredefinedAttribute(vec2, a_tex_coord);

PredefinedUniformBlock(CameraUniformBlock, camera);

uniform mat4 u_world_from_local;

out vec2 v_tex_coord;
out vec3 v_position;
out vec3 v_normal;

out vec4 v_curr_proj_pos;

void main()
{
    v_tex_coord = a_tex_coord;

    vec4 world_space_position = u_world_from_local * vec4(a_position, 1.0);
    vec4 view_space_position  = camera.view_from_world * world_space_position;

    vec3 world_space_normal = transpose(inverse(u_world_from_local)) * a_normal;
    vec3 view_space_normal = mat3(camera.view_from_world) * world_space_normal;

    v_position = view_space_position.xyz;
    v_normal = view_space_normal;

    gl_Position = camera.projection_from_view * view_space_position;

    v_curr_proj_pos = gl_Position;
}
