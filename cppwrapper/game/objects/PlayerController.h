//
// Created by Sidorenko Nikita on 2018-12-22.
//

#ifndef CPPWRAPPER_PLAYERCONTROLLER_H
#define CPPWRAPPER_PLAYERCONTROLLER_H

#include "objects/SkinnedMeshObject.h"
#include "EngTypes.h"


class PlayerController : public SkinnedMeshObject {
public:
  static TexturePtr diffuse;
  static TexturePtr normal;
  static TexturePtr specular;

  enum class State : int {

  };

public:
  void start() override;
  void update(float dt) override;

private:
  AnimationPlaybackPtr _runPlayback;
  AnimationPlaybackPtr _idlePlayback;
  vec3 _speed = vec3(0);
  vec3 _acceleration = vec3(0);
  LightObjectPtr _topLight; // spot light always above the player
};


#endif //CPPWRAPPER_PLAYERCONTROLLER_H
