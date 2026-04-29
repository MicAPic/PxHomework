#pragma once

#include "PxPhysicsAPI.h"

class PhysicsEngine;
class Enemy;

class Grenade {
public:
    Grenade(PhysicsEngine* engine, physx::PxVec3 position, physx::PxVec3 velocity);

    void Update(float dt);
    bool ShouldExplode() const { return fuse <= 0.0f && !exploded; }
    bool IsDone() const { return exploded; }

    void Explode(PhysicsEngine* engine, Enemy* enemy);

private:
    physx::PxVec3 GetPosition() const;
    physx::PxRigidDynamic* GetActor() const { return actor; }

    physx::PxRigidDynamic* actor;
    float fuse;
    bool exploded;
};