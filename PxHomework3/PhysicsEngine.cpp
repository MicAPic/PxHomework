#include "PhysicsEngine.h"

using namespace physx;

#define SAFE_RELEASE(obj) {	\
	if (obj) {				\
		obj->release();		\
		obj = nullptr;		\
	}						\
}

PhysicsEngine::PhysicsEngine() {
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, allocatorCallback, errorCallback);
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), false);
	PxInitExtensions(*physics, nullptr);

	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	dispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = dispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	scene = physics->createScene(sceneDesc);

	nv::cloth::InitializeNvCloth(&allocatorCallback, &errorCallback, nullptr, nullptr);
	factory = NvClothCreateFactoryCPU();
	solver = factory->createSolver();
}

PhysicsEngine::~PhysicsEngine() {
	for (Cloth* cloth : cloths) {
		solver->removeCloth(cloth->Get());
		delete cloth;
	}
	cloths.clear();

	NV_CLOTH_DELETE(solver);
	NvClothDestroyFactory(factory);

	std::vector<PxRigidActor*> actors = GetActors();
	for (PxRigidActor* actor : actors) {
		uint32_t shapesNum = actor->getNbShapes();
		if (shapesNum > 0) {
			std::vector<PxShape*> shapes(shapesNum);
			actor->getShapes(shapes.data(), shapesNum);
			for (PxShape* shape : shapes) {
				SAFE_RELEASE(shape);
			}
		}
		SAFE_RELEASE(actor);
	}

	SAFE_RELEASE(scene);
	SAFE_RELEASE(dispatcher);
	PxCloseExtensions();
	SAFE_RELEASE(physics);
	SAFE_RELEASE(foundation);
}

void PhysicsEngine::Simulate(float elapsedTime) {
	scene->simulate(elapsedTime);
	scene->fetchResults(true);

	solver->beginSimulation(elapsedTime);
	for (int i = 0; i < solver->getSimulationChunkCount(); i++) {
		solver->simulateChunk(i);
	}
	solver->endSimulation();
}

PxMaterial* PhysicsEngine::CreateMaterial(float staticFriction, float dynamicFriction, float restitution) {
	return physics->createMaterial(staticFriction, dynamicFriction, restitution);
}

PxRigidStatic* PhysicsEngine::AddGround(PxVec3 normal, float distance, PxMaterial* material) {
	PxPlane plane(normal.getNormalized(), distance);
	PxRigidStatic* groundPlane = PxCreatePlane(*physics, plane, *material);
	scene->addActor(*groundPlane);
	return groundPlane;
}

PxShape* PhysicsEngine::CreateBoxShape(PxVec3 size, PxMaterial* material) {
	PxBoxGeometry geometry(size / 2.0f);
	return physics->createShape(geometry, *material, false);
}

PxRigidStatic* PhysicsEngine::AddStaticActor(PxShape* shape, PxVec3 position, PxQuat rotation) {
	PxRigidStatic* actor = physics->createRigidStatic(PxTransform(position, rotation));
	actor->attachShape(*shape);
	scene->addActor(*actor);
	return actor;
}

void PhysicsEngine::AddCloth(Cloth* cloth) {
	solver->addCloth(cloth->Get());
	cloths.push_back(cloth);
}

std::vector<Cloth*> PhysicsEngine::GetCloths() const {
	return cloths;
}

std::vector<PxRigidActor*> PhysicsEngine::GetActors() {
	PxActorTypeFlags types = PxActorTypeFlag::eRIGID_STATIC | PxActorTypeFlag::eRIGID_DYNAMIC;
	uint32_t actorsNum = scene->getNbActors(types);
	std::vector<PxRigidActor*> actors(actorsNum);
	if (actorsNum > 0) {
		scene->getActors(types, reinterpret_cast<PxActor**>(actors.data()), actorsNum);
	}
	return actors;
}
