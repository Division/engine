//
// Created by Sidorenko Nikita on 2018-12-30.
//

#include "StainGlass.h"
#include "objects/Projector.h"
#include "render/texture/SpriteSheet.h"

void StainGlass::start() {
  _cullingData.type = CullingData::Type::Sphere;

  vec3 scale = transform()->parent()->scale();
  vec3 size = (_mesh->aabb().max - _mesh->aabb().min) * scale;
  vec3 halfSize = size / 2.0f;
  _cullingData.sphere.radius = glm::length(halfSize);
  _cullingData.sphere.position = transform()->worldPosition() + halfSize + _mesh->aabb().min;

//  layer(1 << 5);
//  _projector->cameraVisibilityMask(~layer());
}

void StainGlass::update(float dt) {

}
