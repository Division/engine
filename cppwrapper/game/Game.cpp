//
// Created by Sidorenko Nikita on 3/30/18.
//

#include "Game.h"
#include "objects/Sprite.h"
#include "loader/HierarchyLoader.h"
#include <vector>

GameObjectPtr rootObj;
std::shared_ptr<Sprite> sprite1;
std::shared_ptr<Sprite> sprite2;
std::shared_ptr<Sprite> sprite3;
CameraPtr camera;

LightObjectPtr light;
LightObjectPtr light2;
GameObjectPtr lightRing1;

float ang = 0;
float camXAngle = 0;
float camYAngle = 0;
ModelBundlePtr bundle;

void Game::init(Engine *engine) {
  _engine = engine;

  bundle = Resources::loadModel("resources/models/group.mdl");
  rootObj = loader::loadHierarchy(bundle);

  light = CreateGameObject<LightObject>();
  light->transform()->position(vec3(0, 4, 0));
  light->radius(20);
  light->color(vec3(0, 1, 0));
  light->enableDebug();

  light2 = CreateGameObject<LightObject>();
  light2->transform()->position(vec3(0, 4, 0));
  light2->radius(30);
  light2->color(vec3(1, 0, 0));
  light2->enableDebug();

  lightRing1 = CreateGameObject<GameObject>();
  lightRing1->transform()->rotation(glm::angleAxis((float)M_PI / 2, vec3(0, 1, 0)));
  lightRing1->transform()->position(vec3(1.2, 0, 1));
  int ringCount = 10;
  for (int i = 0; i < ringCount; i++) {
    auto lightInRing = CreateGameObject<LightObject>();
    float ang = M_PI * 2 * i / 10.0f;
    lightInRing->transform()->position(vec3(cosf(ang) * 4, 0, sinf(ang) * 4));
    lightInRing->radius(7);
    lightInRing->color(vec3(0, 1, 1));
    lightInRing->enableDebug();
    lightInRing->transform()->parent(lightRing1->transform());
  }

  camera = CreateGameObject<Camera>();
  camera->transform()->position(vec3(0, 0, 5));

  sprite1 = CreateGameObject<Sprite>();
  sprite1->transform()->position(vec3(0, 0, -10));
  sprite1->transform()->scale(vec3(0.3, 0.3, 0.3));
  sprite1->materialColor()->color(vec4(1, 1, 0, 1));

  sprite2 = CreateGameObject<Sprite>();
  sprite2->transform()->position(vec3(0, 8, 0));
  sprite2->transform()->scale(vec3(2, 2, 2));
  sprite2->transform()->setParent(sprite1->transform());

  sprite3 = CreateGameObject<Sprite>();
  sprite3->transform()->position(vec3(3, 0, 0));
  sprite3->transform()->scale(vec3(0.5, 0.5, 0.5));
  sprite3->transform()->setParent(sprite2->transform());
  sprite3->materialColor()->color(vec4(1, 0, 1, 1));
}

void Game::update(float dt) {
  _updateInput(dt);
  _updateGameLogic(dt);

  _scene.update(dt);
  _engine->renderScene(_scene);
}

void Game::_updateInput(float dt) {
  auto input = getEngine()->input();

  vec3 posDelta = vec3(0, 0, 0);
  if (input->keyDown(Key::Left) || input->keyDown(Key::A)) {
    posDelta += camera->transform()->left();
  }

  if (input->keyDown(Key::Right) || input->keyDown(Key::D)) {
    posDelta += camera->transform()->right();
  }

  if (input->keyDown(Key::Up) || input->keyDown(Key::W)) {
    posDelta += camera->transform()->forward();
  }

  if (input->keyDown(Key::Down) || input->keyDown(Key::S)) {
    posDelta += camera->transform()->backward();
  }

  if (input->keyDown(Key::E)) {
    posDelta += camera->transform()->up();
  }

  if (input->keyDown(Key::Q)) {
    posDelta += camera->transform()->down();
  }

  if (input->keyDown(Key::Space)) {
    rootObj->transform()->rotate(vec3(0, 0, 1), dt * PI);
  }

  if (input->keyDown(Key::MouseLeft)) {
    camXAngle -= input->mouseDelta().y * 0.008;
    camYAngle -= input->mouseDelta().x * 0.008;
    quat rotation(vec3(camXAngle, camYAngle, 0));
    camera->transform()->rotation(rotation);
  }

  camera->transform()->translate(posDelta * dt * 20.0f);
}

void Game::_updateGameLogic(float dt) {
  ang += dt * PI;
  sprite2->materialColor()->color(vec4((sin(ang) + 1) / 2, (cos(ang) + 1) / 2, cos(ang * 0.5) + sin(ang * 0.2), 1));
  sprite1->transform()->rotate(vec3(0, 0, 1), dt * PI);
  sprite2->transform()->rotate(vec3(0, 0, 1), dt * PI * 2);

  light->transform()->setPosition(vec3(cos(ang) * 5, 3, sin(ang) * 5));
  lightRing1->transform()->rotate(vec3(0, 1, 0), dt * PI * 0.2);
}
