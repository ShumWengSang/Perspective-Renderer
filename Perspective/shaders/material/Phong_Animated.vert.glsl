#version 460

#include <ShaderLocations.h>
#include <CameraUniforms.h>

PredefinedAttribute(vec3, a_position);
PredefinedAttribute(vec3, a_normal);
PredefinedAttribute(vec2, a_tex_coord);
PredefinedAttribute(ivec4, a_boneIDs);
PredefinedAttribute(vec4, a_weights);

PredefinedUniformBlock(CameraUniformBlock, camera);

uniform mat4 u_world_from_local;
const int MAX_BONES = 200;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 u_finalBonesMatrices[MAX_BONES];

out vec2 v_tex_coord;
out vec3 v_position;
out vec3 v_normal;

out vec4 v_curr_proj_pos;
out ivec4 v_boneID;
out vec4 v_weight;

void main()
{
    v_tex_coord = a_tex_coord;
    v_boneID = ivec4(-1);
    v_weight = ivec4(-1);
    vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(a_boneIDs[i] == -1)
            continue;
        if(a_boneIDs[i] >=MAX_BONES)
        {
            totalPosition = vec4(a_position,1.0f);
            break;
        }
        vec4 localPosition = u_finalBonesMatrices[a_boneIDs[i]] * vec4(a_position,1.0f);
        totalPosition += localPosition * a_weights[i];
        vec3 localNormal = mat3(u_finalBonesMatrices[a_boneIDs[i]]) * a_normal;
        v_boneID[i] = a_boneIDs[i];
        v_weight[i] = a_weights[i];
    }


    vec4 world_space_position = u_world_from_local * totalPosition;
    vec4 view_space_position  = camera.ViewFromWorldMatrix * world_space_position;

    vec3 view_space_normal = transpose(inverse(mat3(camera.ViewFromWorldMatrix * u_world_from_local))) * a_normal;
    // vec3 view_space_normal = mat3(camera.ViewFromWorldMatrix) * world_space_normal;

    v_position = view_space_position.xyz;
    v_normal = view_space_normal;

    gl_Position = camera.ProjectFromViewMatrix * view_space_position;

    v_curr_proj_pos = gl_Position;
}
