//
// Created by Sidorenko Nikita on 3/30/18.
//

#ifndef CPPWRAPPER_GAME_H
#define CPPWRAPPER_GAME_H

#include "EngineMain.h"
#include <memory>

class Level;
class PlayerController;
class FollowCamera;

class Game: public IGame {

public:
  void init(std::shared_ptr<Engine> engine) override;
  void update(float dt) override;

private:
  bool _cameraControl = false;
  std::shared_ptr<Engine> _engine = nullptr;
  SpriteSheetPtr _spritesheet;
  std::shared_ptr<Scene> _scene;
  std::shared_ptr<Level> _level;
  std::shared_ptr<PlayerController> _player;
  CameraPtr _camera2D;
  std::shared_ptr<FollowCamera> _camera;

  float _inputInterval = 0;

private:
  void _updateGameLogic(float dt);

  void _updateInput(float dt);
};


#endif //CPPWRAPPER_GAME_H
