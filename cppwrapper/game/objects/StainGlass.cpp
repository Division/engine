//
// Created by Sidorenko Nikita on 2018-12-30.
//

#include "StainGlass.h"
#include "objects/Projector.h"
#include "render/texture/SpriteSheet.h"
#include "render/material/MaterialTypes.h"
#include <memory>
#include "render/texture/SpriteSheet.h"

void StainGlass::start() {
  _cullingData.type = CullingData::Type::Sphere;

  vec3 scale = transform()->parent()->scale();
  vec3 size = (_mesh->aabb().max - _mesh->aabb().min) * scale;
  vec3 halfSize = size / 2.0f;
  _cullingData.sphere.radius = glm::length(halfSize);
  _cullingData.sphere.position = transform()->worldPosition() + halfSize + _mesh->aabb().min * scale;
}

void StainGlass::setup(SpriteSheetPtr &spritesheet, TexturePtr &decals, const std::string &spriteName) {
  auto material = std::make_shared<MaterialTexture>();
  material->texture(decals);
  auto &data = spritesheet->getSpriteData(spriteName);
  _objectParamsStruct.uvOffset = vec2(data.bounds.x, data.bounds.y);
  _objectParamsStruct.uvScale = vec2(data.bounds.width, data.bounds.height);
  this->material(material);
}
