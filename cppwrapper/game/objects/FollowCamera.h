//
// Created by Sidorenko Nikita on 2018-12-23.
//

#ifndef CPPWRAPPER_FOLLOWCAMERA_H
#define CPPWRAPPER_FOLLOWCAMERA_H

#include "objects/Camera.h"
#include <memory>

class PlayerController;

class FollowCamera : public Camera {
public:
  void start() override;
  void update(float dt) override;
  void setPlayer(std::shared_ptr<PlayerController> player);

private:
  std::weak_ptr<PlayerController> _player;
  std::weak_ptr<GameObject> _container;
};


#endif //CPPWRAPPER_FOLLOWCAMERA_H
