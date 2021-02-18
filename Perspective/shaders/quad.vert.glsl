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

out vec2 v_uv;
out vec3 ray;

PredefinedUniformBlock(CameraUniformBlock, camera);

vec3 skyray(vec2 texcoord, float fovy, float aspect)
{
    float d = 0.5 / tan(fovy / 2.0);
    return normalize(vec3((texcoord.x - 0.5) * aspect, texcoord.y - 0.5, -d));
}

vec3 getViewDirection(mat4 viewFromProj, mat4 worldFromView)
{
    vec4 viewSpacePos = viewFromProj * gl_Position;
    viewSpacePos.xyz /= viewSpacePos.w;
    return mat3(worldFromView) * viewSpacePos.xyz;
}

void main()
{
    // Defines a full screen triangle with CW vertex ordering
    v_uv = vec2(gl_VertexID & 2, (gl_VertexID << 1) & 2);
    gl_Position = vec4(v_uv * 2.0 - 1.0, 0.0, 1.0);

    ray = getViewDirection(inverse(camera.ProjectFromViewMatrix), inverse(camera.ViewFromWorldMatrix));
}
