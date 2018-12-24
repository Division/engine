//
// Created by Sidorenko Nikita on 2018-12-23.
//

#include "FollowCamera.h"
#include "PlayerController.h"

void FollowCamera::start() {
  auto container = CreateGameObject<GameObject>();
  transform()->parent(container->transform());
  transform()->position(vec3(0, 0, 20));

  container->transform()->rotate(vec3(0, 1, 0), RAD(225));
  container->transform()->rotate(vec3(1, 0, 0), RAD(-45));

  _container = container;
}

void FollowCamera::setPlayer(std::shared_ptr<PlayerController> player) {
  _player = player;
}

void FollowCamera::update(float dt) {
  auto player = _player.lock();
  if (!player) { return; }

  _container.lock()->transform()->position(player->transform()->position() + vec3(0, 1, 0));
}
