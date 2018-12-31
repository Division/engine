//
// Created by Sidorenko Nikita on 2018-12-30.
//

#ifndef CPPWRAPPER_STAINGLASS_H
#define CPPWRAPPER_STAINGLASS_H

#include "objects/MeshObject.h"
#include "EngTypes.h"
#include <memory>

class StainGlass : public MeshObject {
public:

  void start() override;
  void update(float dt) override;
  void setSpriteSheet(SpriteSheetPtr &spritesheet) { _spritesheet = spritesheet; }

private:
  std::weak_ptr<SpriteSheet> _spritesheet;
  ProjectorPtr _projector;
};


#endif //CPPWRAPPER_STAINGLASS_H
