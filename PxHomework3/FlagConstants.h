#pragma once

#include "PxPhysicsAPI.h"

namespace Const {
    constexpr float FIXED_DT = 1.0f / 60.0f;

    namespace Flag {
        constexpr uint32_t COLS = 20;
        constexpr uint32_t ROWS = 12;
        constexpr float WIDTH = 8.0f;
        constexpr float HEIGHT = 5.0f;
        constexpr float TOP_Y = 12.0f;

        const physx::PxVec3 DAMPING = physx::PxVec3(0.1f, 0.1f, 0.1f);
        constexpr float DRAG_COEFFICIENT = 0.4f;
        constexpr float LIFT_COEFFICIENT = 0.5f;

        // доп ветер для прямоугольного флага, чтобы не падал
        const physx::PxVec3 FLY_WIND = physx::PxVec3(5.5f, 0.0f, 3.0f);

        const physx::PxVec3 COLOR = physx::PxVec3(0.1f, 0.3f, 0.9f);
    }

    namespace Banner {
        constexpr float LENGTH = 8.0f;
        constexpr float SPAN = 5.0f;
    }

    namespace Scene {
        const physx::PxVec3 RECT_ORIGIN = physx::PxVec3(-9.0f, Flag::TOP_Y, 0.0f);
        const physx::PxVec3 BANNER_ORIGIN = physx::PxVec3(2.0f, Flag::TOP_Y, 0.0f);

        constexpr float POLE_HEIGHT = 13.0f;
        constexpr float POLE_HALF_THICKNESS = 0.3f;
        constexpr float CROSSBAR_HALF_THICKNESS = 0.3f;
        constexpr float CROSSBAR_END_MARGIN = 0.6f;

        constexpr float MATERIAL_STATIC_FRICTION = 0.6f;
        constexpr float MATERIAL_DYNAMIC_FRICTION = 0.5f;
        constexpr float MATERIAL_RESTITUTION = 0.1f;

        const physx::PxVec3 GROUND_NORMAL = physx::PxVec3(0.0f, 1.0f, 0.0f);
        constexpr float GROUND_DISTANCE = 0.0f;
    }

    namespace Wind {
        const physx::PxVec3 DIRECTION = physx::PxVec3(0.0f, 0.0f, 1.0f);
        constexpr float SPEED = 1.5f;
    }
}