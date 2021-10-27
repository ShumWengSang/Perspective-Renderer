#version 460

#include <ShaderLocations.h>
#include <CameraUniforms.h>
#include <Quaternion.h>

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

layout(std140, binding = PredefinedUniformBlockBinding(VQSUniformBlock)) uniform vqsBlock
{ 
    VQS vqs[MAX_BONES];
};

vec3 RotateCustom(in int index, in vec3 p){
    vec4 q = vqs[index].q;
    
    vec3 v = vqs[index].v;
    float s = vqs[index].s;
    
    return s * rotate_vector(q, p) + v;
}

mat4 translate(mat4 m, vec3 v)
{
    mat4 res = mat4(m);
    res[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
    return res;
}

mat4 scale(mat4 m, vec3 s)
{
    mat4 Result;
    Result[0] = m[0] * s[0];
    Result[1] = m[1] * s[1];
    Result[2] = m[2] * s[2];
    Result[3] = m[3];
    return Result;
}

mat4 QuaternionToMat4(in vec4 q){
    mat4 result = mat4(1.0f);
    vec4 norm = q;
    result[0][0] = 1 - 2 * (norm.y * norm.y + norm.z * norm.z);
    result[1][0] = 2 * (norm.x * norm.y - norm.z * norm.s);
    result[2][0] = 2 * (norm.x * norm.z + norm.y * norm.s);

    result[0][1] = 2 * (norm.x * norm.y + norm.s * norm.z);
    result[1][1] = 1 - 2 * (norm.x * norm.x + norm.z * norm.z);
    result[2][1] = 2 * (norm.y * norm.z - norm.s * norm.x);

    result[0][2] = 2 * (norm.x * norm.z - norm.s * norm.y);
    result[1][2] = 2 * (norm.y * norm.z + norm.s * norm.x);
    result[2][2] = 1 - 2 * (norm.x * norm.x + norm.y * norm.y);

    return result;
}

mat4 VQSToMat4(in int index){
    mat4 r = QuaternionToMat4(normalize(vqs[index].q));
    mat4 t = translate(mat4(1.0), vqs[index].v);
    mat4 s = scale(mat4(1.0), vec3(vqs[index].s, vqs[index].s, vqs[index].s));
    // Scale first, then rotate, then translate
    return t * r * s;
}

out vec2 v_tex_coord;
out vec3 v_position;
out vec3 v_normal;

out vec4 v_curr_proj_pos;

void main()
{
    v_tex_coord = a_tex_coord;
    vec4 totalPosition = vec4(0.0);
    vec3 totalNormal = vec3(0.0);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(a_boneIDs[i] == -1)
            continue;
        if(a_boneIDs[i] >=MAX_BONES)
        {
            totalPosition = vec4(a_position,1.0f);
            break;
        }
        // vec4 localPosition = u_finalBonesMatrices[a_boneIDs[i]] * vec4(a_position,1.0f);
        vec4 localPosition = vec4(RotateCustom(a_boneIDs[i], a_position), 1.0f);
        totalPosition += localPosition * a_weights[i];
         vec3 localNormal = mat3(u_finalBonesMatrices[a_boneIDs[i]]) * a_normal;
        // vec3 localNormal = mat3(VQSToMat4(a_boneIDs[i]))* a_normal;
        totalNormal += localNormal * a_weights[i];
    }


    vec4 world_space_position = u_world_from_local * totalPosition;
    vec4 view_space_position  = camera.ViewFromWorldMatrix * world_space_position;

    vec3 view_space_normal = transpose(inverse(mat3(camera.ViewFromWorldMatrix * u_world_from_local))) *
    normalize(vec3(totalNormal));
    // vec3 view_space_normal = mat3(camera.ViewFromWorldMatrix) * world_space_normal;

    v_position = view_space_position.xyz;
    v_normal = view_space_normal;

    gl_Position = camera.ProjectFromViewMatrix * view_space_position;

    v_curr_proj_pos = gl_Position;
}
