#version 460

//
// NOTE: This requires no attributes! Just call
//
//    glDrawArrays(GL_TRIANGLES, 0, 3)
//
// with a valid but "empty" VAO bound
//
#include <CameraUniforms.h>
#include <ShaderLocations.h>
uniform mat4 u_world_from_local;

PredefinedUniformBlock(CameraUniformBlock, camera);
PredefinedAttribute(vec3, a_position);


void main()
{
    vec4 world_space_position = u_world_from_local * vec4(a_position, 1.0);
    vec4 view_space_position  = camera.ViewFromWorldMatrix * world_space_position;
    gl_Position = camera.ProjectFromViewMatrix * view_space_position;
}
