#pragma once

#include "PxPhysicsAPI.h"
#include "ShooterConstants.h"

class PhysicsEngine;

class Enemy {
public:
    Enemy(PhysicsEngine* engine, physx::PxVec3 position);

    physx::PxRigidDynamic* GetActor() const { return actor; }
    physx::PxVec3 GetPosition() const;
    bool IsAlive() const { return health > Const::Enemy::MIN_HEALTH; }

    void TakeDamage(float damage, const physx::PxVec3& pushDir, float pushImpulse);

private:
    float GetHealth() const { return health; }
    void ApplyImpulse(const physx::PxVec3& impulse);

    physx::PxRigidDynamic* actor;
    float health;
};