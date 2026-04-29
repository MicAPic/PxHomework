#pragma once

#include <vector>
#include <memory>
#include "PxPhysicsAPI.h"

class PhysicsEngine;
class Enemy;
class Grenade;

class Player {
public:
    Player(PhysicsEngine* engine, Enemy* enemy);
    ~Player();

    void Shoot(const physx::PxVec3& origin, const physx::PxVec3& dir);
    void ThrowGrenade(const physx::PxVec3& origin, const physx::PxVec3& dir);

    void Update(float dt);

    bool HasTrail() const { return trailFramesLeft > 0; }
    void DrawTrail();

private:
    PhysicsEngine* engine;
    Enemy* enemy;
    std::vector<std::unique_ptr<Grenade>> grenades;

    physx::PxVec3 trailStart;
    physx::PxVec3 trailEnd;
    int trailFramesLeft;

    static physx::PxVec3 ApplySpread(const physx::PxVec3& dir, float halfAngle);
};
