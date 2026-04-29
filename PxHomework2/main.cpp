#include <iostream>
#include <cmath>
#include "PhysicsEngine.h"
#include "ShooterConstants.h"
#include "Enemy.h"
#include "Player.h"
#include "snippetrender/SnippetRender.h"
#include "snippetrender/SnippetCamera.h"

using namespace physx;

PhysicsEngine* physicsEngine = nullptr;
Snippets::Camera* camera = nullptr;
Player* player = nullptr;
Enemy* enemy = nullptr;

void CreateObstacles(PxMaterial* material) {
    for (const auto& def : Const::OBSTACLES) {
        PxShape* shape = physicsEngine->CreateBoxShape(def.size, material, CustomFilterData::eOBSTACLE);
        physicsEngine->AddStaticActor(shape, def.position, PxQuat(PxIdentity));
    }
}

void SetupScene() {
    PxMaterial* groundMaterial = physicsEngine->GetMaterial(0.7f, 0.6f, 0.1f);
    physicsEngine->AddGround(PxVec3(0.0f, 1.0f, 0.0f), 0.0f, groundMaterial);

    CreateObstacles(groundMaterial);

    enemy = new Enemy(physicsEngine, Const::Enemy::SPAWN_POINT);
    player = new Player(physicsEngine, enemy);
}

void keyPressedCallback(unsigned char key, const PxTransform& cameraTransform) {
    switch (toupper(key)) {
    case ' ':
        player->Shoot(cameraTransform.p, camera->getDir());
        break;
    case 'G':
        player->ThrowGrenade(cameraTransform.p, camera->getDir());
        break;
    default:
        break;
    }
}

void renderCallback() {
    physicsEngine->Simulate(Const::FIXED_DT);
    player->Update(Const::FIXED_DT);

    Snippets::startRender(camera, 0.1f, 10000.0f);

    std::vector<PxRigidActor*> actors = physicsEngine->GetActors();
    if (!actors.empty()) {
        Snippets::renderActors(actors.data(), static_cast<PxU32>(actors.size()));
    }

    if (player->HasTrail()) {
        player->DrawTrail();
    }

    Snippets::finishRender();
}

void exitCallback() {
    delete player;
    delete enemy;
    delete camera;
    delete physicsEngine;
}

int main() {
    camera = new Snippets::Camera(PxVec3(0.0f, 2.0f, -2.0f), PxVec3(0.0f, -0.05f, 1.0f));
    Snippets::setupDefault("Shooter", camera, keyPressedCallback, renderCallback, exitCallback);

    physicsEngine = new PhysicsEngine();
    SetupScene();

    std::cout << "~ Shooter ~" << std::endl;
    std::cout << "Controls: SPACE to shoot, G to throw a grenade" << std::endl;

    glutMainLoop();
    return 0;
}
