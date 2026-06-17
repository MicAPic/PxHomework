#include <iostream>
#include <vector>
#include "PhysicsEngine.h"
#include "Cloth.h"
#include "FlagBuilder.h"
#include "Wind.h"
#include "FlagConstants.h"
#include "snippetrender/SnippetRender.h"
#include "snippetrender/SnippetCamera.h"

using namespace physx;

PhysicsEngine* physicsEngine = nullptr;
Snippets::Camera* camera = nullptr;
Wind* wind = nullptr;

static void AddPole(PxMaterial* material, float x) {
	PxShape* shape = physicsEngine->CreateBoxShape(PxVec3(Const::Scene::POLE_HALF_THICKNESS, Const::Scene::POLE_HEIGHT, Const::Scene::POLE_HALF_THICKNESS), material);
	physicsEngine->AddStaticActor(shape, PxVec3(x, Const::Scene::POLE_HEIGHT * 0.5f, 0.0f), PxQuat(PxIdentity));
}

static void AddCrossbar(PxMaterial* material, float xStart, float xEnd, float y) {
	float length = (xEnd - xStart) + Const::Scene::CROSSBAR_END_MARGIN;
	PxShape* shape = physicsEngine->CreateBoxShape(PxVec3(length, Const::Scene::CROSSBAR_HALF_THICKNESS, Const::Scene::CROSSBAR_HALF_THICKNESS), material);
	physicsEngine->AddStaticActor(shape, PxVec3((xStart + xEnd) * 0.5f, y, 0.0f), PxQuat(PxIdentity));
}

static Cloth* SpawnFlag(const FlagMesh& mesh, PxVec3 additionalWind = PxVec3(0.0f)) {
	Cloth* cloth = new Cloth(mesh.points, mesh.triangles, mesh.invMasses);
	cloth->SetDamping(Const::Flag::DAMPING);
	cloth->SetDragCoefficient(Const::Flag::DRAG_COEFFICIENT);
	cloth->SetLiftCoefficient(Const::Flag::LIFT_COEFFICIENT);
	cloth->SetAdditionalWind(additionalWind);

	std::vector<PxVec4> planes = { PxVec4(0.0f, 1.0f, 0.0f, 0.0f) };
	std::vector<uint32_t> planesIndices = { 1 };
	cloth->SetPlaneCollisions(planes, planesIndices);

	physicsEngine->AddCloth(cloth);
	return cloth;
}

static void SetupScene() {
	PxMaterial* material = physicsEngine->CreateMaterial(Const::Scene::MATERIAL_STATIC_FRICTION, Const::Scene::MATERIAL_DYNAMIC_FRICTION, Const::Scene::MATERIAL_RESTITUTION);
	physicsEngine->AddGround(Const::Scene::GROUND_NORMAL, Const::Scene::GROUND_DISTANCE, material);

	// прямоугольный флаг закрепленный по левому краю
	PxVec3 rectOrigin = Const::Scene::RECT_ORIGIN;
	AddPole(material, rectOrigin.x);
	SpawnFlag(FlagBuilder::BuildRectangle(rectOrigin, Const::Flag::WIDTH, Const::Flag::HEIGHT, Const::Flag::COLS, Const::Flag::ROWS), Const::Flag::FLY_WIND);

	// непрямоугольный флаг
	PxVec3 bannerOrigin = Const::Scene::BANNER_ORIGIN;
	AddPole(material, bannerOrigin.x);
	AddCrossbar(material, bannerOrigin.x, bannerOrigin.x + Const::Banner::SPAN, Const::Flag::TOP_Y);
	SpawnFlag(FlagBuilder::BuildPennant(bannerOrigin, Const::Banner::LENGTH, Const::Banner::SPAN, Const::Flag::COLS, Const::Flag::ROWS));
}

void keyPressedCallback(unsigned char key, const PxTransform& cameraTransform) {
	(void)key;
	(void)cameraTransform;
}

void renderCallback() {
	const float dt = Const::FIXED_DT;

	wind->Update(dt);
	PxVec3 windVelocity = wind->GetVelocity();
	for (Cloth* cloth : physicsEngine->GetCloths()) {
		cloth->ApplyWind(windVelocity);
	}

	physicsEngine->Simulate(dt);

	Snippets::startRender(camera, 0.1f, 10000.0f);

	std::vector<PxRigidActor*> actors = physicsEngine->GetActors();
	if (!actors.empty()) {
		Snippets::renderActors(actors.data(), static_cast<PxU32>(actors.size()));
	}

	const PxVec3 flagColor = Const::Flag::COLOR;
	for (Cloth* cloth : physicsEngine->GetCloths()) {
		uint32_t particleNum = cloth->GetNumParticles();
		PxVec4* particles = cloth->GetCurrentParticles();
		std::vector<uint32_t> indices = cloth->GetMeshIndices();

		glDisable(GL_CULL_FACE);
		Snippets::renderMesh(particleNum, particles, static_cast<PxU32>(indices.size() / 3), indices.data(), flagColor);
		glEnable(GL_CULL_FACE);
	}

	Snippets::finishRender();
}

void exitCallback() {
	delete wind;
	delete camera;
	delete physicsEngine;
}

int main() {
	camera = new Snippets::Camera(PxVec3(0.0f, 8.0f, 25.0f), PxVec3(0.0f, -0.1f, -1.0f));
	Snippets::setupDefault("Flags", camera, keyPressedCallback, renderCallback, exitCallback);

	physicsEngine = new PhysicsEngine();
	wind = new Wind(Const::Wind::DIRECTION, Const::Wind::SPEED);

	SetupScene();

	std::cout << "~ Flags ~" << std::endl;
	std::cout << "Rectangular flag and a swallowtail banner blown by changing wind." << std::endl;

	glutMainLoop();
	return 0;
}
