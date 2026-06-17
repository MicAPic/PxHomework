#include "Wind.h"

using namespace physx;

Wind::Wind(PxVec3 baseDirection, float baseStrength)
	: time(0.0f)
	, baseStrength(baseStrength)
	, baseDirection(baseDirection.getNormalized())
	, currentVelocity(baseDirection.getNormalized() * baseStrength) {
}

void Wind::Update(float dt) {
	(void)dt;
	currentVelocity = baseDirection * baseStrength;
}

PxVec3 Wind::GetVelocity() const {
	return currentVelocity;
}
