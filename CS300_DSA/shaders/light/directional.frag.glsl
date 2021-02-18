#version 460

#include <common.glsl>
#include <ShaderLocations.h>
#include <CameraUniforms.h>
#include <SceneUniforms.h>
#include <ShaderTypes.h>

in vec2 v_uv;

PredefinedUniformBlock(SceneUniformBlock, scene);
PredefinedUniformBlock(CameraUniformBlock, camera);
PredefinedUniformBlock(DirectionalLightBlock, directionalLight);

PredefinedUniform(sampler2D, u_g_buffer_diffuse);
PredefinedUniform(sampler2D, u_g_buffer_ambient);
PredefinedUniform(sampler2D, u_g_buffer_specular);
PredefinedUniform(sampler2D, u_g_buffer_viewPos);
PredefinedUniform(sampler2D, u_g_buffer_normal);
PredefinedUniform(sampler2D, u_g_buffer_depth);

PredefinedOutput(vec4, o_color);

vec3 calculate_specular(float strength, vec3 color, vec3 view_pos, vec3 vert_pos, vec3 light_dir, vec3 normal)
{
    vec3 view_dir = normalize(view_pos - vert_pos);
    vec3 ref_dir = reflect(-light_dir, normal);

    float spec = pow(max(dot(view_dir, ref_dir), 0.0), 32.0);
    return strength * spec * color;
}

void main()
{
    const vec3 diffuse = texture(u_g_buffer_diffuse, v_uv).xyz;
    const vec3 ambient = texture(u_g_buffer_ambient, v_uv).xyz;
    const vec3 specular = texture(u_g_buffer_specular, v_uv).xyz;
    const vec3 viewPos = texture(u_g_buffer_viewPos, v_uv).xyz;
    const vec3 viewNormal = texture(u_g_buffer_normal, v_uv).xyz;
    const float shininiess = texture(u_g_buffer_specular, v_uv).w;
    const float depth = texture(u_g_buffer_depth, v_uv).r;

    vec3 light_col = vec3(1.0);
    vec3 light_dir = normalize(-directionalLight.viewDirection).xyz;
    float light_dif = max(dot(viewNormal, light_dir), 0.0);
    vec4 cameraViewPos = normalize(camera.ViewFromWorldMatrix * camera.camPos);

    vec3 view_dir = normalize(cameraViewPos.xyz - viewPos);
    vec3 reflect_dir = reflect(-light_dir, viewNormal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shininiess);

    const vec3 FinalAmbient = directionalLight.ambientColor.xyz * ambient;
    const vec3 FinalDiffuse = directionalLight.diffuseColor.xyz * diffuse * light_dif;
    const vec3 FinalSpecular = directionalLight.specularColor.xyz * light_col * specular * spec;

    vec3 result = (FinalAmbient + FinalDiffuse + FinalSpecular);
    o_color = vec4(result, 1.0);

    // o_color = vec4(0.5);
}
