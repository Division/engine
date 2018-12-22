//
// Created by Sidorenko Nikita on 2018-12-22.
//

#include "PlayerController.h"
#include "render/texture/Texture.h"
#include "EngTypes.h"
#include "loader/TextureLoader.h"
#include "render/material/MaterialTypes.h"
#include <memory>

TexturePtr PlayerController::diffuse;
TexturePtr PlayerController::normal;
TexturePtr PlayerController::specular;

void PlayerController::start() {
  diffuse = loader::loadTexture("resources/models/dwarf/dwarf_texture_diffuse.jpg");
  normal = loader::loadTexture("resources/models/dwarf/dwarf_texture_diffuse.jpg");
  specular = loader::loadTexture("resources/models/dwarf/dwarf_texture_diffuse.jpg");

  auto material = std::make_shared<MaterialTextureLighting>();
  material->texture(diffuse);
  _material = material;
}
