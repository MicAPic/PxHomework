#include <iostream>
#include <cmath>
#include "PhysicsEngine.h"
#include "BilliardsConstants.h"
#include "snippetrender/SnippetRender.h"
#include "snippetrender/SnippetCamera.h"

using namespace physx;

PhysicsEngine* physicsEngine = nullptr;
Snippets::Camera* camera = nullptr;
PxRigidDynamic* cueBall = nullptr;
PxRigidDynamic* cueStick = nullptr;
float cueYaw = 0.0f;
float cuePitch = 0.0f;
bool isGameOver = false;

void UpdateCueStickPosition() {
    if (!cueBall || !cueStick || isGameOver) return;
    PxVec3 ballPos = cueBall->getGlobalPose().p;
    float offsetDist = Const::CUE_LENGTH / 2 + Const::CUE_OFFSET;
    PxVec3 cuePos = ballPos + PxVec3(
        sinf(cueYaw) * offsetDist * cosf(cuePitch),
        offsetDist * sinf(cuePitch),
        cosf(cueYaw) * offsetDist * cosf(cuePitch));
    PxQuat cueRot = PxQuat(cueYaw, PxVec3(0, 1, 0))
                  * PxQuat(-cuePitch, PxVec3(1, 0, 0))
                  * PxQuat(-PxHalfPi, PxVec3(0, 1, 0));
    cueStick->setKinematicTarget(PxTransform(cuePos, cueRot));
}

void CreateTable(PxMaterial* tableMaterial) {
    float halfWidth = Const::TABLE_WIDTH / 2;
    float halfHeight = Const::TABLE_HEIGHT / 2;
    float wallY = Const::BORDER_HEIGHT / 2;
    float outerOffset = Const::BORDER_THICKNESS;
    float phw = Const::POCKET_HALF_WIDTH;
    float floorY = -Const::BORDER_THICKNESS / 2;

    PxVec3 centralFloorSize(Const::TABLE_WIDTH - 2 * phw, Const::BORDER_THICKNESS, Const::TABLE_HEIGHT);
    PxShape* centralFloor = physicsEngine->CreateBoxShape(centralFloorSize, tableMaterial, CustomFilterData::eOBSTACLE);
    physicsEngine->AddStaticActor(centralFloor, PxVec3(0, floorY, 0), PxQuat(PxIdentity));

    // edge'и формируют лузы
    float edgeLen = halfHeight - 2 * phw;
    PxVec3 edgeFloorSize(phw, Const::BORDER_THICKNESS, edgeLen);

    PxShape* leftEdge1 = physicsEngine->CreateBoxShape(edgeFloorSize, tableMaterial, CustomFilterData::eOBSTACLE);
    physicsEngine->AddStaticActor(leftEdge1, PxVec3(-halfWidth + phw / 2, floorY, -halfHeight / 2), PxQuat(PxIdentity));

    PxShape* leftEdge2 = physicsEngine->CreateBoxShape(edgeFloorSize, tableMaterial, CustomFilterData::eOBSTACLE);
    physicsEngine->AddStaticActor(leftEdge2, PxVec3(-halfWidth + phw / 2, floorY, halfHeight / 2), PxQuat(PxIdentity));

    PxShape* rightEdge1 = physicsEngine->CreateBoxShape(edgeFloorSize, tableMaterial, CustomFilterData::eOBSTACLE);
    physicsEngine->AddStaticActor(rightEdge1, PxVec3(halfWidth - phw / 2, floorY, -halfHeight / 2), PxQuat(PxIdentity));

    PxShape* rightEdge2 = physicsEngine->CreateBoxShape(edgeFloorSize, tableMaterial, CustomFilterData::eOBSTACLE);
    physicsEngine->AddStaticActor(rightEdge2, PxVec3(halfWidth - phw / 2, floorY, halfHeight / 2), PxQuat(PxIdentity));
    //

    // первый ряд стен, боковушки луз
    float innerShortLen = Const::TABLE_WIDTH - 2 * phw;
    PxVec3 shortBorderSize(innerShortLen, Const::BORDER_HEIGHT, Const::BORDER_THICKNESS);

    PxShape* topBorder = physicsEngine->CreateBoxShape(shortBorderSize, tableMaterial, CustomFilterData::eOBSTACLE);
    physicsEngine->AddStaticActor(topBorder, PxVec3(0, wallY, -halfHeight - Const::BORDER_THICKNESS / 2), PxQuat(PxIdentity));

    PxShape* bottomBorder = physicsEngine->CreateBoxShape(shortBorderSize, tableMaterial, CustomFilterData::eOBSTACLE);
    physicsEngine->AddStaticActor(bottomBorder, PxVec3(0, wallY, halfHeight + Const::BORDER_THICKNESS / 2), PxQuat(PxIdentity));

    float innerLongLen = halfHeight - 2 * phw;
    PxVec3 sideBorderSize(Const::BORDER_THICKNESS, Const::BORDER_HEIGHT, innerLongLen);

    PxShape* leftBorder1 = physicsEngine->CreateBoxShape(sideBorderSize, tableMaterial, CustomFilterData::eOBSTACLE);
    physicsEngine->AddStaticActor(leftBorder1, PxVec3(-halfWidth - Const::BORDER_THICKNESS / 2, wallY, -halfHeight / 2), PxQuat(PxIdentity));

    PxShape* leftBorder2 = physicsEngine->CreateBoxShape(sideBorderSize, tableMaterial, CustomFilterData::eOBSTACLE);
    physicsEngine->AddStaticActor(leftBorder2, PxVec3(-halfWidth - Const::BORDER_THICKNESS / 2, wallY, halfHeight / 2), PxQuat(PxIdentity));

    PxShape* rightBorder1 = physicsEngine->CreateBoxShape(sideBorderSize, tableMaterial, CustomFilterData::eOBSTACLE);
    physicsEngine->AddStaticActor(rightBorder1, PxVec3(halfWidth + Const::BORDER_THICKNESS / 2, wallY, -halfHeight / 2), PxQuat(PxIdentity));

    PxShape* rightBorder2 = physicsEngine->CreateBoxShape(sideBorderSize, tableMaterial, CustomFilterData::eOBSTACLE);
    physicsEngine->AddStaticActor(rightBorder2, PxVec3(halfWidth + Const::BORDER_THICKNESS / 2, wallY, halfHeight / 2), PxQuat(PxIdentity));
    //

    // второй ряд стен
    float outerWallHeight = Const::BORDER_HEIGHT + Const::POCKET_DEPTH;
    float outerWallY = (Const::BORDER_HEIGHT - Const::POCKET_DEPTH) / 2;

    float outerShortLen = Const::TABLE_WIDTH + 2 * (outerOffset + Const::BORDER_THICKNESS);
    PxVec3 outerShortSize(outerShortLen, outerWallHeight, Const::BORDER_THICKNESS);

    PxShape* outerTop = physicsEngine->CreateBoxShape(outerShortSize, tableMaterial, CustomFilterData::eOBSTACLE);
    physicsEngine->AddStaticActor(outerTop, PxVec3(0, outerWallY, -halfHeight - outerOffset - Const::BORDER_THICKNESS / 2), PxQuat(PxIdentity));

    PxShape* outerBottom = physicsEngine->CreateBoxShape(outerShortSize, tableMaterial, CustomFilterData::eOBSTACLE);
    physicsEngine->AddStaticActor(outerBottom, PxVec3(0, outerWallY, halfHeight + outerOffset + Const::BORDER_THICKNESS / 2), PxQuat(PxIdentity));

    float outerLongLen = Const::TABLE_HEIGHT + 2 * outerOffset;
    PxVec3 outerLongSize(Const::BORDER_THICKNESS, outerWallHeight, outerLongLen);

    PxShape* outerLeft = physicsEngine->CreateBoxShape(outerLongSize, tableMaterial, CustomFilterData::eOBSTACLE);
    physicsEngine->AddStaticActor(outerLeft, PxVec3(-halfWidth - outerOffset - Const::BORDER_THICKNESS / 2, outerWallY, 0), PxQuat(PxIdentity));

    PxShape* outerRight = physicsEngine->CreateBoxShape(outerLongSize, tableMaterial, CustomFilterData::eOBSTACLE);
    physicsEngine->AddStaticActor(outerRight, PxVec3(halfWidth + outerOffset + Const::BORDER_THICKNESS / 2, outerWallY, 0), PxQuat(PxIdentity));
    //
}

void CreatePockets(PxMaterial* material) {
    float halfWidth = Const::TABLE_WIDTH / 2;
    float halfHeight = Const::TABLE_HEIGHT / 2;
    float phw = Const::POCKET_HALF_WIDTH;
    float triggerY = -Const::POCKET_DEPTH / 2 + Const::POCKET_OFFSET_Y;

    PxVec3 cornerTriggerSize(2 * phw, Const::POCKET_DEPTH, 2 * phw);
    PxVec3 sideTriggerSize(2 * phw, Const::POCKET_DEPTH, 2 * phw);

    struct PocketDef { PxVec3 size; PxVec3 position; };
    PocketDef pockets[] = {
        { cornerTriggerSize, PxVec3(-halfWidth, triggerY, -halfHeight) },
        { cornerTriggerSize, PxVec3(halfWidth, triggerY, -halfHeight) },
        { cornerTriggerSize, PxVec3(-halfWidth, triggerY, halfHeight) },
        { cornerTriggerSize, PxVec3(halfWidth, triggerY, halfHeight) },
        { sideTriggerSize, PxVec3(-halfWidth, triggerY, 0) },
        { sideTriggerSize, PxVec3(halfWidth, triggerY, 0) }
    };

    for (auto& pocket : pockets) {
        PxShape* pocketShape = physicsEngine->CreateBoxShape(
            pocket.size, material, CustomFilterData::eTRIGGER, false,
            PxShapeFlag::eVISUALIZATION | PxShapeFlag::eTRIGGER_SHAPE);
        physicsEngine->AddStaticActor(pocketShape, pocket.position, PxQuat(PxIdentity));
    }
}

// см. https://stackoverflow.com/questions/59465398/pool-game-in-java-how-to-put-15-balls, только у них h посчитан неправильно
void CreateBalls(PxMaterial* ballMaterial) {
    float pyramidStartZ = -Const::TABLE_HEIGHT / 4;
    float spacing = Const::BALL_RADIUS * Const::BALL_SPACING_MULTIPLIER;
    
    constexpr int pyramidRows = 5;
    int ballIndex = 0;
    for (int row = 0; row < pyramidRows; row++) {
        for (int col = 0; col <= row; col++) {
            float x = (col - row / 2.0f) * spacing;
            float z = pyramidStartZ - row * spacing * Const::TRIANGLE_ROW_HEIGHT_FACTOR;
            PxVec3 pos(x, Const::BALL_RADIUS, z);
            
            PxShape* ballShape = physicsEngine->CreateSphereShape(Const::BALL_RADIUS, ballMaterial, CustomFilterData::eDYNAMIC);
            PxRigidDynamic* ball = physicsEngine->AddDynamicActor(ballShape, pos, PxQuat(PxIdentity), Const::BALL_DENSITY);
            ball->setLinearDamping(Const::BALL_LINEAR_DAMPING);
            ball->setAngularDamping(Const::BALL_ANGULAR_DAMPING);
            ballIndex++;
        }
    }
    
    float cueBallZ = Const::TABLE_HEIGHT / 4;
    PxShape* cueBallShape = physicsEngine->CreateSphereShape(Const::BALL_RADIUS, ballMaterial, CustomFilterData::eCUE_BALL);
    cueBall = physicsEngine->AddDynamicActor(cueBallShape, PxVec3(0, Const::BALL_RADIUS, cueBallZ), PxQuat(PxIdentity), Const::BALL_DENSITY);
    cueBall->setLinearDamping(Const::BALL_LINEAR_DAMPING);
    cueBall->setAngularDamping(Const::BALL_ANGULAR_DAMPING);
}

void CreateCueStick(PxMaterial* material) {
    PxShape* cueShape = physicsEngine->CreateCapsuleShape(Const::CUE_RADIUS, Const::CUE_LENGTH, material, CustomFilterData::eCUE_STICK);
    
    float offsetDist = Const::CUE_LENGTH / 2 + Const::CUE_OFFSET;
    PxVec3 cuePos = PxVec3(0, Const::BALL_RADIUS, Const::TABLE_HEIGHT / 4 + offsetDist);
    PxQuat cueRot = PxQuat(-PxHalfPi, PxVec3(0, 1, 0));
    
    cueStick = physicsEngine->AddDynamicActor(cueShape, cuePos, cueRot, Const::CUE_STICK_DENSITY);
    cueStick->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
}

bool IsCueBallInScene() {
    if (!cueBall) return false;
    std::vector<PxRigidActor*> actors = physicsEngine->GetActors(PxActorTypeFlag::eRIGID_DYNAMIC);
    for (PxRigidActor* actor : actors) {
        if (actor == cueBall) return true;
    }
    return false;
}

int CountBallsInScene() {
    std::vector<PxRigidActor*> actors = physicsEngine->GetActors(PxActorTypeFlag::eRIGID_DYNAMIC);
    int count = 0;
    for (PxRigidActor* actor : actors) {
        if (actor != cueStick) count++;
    }
    return count;
}

void CheckGameState() {
    if (isGameOver) return;
    
    if (!IsCueBallInScene()) {
        isGameOver = true;
        std::cout << "Game over :(" << std::endl;
        return;
    }
    
    int ballCount = CountBallsInScene();
    if (ballCount == Const::BALLS_TO_WIN) {
        isGameOver = true;
        std::cout << "You won :)" << std::endl;
    }
}

void SetupScene() {
    PxMaterial* tableMaterial = physicsEngine->GetMaterial(0.7f, 0.6f, 0.4f);
    PxMaterial* ballMaterial = physicsEngine->GetMaterial(0.6f, 0.5f, 0.4f);
    
    CreateTable(tableMaterial);
    CreatePockets(tableMaterial);
    CreateBalls(ballMaterial);
    CreateCueStick(tableMaterial);
    
    UpdateCueStickPosition();
}

void keyPressedCallback(unsigned char key, const PxTransform& cameraTransform) {
    if (isGameOver) return;
    
    switch (toupper(key)) {
    case 'J':
        cueYaw -= Const::CUE_AIM_STEP;
        UpdateCueStickPosition();
        break;
    case 'L':
        cueYaw += Const::CUE_AIM_STEP;
        UpdateCueStickPosition();
        break;
    case 'I':
        cuePitch = min(cuePitch + Const::CUE_AIM_STEP, PxHalfPi);
        UpdateCueStickPosition();
        break;
    case 'K':
        cuePitch = max(cuePitch - Const::CUE_AIM_STEP, 0.0f);
        UpdateCueStickPosition();
        break;
    case ' ':
        if (cueBall) {
            PxVec3 hitDir(-sinf(cueYaw), 0, -cosf(cueYaw));
            cueBall->addForce(hitDir * Const::HIT_FORCE, PxForceMode::eIMPULSE);
        }
        break;
    default:
        break;
    }
}

void renderCallback() {
    physicsEngine->Simulate(1.0f / 60.0f);
    CheckGameState();
    UpdateCueStickPosition();

    Snippets::startRender(camera);
    std::vector<PxRigidActor*> actors = physicsEngine->GetActors();
    if (!actors.empty()) {
        Snippets::renderActors(actors.data(), static_cast<PxU32>(actors.size()));
    }
    Snippets::finishRender();
}

void exitCallback() {
    delete camera;
    delete physicsEngine;
}

int main() {
    camera = new Snippets::Camera(PxVec3(0.0f, 5.0f, 5.0f), PxVec3(0.0f, -0.5f, -1.0f));
    Snippets::setupDefault("Billiards", camera, keyPressedCallback, renderCallback, exitCallback);

    physicsEngine = new PhysicsEngine();
    SetupScene();

    std::cout << "~ Billiards ~" << std::endl;
    std::cout << "Controls: J/L to aim left/right, I/K to tilt up/down, SPACE to hit" << std::endl;

    glutMainLoop();
    return 0;
}
