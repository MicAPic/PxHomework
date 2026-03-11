#pragma once

namespace Const {
    constexpr float TABLE_WIDTH = 1.25f;
    constexpr float TABLE_HEIGHT = 2.5f;
    constexpr float BORDER_HEIGHT = 0.1f;
    constexpr float BORDER_THICKNESS = 0.03f;

    constexpr float BALL_RADIUS = 0.0275f;
    constexpr float POCKET_HALF_WIDTH = 0.06f;
    constexpr float POCKET_DEPTH = 0.18f;
    constexpr float POCKET_OFFSET_Y = -0.025f;

    constexpr float BALL_SPACING_MULTIPLIER = 2.1f;
    constexpr float TRIANGLE_ROW_HEIGHT_FACTOR = 0.866025f; // sqrt(3) / 2

    constexpr float CUE_RADIUS = 0.01f;
    constexpr float CUE_LENGTH = 0.5f;
    constexpr float CUE_OFFSET = 0.05f;
    constexpr float CUE_AIM_STEP = 0.1f;
    constexpr float HIT_FORCE = 0.5f;

    constexpr float BALL_DENSITY = 1000.0f;
    constexpr float CUE_STICK_DENSITY = 1.0f;

    constexpr float BALL_LINEAR_DAMPING = 0.8f;
    constexpr float BALL_ANGULAR_DAMPING = 0.3f;

    constexpr int BALLS_TO_WIN = 1;
}