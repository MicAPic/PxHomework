#include "Wind.h"
#include <cmath>

using namespace physx;

Wind::Wind(PxVec3 baseDirection, float baseStrength)
	: time(0.0f)
	, baseStrength(baseStrength)
	, baseDirection(baseDirection.getNormalized())
	, currentVelocity(baseDirection.getNormalized() * baseStrength) {
}

void Wind::Update(float dt) {
	time += dt;

	// Swing the direction left/right around the vertical axis.
	float yaw = 0.6f * std::sin(time * 0.7f) + 0.25f * std::sin(time * 1.9f);
	PxQuat rotation(yaw, PxVec3(0.0f, 1.0f, 0.0f));
	PxVec3 direction = rotation.rotate(baseDirection);

	// Oscillating strength plus an occasional gust.
	float pulse = 0.55f + 0.45f * std::sin(time * 1.3f);
	float gust = 0.35f * std::sin(time * 3.7f) * std::sin(time * 0.5f);
	float strength = baseStrength * std::fmax(0.0f, pulse + gust);

	currentVelocity = direction * strength;
}

PxVec3 Wind::GetVelocity() const {
	return currentVelocity;
}
