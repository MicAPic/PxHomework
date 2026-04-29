#include "Player.h"
#include "Enemy.h"
#include "Grenade.h"
#include "PhysicsEngine.h"
#include "ShooterConstants.h"
#include "snippetrender/SnippetRender.h"

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <cfloat>

using namespace physx;

Player::Player(PhysicsEngine* engineIn, Enemy* enemyIn)
    : engine(engineIn), enemy(enemyIn),
      trailStart(0.0f), trailEnd(0.0f), trailFramesLeft(0) {
}

Player::~Player() {
    grenades.clear();
}

PxVec3 Player::ApplySpread(const PxVec3& dirIn, float halfAngle) {
    PxVec3 dir = dirIn.getNormalized();

    PxVec3 up(0.0f, 1.0f, 0.0f);
    if (fabsf(dir.dot(up)) > 0.99f) {
        up = PxVec3(1.0f, 0.0f, 0.0f);
    }
    PxVec3 right = dir.cross(up).getNormalized();
    PxVec3 trueUp = right.cross(dir).getNormalized();

    auto rand01 = []() { return static_cast<float>(rand()) / static_cast<float>(RAND_MAX); };
    float yaw = (rand01() * 2.0f - 1.0f) * halfAngle;
    float pitch = (rand01() * 2.0f - 1.0f) * halfAngle;

    PxVec3 spread = dir + right * tanf(yaw) + trueUp * tanf(pitch);
    return spread.getNormalized();
}

void Player::Shoot(const PxVec3& origin, const PxVec3& dirIn) {
    PxVec3 dir = ApplySpread(dirIn, Const::Shooting::SPREAD_HALF_ANGLE);

    PxVec3 worldUp(0.0f, 1.0f, 0.0f);
    PxVec3 right = worldUp.cross(dir);
    right.normalize();

    PxVec3 barrelOffset = right * 0.20f + worldUp * (-0.20f) + dir * 0.30f;

    trailStart = origin + barrelOffset;
    trailEnd = origin + dir * Const::Shooting::RANGE;
    trailFramesLeft = Const::Shooting::DEBUG_TRAIL_FRAMES;

    PxRaycastHit hits[32];
    PxRaycastBuffer buffer(hits, 32);
    if (!engine->Raycast(origin, dir, Const::Shooting::RANGE, buffer)) {
        return;
    }

    uint32_t hitsCount = buffer.getNbTouches();

    int closestIdx = -1;
    float closestDist = FLT_MAX;
    for (uint32_t i = 0; i < hitsCount; i++) {
        const PxRaycastHit& hit = buffer.getTouch(i);
        if (hit.distance < closestDist) {
            closestDist = hit.distance;
            closestIdx = static_cast<int>(i);
        }
    }

    if (closestIdx < 0) return;

    const PxRaycastHit& hit = buffer.getTouch(closestIdx);
    trailEnd = hit.position;

    if (enemy && hit.actor == enemy->GetActor()) {
        std::cout << "Hit! Enemy struck at ("
                  << hit.position.x << ", " << hit.position.y << ", " << hit.position.z << ")" << std::endl;
        enemy->TakeDamage(Const::Shooting::DAMAGE, dir, Const::Shooting::HIT_IMPULSE);
    }
}

void Player::ThrowGrenade(const PxVec3& origin, const PxVec3& dirIn) {
    PxVec3 dir = dirIn.getNormalized();
    PxVec3 spawnPos = origin + dir * 0.5f;
    PxVec3 velocity = dir * Const::Grenade::THROW_SPEED + PxVec3(0.0f, Const::Grenade::THROW_UP, 0.0f);

    grenades.emplace_back(std::make_unique<Grenade>(engine, spawnPos, velocity));
    std::cout << "Grenade thrown!" << std::endl;
}

void Player::Update(float dt) {
    if (trailFramesLeft > 0) {
        trailFramesLeft--;
    }

    for (auto& g : grenades) {
        g->Update(dt);
        if (g->ShouldExplode()) {
            g->Explode(engine, enemy);
        }
    }

    grenades.erase(
        std::remove_if(grenades.begin(), grenades.end(),
            [](const std::unique_ptr<Grenade>& g) { return g->IsDone(); }),
        grenades.end()
    );
}

// пришлось использовать GL, т.к. Snippets::DrawLine не работал
void Player::DrawTrail() {
    GLboolean depthWasEnabled = glIsEnabled(GL_DEPTH_TEST);
    GLboolean lightingWasEnabled = glIsEnabled(GL_LIGHTING);
    GLfloat prevLineWidth = 1.0f;
    glGetFloatv(GL_LINE_WIDTH, &prevLineWidth);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glLineWidth(3.0f);

    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f); // красный
    glVertex3f(trailStart.x, trailStart.y, trailStart.z);
    glVertex3f(trailEnd.x, trailEnd.y, trailEnd.z);
    glEnd();

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glLineWidth(prevLineWidth);
    if (depthWasEnabled) glEnable(GL_DEPTH_TEST);
    if (lightingWasEnabled) glEnable(GL_LIGHTING);
}