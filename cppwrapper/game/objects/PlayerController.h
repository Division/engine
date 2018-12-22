//
// Created by Sidorenko Nikita on 2018-12-22.
//

#ifndef CPPWRAPPER_PLAYERCONTROLLER_H
#define CPPWRAPPER_PLAYERCONTROLLER_H

#include "objects/SkinnedMeshObject.h"

class PlayerController : public SkinnedMeshObject {

public:
  static TexturePtr diffuse;
  static TexturePtr normal;
  static TexturePtr specular;

public:
  void start() override;
};


#endif //CPPWRAPPER_PLAYERCONTROLLER_H
