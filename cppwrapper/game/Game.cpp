//
// Created by Sidorenko Nikita on 3/30/18.
//

#include "Game.h"
#include "objects/Sprite.h"
#include "objects/Terrain.h"
#include "objects/Projector.h"
#include "loader/HierarchyLoader.h"
#include <vector>
#include "EngMath.h"
#include "loader/TextureLoader.h"
#include "loader/SpritesheetLoader.h"
#include "EngTypes.h"
#include "render/material/MaterialTypes.h"
#include "objects/FollowCamera.h"
#include "objects/LightObject.h"
#include "render/renderer/Renderer.h"
#include "utils/Performance.h"
#include "render/renderer/SceneRenderer.h"
#include "level/Level.h"
#include "loader/ModelLoader.h"
#include "objects/PlayerController.h"


float camXAngle = 0;
float camYAngle = 0;
ModelBundlePtr bundle;

double drawTime = 0;
int decalIndex = 0;

void Game::init(std::shared_ptr<Engine> engine) {
  _engine = engine;
  _spritesheet = loader::loadSpritesheet("resources/common/decals.json");
  auto decals = loader::loadTexture("resources/common/" + _spritesheet->spritesheetName());
  engine->projectorTexture(decals);

  _scene = std::make_shared<Scene>();
  _level = std::make_shared<Level>(_scene, _spritesheet, decals);
  _level->load("resources/level/level1.mdl");
  auto characterBundle = loader::loadModel("resources/models/dwarf/dwarf.mdl");
  auto characterIdle = loader::loadModel("resources/models/dwarf/dwarf_idle.mdl");
  auto characterRun = loader::loadModel("resources/models/dwarf/dwarf_run.mdl");
  auto characterAttackLeg = loader::loadModel("resources/models/dwarf/dwarf_attack_leg.mdl");
  characterBundle->appendAnimationBundle(characterRun, "run");
  characterBundle->appendAnimationBundle(characterIdle, "idle");
  characterBundle->appendAnimationBundle(characterAttackLeg, "attack_leg");

  _player = loader::loadSkinnedMesh<PlayerController>(characterBundle);
  _player->transform()->position(vec3(50, 1, -15));
  _player->transform()->scale(vec3(0.014, 0.014, 0.014));

  _camera2D = CreateGameObject<Camera>();
  _camera2D->mode(Camera::Mode::UI);
  _camera = CreateGameObject<FollowCamera>();
  _camera->setPlayer(_player);
}

void Game::update(float dt) {
  _updateInput(dt);
  _updateGameLogic(dt);

  _scene->update(dt);
  _engine->renderScene(_scene, _camera, _camera2D);
}

void Game::_updateInput(float dt) {
  auto input = getEngine()->input();

  vec3 posDelta = vec3(0, 0, 0);

  if (input->keyDown(Key::E)) {
    posDelta += _camera->transform()->up();
  }

  if (input->keyDown(Key::Q)) {
    posDelta += _camera->transform()->down();
  }

  if (input->keyDown(Key::Equal)) {
    engine::Performance::printAverages();
  }

  if (input->keyDown(Key::Tab)) {
    _cameraControl = !_cameraControl;
    _camera->setFreeCamera(_cameraControl);
    _player->controlsEnabled(!_cameraControl);
  }

  if (input->keyDown(Key::C) && getEngine()->time() - drawTime > 0.3) {
    auto proj = CreateGameObject<Projector>();
    drawTime = getEngine()->time();
    proj->setDebugEnabled(true);
    proj->type(ProjectorType::Decal);
    proj->zFar(3);
    proj->orthographicSize(1);
    proj->isOrthographic(true);
    std::string decalName = _spritesheet->spriteNames()[decalIndex++]; decalIndex %= _spritesheet->spriteNames().size();
    if (decalName == "flashlight") { decalName = _spritesheet->spriteNames()[decalIndex++]; decalIndex %= _spritesheet->spriteNames().size(); }
    proj->spriteBounds(_spritesheet->getSpriteData(decalName).bounds);
    proj->transform()->position(_camera->transform()->position());
    proj->transform()->rotation(_camera->transform()->rotation());
  }

  if (input->keyDown(Key::MouseLeft)) {
    camXAngle -= input->mouseDelta().y * 0.008;
    camYAngle -= input->mouseDelta().x * 0.008;
  }

  _camera->transform()->translate(posDelta * dt * 20.0f);
}

void Game::_updateGameLogic(float dt) {
  auto depthTexture = getEngine()->sceneRenderer()->shadowMapDepthTexture();
  if (depthTexture) {
    _engine->debugDraw()->drawDepthImage(depthTexture, vec4(-600, -400, 300, 300), vec2(0.05f, 40.0f));
  }
}
