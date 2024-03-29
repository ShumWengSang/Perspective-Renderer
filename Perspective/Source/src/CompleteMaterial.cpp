#include "stdafx.h"
#include "CompleteMaterial.h"
#include "ShaderSystem.h"
#include "TransformSystem.h"
#include "TextureSystem.h"

CompleteMaterial::CompleteMaterial() {
    ShaderSystem::getInstance().AddProgram("material/complete", this);
}

void CompleteMaterial::ProgramLoaded(GLuint program) {
    this->program = program;
    if (program) {
        modelMatrixLocation = glGetUniformLocation(program, "u_world_from_local");
        prevModelMatrixLocation =
                glGetUniformLocation(program, "u_prev_world_from_local");
        normalMatrixLocation =
                glGetUniformLocation(program, "u_world_from_tangent");

        baseColorLocation = glGetUniformLocation(program, "u_base_color");
        roughnessMapLocation = glGetUniformLocation(program, "u_roughness_map");
        metallicMapLocation = glGetUniformLocation(program, "u_metallic_map");
        normalMapLocation = glGetUniformLocation(program, "u_normal_map");
    }
}

void CompleteMaterial::BindUniforms(
        Transform &transform, const Transform &prevTransform
                                   ) const {
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE,
                       glm::value_ptr(transform.matrix));
    glUniformMatrix4fv(prevModelMatrixLocation, 1, GL_FALSE,
                       glm::value_ptr(prevTransform.matrix));
    glUniformMatrix3fv(normalMatrixLocation, 1, GL_FALSE,
                       glm::value_ptr(transform.normalMatrix));

    if (!baseColorTexture) {
        baseColorTexture =
                TextureSystem::getInstance().LoadDataTexture("Common/default/base_color.png");
    }

    const GLuint baseColorUnit = 0;
    glBindTextureUnit(baseColorUnit, baseColorTexture);
    glUniform1i(baseColorLocation, baseColorUnit);

    if (!normalMap) {
        normalMap = TextureSystem::getInstance().LoadDataTexture(
                "Common/default/normal.png");
    }

    const GLuint normalMapUnit = 1;
    glBindTextureUnit(normalMapUnit, normalMap);
    glUniform1i(normalMapLocation, normalMapUnit);

    const GLuint roughnessMapUnit = 2;
    glBindTextureUnit(roughnessMapUnit, roughnessMap);
    glUniform1i(roughnessMapLocation, roughnessMapUnit);

    const GLuint metallicMapUnit = 3;
    glBindTextureUnit(metallicMapUnit, metallicMap);
    glUniform1i(metallicMapLocation, metallicMapUnit);
}
