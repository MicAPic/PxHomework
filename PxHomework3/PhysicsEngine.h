#pragma once

#include <vector>
#include "PxPhysicsAPI.h"
#include "NvCloth/Factory.h"
#include "NvCloth/Solver.h"
#include "Cloth.h"

class PhysicsEngine {
public:
	PhysicsEngine();
	~PhysicsEngine();

	void Simulate(float elapsedTime);

	physx::PxMaterial* CreateMaterial(float staticFriction, float dynamicFriction, float restitution);
	physx::PxRigidStatic* AddGround(physx::PxVec3 normal, float distance, physx::PxMaterial* material);
	physx::PxShape* CreateBoxShape(physx::PxVec3 size, physx::PxMaterial* material);
	physx::PxRigidStatic* AddStaticActor(physx::PxShape* shape, physx::PxVec3 position, physx::PxQuat rotation);

	void AddCloth(Cloth* cloth);
	std::vector<Cloth*> GetCloths() const;

	std::vector<physx::PxRigidActor*> GetActors();

private:
	physx::PxDefaultAllocator allocatorCallback;
	physx::PxDefaultErrorCallback errorCallback;
	physx::PxFoundation* foundation;
	physx::PxPhysics* physics;
	physx::PxDefaultCpuDispatcher* dispatcher;
	physx::PxScene* scene;

	nv::cloth::Factory* factory;
	nv::cloth::Solver* solver;
	std::vector<Cloth*> cloths;

	friend class Cloth;
};
