#include "Grenade.h"
#include "Enemy.h"
#include "PhysicsEngine.h"
#include "ShooterConstants.h"
#include <iostream>

using namespace physx;

Grenade::Grenade(PhysicsEngine* engine, PxVec3 position, PxVec3 velocity)
    : actor(nullptr), fuse(Const::Grenade::FUSE_SECONDS), exploded(false)
{
    PxMaterial* material = engine->GetMaterial(0.4f, 0.3f, 0.55f);

    PxShape* shape = engine->CreateSphereShape(
        Const::Grenade::RADIUS, material, CustomFilterData::eDYNAMIC
    );

    actor = engine->AddDynamicActor(shape, position, PxQuat(PxIdentity), Const::Grenade::DENSITY);
    actor->setLinearVelocity(velocity);
}

void Grenade::Update(float dt) {
    if (!exploded) {
        fuse -= dt;
    }
}

PxVec3 Grenade::GetPosition() const {
    return actor ? actor->getGlobalPose().p : PxVec3(0.0f);
}

void Grenade::Explode(PhysicsEngine* engine, Enemy* enemy) {
    if (exploded || !actor) return;
    exploded = true;

    PxVec3 explosionPos = GetPosition();
    std::cout << "Grenade exploded at ("
              << explosionPos.x << ", " << explosionPos.y << ", " << explosionPos.z << ")" << std::endl;

    if (enemy && enemy->IsAlive()) {
        PxVec3 enemyPos = enemy->GetPosition();
        PxVec3 toEnemy = enemyPos - explosionPos;
        float dist = toEnemy.magnitude();

        if (dist <= Const::Grenade::BLAST_RADIUS && dist > 1e-4f) {
            PxVec3 dir = toEnemy / dist;

            // проверка на стену
            PxRaycastHit hits[16];
            PxRaycastBuffer buffer(hits, 16);
            bool blockedByWall = false;
            if (engine->Raycast(explosionPos, dir, dist, buffer)) {
                uint32_t count = buffer.getNbTouches();
                for (uint32_t i = 0; i < count; i++) {
                    const PxRaycastHit& hit = buffer.getTouch(i);
                    if (hit.actor == enemy->GetActor()) {
                        break;
                    }
                    PxShape* shape = hit.shape;
                    if (shape) {
                        PxFilterData fd = shape->getSimulationFilterData();
                        if (fd.word0 == CustomFilterData::eOBSTACLE) {
                            blockedByWall = true;
                            break;
                        }
                    }
                }
            }

            if (!blockedByWall) {
                float falloff = 1.0f - dist / Const::Grenade::BLAST_RADIUS;
                if (falloff < 0.0f) falloff = 0.0f;
                float damage = Const::Grenade::MAX_DAMAGE * falloff;
                float pushImpulse = Const::Grenade::MAX_PUSH_IMPULSE * falloff;
                enemy->TakeDamage(damage, dir, pushImpulse);
            } else {
                std::cout << "Wall blocked the blast." << std::endl;
            }
        }
    }

    engine->MarkActor(actor);
    actor = nullptr;
}
