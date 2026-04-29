#pragma once

#include "PxPhysicsAPI.h"

namespace Const {
    constexpr float FIXED_DT = 1.0f / 60.0f;

    struct ObstacleDef {
        physx::PxVec3 size;
        physx::PxVec3 position;
    };

    const ObstacleDef OBSTACLES[] = {
        { physx::PxVec3(2.0f, 2.0f, 0.5f),  physx::PxVec3(-3.0f, 1.0f,  4.0f) },
        { physx::PxVec3(0.5f, 3.0f, 4.0f),  physx::PxVec3( 4.0f, 1.5f,  6.0f) },
        { physx::PxVec3(3.0f, 1.5f, 0.5f),  physx::PxVec3( 0.0f, 0.75f, 10.0f) },
        { physx::PxVec3(1.0f, 1.0f, 1.0f),  physx::PxVec3(-5.0f, 0.5f,  8.0f) },
        { physx::PxVec3(1.5f, 1.5f, 1.5f),  physx::PxVec3( 6.0f, 0.75f, 2.0f) },
        { physx::PxVec3(4.0f, 3.0f, 0.5f),  physx::PxVec3( 2.0f, 1.5f, 14.0f) }, // стена для теста
    };

    namespace Enemy {
        constexpr float CAPSULE_RADIUS = 0.4f;
        constexpr float CAPSULE_HEIGHT = 1.2f;
        constexpr float DENSITY = 80.0f;
        constexpr float LINEAR_DAMPING = 0.2f;
        constexpr float ANGULAR_DAMPING = 0.2f;
        const physx::PxVec3 SPAWN_POINT = physx::PxVec3(0.0f, 1.4f, 18.0f);
        constexpr float MAX_HEALTH = 100.0f;
        constexpr float MIN_HEALTH = 0.0f;
    }

    namespace Shooting {
        constexpr float SPREAD_HALF_ANGLE = 0.025f;
        constexpr float RANGE = 500.0f;
        constexpr float HIT_IMPULSE = 400.0f;
        constexpr float DAMAGE = 25.0f;
        constexpr int DEBUG_TRAIL_FRAMES = 12;
    }

    namespace Grenade {
        constexpr float RADIUS = 0.15f;
        constexpr float DENSITY = 1200.0f;
        constexpr float THROW_SPEED = 18.0f;
        constexpr float THROW_UP = 4.0f;
        constexpr float FUSE_SECONDS = 2.0f;
        constexpr float BLAST_RADIUS = 6.0f;
        constexpr float MAX_DAMAGE = 100.0f;
        constexpr float MAX_PUSH_IMPULSE = 1500.0f;
    }
}