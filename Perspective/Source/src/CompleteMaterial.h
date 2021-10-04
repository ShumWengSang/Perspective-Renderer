#pragma once
#include "Material.h"

class CompleteMaterial : public Material {
 public:
  CompleteMaterial();
  ~CompleteMaterial() = default;

  mutable GLuint baseColorTexture{};
  mutable GLuint normalMap{};
  mutable GLuint roughnessMap{};
  mutable GLuint metallicMap{};

  void ProgramLoaded(GLuint program) override;
  void BindUniforms(Transform& transform,
                    const Transform& prevTransform) const override;

 private:
  GLint baseColorLocation;
  GLint roughnessMapLocation;
  GLint metallicMapLocation;
  GLint normalMapLocation;

  GLint modelMatrixLocation;
  GLint prevModelMatrixLocation;
  GLint normalMatrixLocation;
};