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
#include "objects/Camera.h"
#include "objects/LightObject.h"
#include "render/renderer/Renderer.h"
#include "utils/Performance.h"
#include "render/renderer/SceneRenderer.h"
#include "level/Level.h"
#include "loader/ModelLoader.h"
#include "objects/PlayerController.h"

TexturePtr texture1;

GameObjectPtr rootObj2;

std::shared_ptr<Sprite> sprite1;
std::shared_ptr<Sprite> sprite2;
std::shared_ptr<Sprite> sprite3;
CameraPtr camera;
CameraPtr camera2D;

TerrainPtr terrain;

SpriteSheetPtr spritesheet;

LightObjectPtr spotLight;
LightObjectPtr light;
LightObjectPtr light2;
GameObjectPtr lightRing1;
ProjectorPtr flashLight;
MaterialTextureProjectionPtr materialTexProj;
//ProjectorPtr projector;

float ang = 0;
float camXAngle = 0;
float camYAngle = 0;
ModelBundlePtr bundle;

mat4 projMatrix;
double drawTime = 0;
int decalIndex = 0;

void Game::init(std::shared_ptr<Engine> engine) {
  _engine = engine;
  _scene = std::make_shared<Scene>();
  _level = std::make_shared<Level>(_scene);
  _level->load("resources/level/level1.mdl");
  auto characterBundle = loader::loadModel("resources/models/dwarf/dwarf.mdl");
  auto characterIdle = loader::loadModel("resources/models/dwarf/dwarf_idle.mdl");
  auto characterRun = loader::loadModel("resources/models/dwarf/dwarf_run.mdl");
  auto characterAttackLeg = loader::loadModel("resources/models/dwarf/dwarf_attack_leg.mdl");
  characterBundle->appendAnimationBundle(characterRun, "run");
  characterBundle->appendAnimationBundle(characterIdle, "idle");
  characterBundle->appendAnimationBundle(characterAttackLeg, "attack_leg");

  camera2D = CreateGameObject<Camera>();
  camera2D->mode(Camera::Mode::UI);
  camera = CreateGameObject<Camera>();
  camera->transform()->position(vec3(0, 5, 15));
//  camera2D->transform()->position(vec3(0, 5, 0));
  camXAngle = -M_PI / 8;

  spritesheet = loader::loadSpritesheet("resources/common/decals.json");
  auto projectorTexture = loader::loadTexture("resources/common/" + spritesheet->spritesheetName(), true);
  engine->projectorTexture(projectorTexture);

  bundle = Resources::loadModel("resources/models/girl.mdl");

//  bundle = Resources::loadModel("resources/models/skin_cilynder.mdl");

  _player = loader::loadSkinnedMesh<PlayerController>(characterBundle);
  _player->transform()->position(vec3(0, 1, -15));
  _player->transform()->scale(vec3(0.02, 0.02, 0.02));

  flashLight = CreateGameObject<Projector>();
//  flashLight->transform()->parent(camera->transform());
  flashLight->type(ProjectorType::Projector);
  flashLight->zFar(40);
  flashLight->zNear(0.05);
  flashLight->attenuation(0.0, 0.01);
  flashLight->fov(50);
  flashLight->castShadows(true);
//  flashLight->transform()->position(vec3(0,0,-0.1));
  flashLight->spriteBounds(spritesheet->getSpriteData("flashlight").bounds);
}

void Game::update(float dt) {
  _updateInput(dt);
  _updateGameLogic(dt);

  _scene->update(dt);
  _engine->renderScene(_scene, camera, camera2D);
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

  if (input->keyDown(Key::Equal)) {
    engine::Performance::printAverages();
  }

  if (input->keyDown(Key::Space)) {
    flashLight->transform()->position(camera->transform()->position());
    flashLight->transform()->rotation(camera->transform()->rotation());
  }

  if (input->keyDown(Key::X)) {
    _player->animation()->play("idle", true);
  }

  if (input->keyDown(Key::Z)) {
    _player->animation()->play("run", true);
  }

  if (input->keyDown(Key::C) && getEngine()->time() - drawTime > 0.3) {
    auto proj = CreateGameObject<Projector>();
    drawTime = getEngine()->time();
    proj->setDebugEnabled(true);
    proj->type(ProjectorType::Decal);
    proj->zFar(3);
    proj->orthographicSize(1);
    proj->isOrthographic(true);
    std::string decalName = spritesheet->spriteNames()[decalIndex++]; decalIndex %= spritesheet->spriteNames().size();
    if (decalName == "flashlight") { decalName = spritesheet->spriteNames()[decalIndex++]; decalIndex %= spritesheet->spriteNames().size(); }
    proj->spriteBounds(spritesheet->getSpriteData(decalName).bounds);
    proj->transform()->position(camera->transform()->position());
    proj->transform()->rotation(camera->transform()->rotation());
  }

  if (input->keyDown(Key::MouseLeft)) {
    camXAngle -= input->mouseDelta().y * 0.008;
    camYAngle -= input->mouseDelta().x * 0.008;
  }

  camera->transform()->translate(posDelta * dt * 20.0f);
}

void Game::_updateGameLogic(float dt) {
//  dt *= 0.05;
  ang += dt * PI;

//  _engine->debugDraw()->drawImage(texture1, vec4(150, 150, 300, 300));

  auto depthTexture = getEngine()->sceneRenderer()->shadowMapDepthTexture();
  if (depthTexture) {
    _engine->debugDraw()->drawDepthImage(depthTexture, vec4(-600, -400, 300, 300), vec2(0.05f, 40.0f));
  }

  quat rotation(vec3(camXAngle, camYAngle, 0));
  camera->transform()->rotation(rotation);

//  player->transform()->rotate(vec3(0, 1, 0), dt * PI * 0.3);

//  light->transform()->setPosition(vec3(cos(ang) * 9, 3, sin(ang) * 9));
//  lightRing1->transform()->rotate(vec3(0, 1, 0), dt * PI * 0.2);

  auto debugDraw = getEngine()->debugDraw();
  debugDraw->drawFrustum(projMatrix, glm::vec4(1, 1, 0, 1));

//  OBB obb(vec3(10, 0, 5), vec3(6, 3, 1));
//  debugDraw->drawOBB(obb, vec4(1, 0, 1, 1));
//  auto debugDraw = getEngine()->debugDraw();
//  debugDraw->drawFrustum(projMatrix);
}
