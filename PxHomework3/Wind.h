#pragma once

#include "PxPhysicsAPI.h"

// Generates a wind velocity whose direction (yaw around the vertical axis) and
// strength both change over time, with an added gust term so the flags never
// settle into a steady state.
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
