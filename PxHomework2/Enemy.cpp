#include "Enemy.h"
#include "PhysicsEngine.h"
#include "ShooterConstants.h"
#include <iostream>

using namespace physx;

Enemy::Enemy(PhysicsEngine* engine, PxVec3 position) : health(Const::Enemy::MAX_HEALTH) {
    PxMaterial* material = engine->GetMaterial(0.6f, 0.5f, 0.1f);

    PxQuat verticalRot(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f));

    PxShape* shape = engine->CreateCapsuleShape(
        Const::Enemy::CAPSULE_RADIUS,
        Const::Enemy::CAPSULE_HEIGHT,
        PxVec3(0.0f),
        verticalRot,
        material,
        CustomFilterData::eDYNAMIC
    );

    actor = engine->AddDynamicActor(shape, position, PxQuat(PxIdentity), Const::Enemy::DENSITY);
    actor->setLinearDamping(Const::Enemy::LINEAR_DAMPING);
    actor->setAngularDamping(Const::Enemy::ANGULAR_DAMPING);

    actor->setRigidDynamicLockFlags(
        PxRigidDynamicLockFlag::eLOCK_ANGULAR_X |
        PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y |
        PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z
    );
}

PxVec3 Enemy::GetPosition() const {
    return actor->getGlobalPose().p;
}

void Enemy::TakeDamage(float damage, const PxVec3& pushDir, float pushImpulse) {
    health -= damage;
    std::cout << "Enemy hit! Damage: " << damage
              << ", remaining HP: " << (health > 0.0f ? health : 0.0f) << std::endl;

    PxVec3 dir = pushDir;
    dir = dir.getNormalized();
    ApplyImpulse(dir * pushImpulse);

    if (health <= Const::Enemy::MIN_HEALTH) {
        std::cout << "Enemy down!" << std::endl;
        actor->setRigidDynamicLockFlags(PxRigidDynamicLockFlags(0));
    }
}

void Enemy::ApplyImpulse(const PxVec3& impulse) {
    if (!actor) return;
    actor->addForce(impulse, PxForceMode::eIMPULSE);
}