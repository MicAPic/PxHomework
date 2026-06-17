#pragma once

#include "PxPhysicsAPI.h"

class Wind {
public:
	Wind(physx::PxVec3 baseDirection, float baseStrength);

	void Update(float dt);
	physx::PxVec3 GetVelocity() const;

private:
	float time;
	float baseStrength;
	physx::PxVec3 baseDirection;
	physx::PxVec3 currentVelocity;
};
