#include "Cloth.h"
#include "PhysicsEngine.h"

using namespace physx;

extern PhysicsEngine* physicsEngine;

Cloth::Cloth(std::vector<PxVec3> points, std::vector<uint32_t> triangles, std::vector<float> invMasses) {
	nv::cloth::Factory* factory = physicsEngine->factory;

	nv::cloth::ClothMeshDesc desc;
	desc.setToDefault();
	desc.points.count = static_cast<uint32_t>(points.size());
	desc.points.stride = sizeof(PxVec3);
	desc.points.data = points.data();
	desc.triangles.count = static_cast<uint32_t>(triangles.size() / 3);
	desc.triangles.stride = 3 * sizeof(uint32_t);
	desc.triangles.data = triangles.data();
	desc.invMasses.count = static_cast<uint32_t>(invMasses.size());
	desc.invMasses.stride = sizeof(float);
	desc.invMasses.data = invMasses.data();

	fabric = NvClothCookFabricFromMesh(factory, desc, physicsEngine->scene->getGravity());

	std::vector<PxVec4> particles(points.size());
	for (size_t i = 0; i < points.size(); i++) {
		particles[i] = PxVec4(points[i], invMasses[i]);
	}
	nv::cloth::Range<PxVec4> particlesRange(particles.data(), particles.data() + particles.size());
	cloth = factory->createCloth(particlesRange, *fabric);
	cloth->setGravity(physicsEngine->scene->getGravity());

	indices = triangles;
}

Cloth::~Cloth() {
	NV_CLOTH_DELETE(cloth);
	fabric->decRefCount();
}

nv::cloth::Cloth* Cloth::Get() const {
	return cloth;
}

std::vector<uint32_t> Cloth::GetMeshIndices() const {
	return indices;
}

uint32_t Cloth::GetNumParticles() const {
	return cloth->getNumParticles();
}

PxVec4* Cloth::GetCurrentParticles() const {
	return &cloth->getCurrentParticles()[0];
}

void Cloth::SetPlaneCollisions(std::vector<PxVec4> planes, std::vector<uint32_t> planesIndices) {
	nv::cloth::Range<PxVec4> planesRange(planes.data(), planes.data() + planes.size());
	cloth->setPlanes(planesRange, 0, 0);

	nv::cloth::Range<uint32_t> planesIndicesRange(planesIndices.data(), planesIndices.data() + planesIndices.size());
	cloth->setConvexes(planesIndicesRange, 0, 0);
}

void Cloth::SetDamping(PxVec3 damping) {
	cloth->setDamping(damping);
}

void Cloth::SetDragCoefficient(float dragCoefficient) {
	cloth->setDragCoefficient(dragCoefficient);
}

void Cloth::SetLiftCoefficient(float liftCoefficient) {
	cloth->setLiftCoefficient(liftCoefficient);
}

void Cloth::SetWindVelocity(PxVec3 wind) {
	cloth->setWindVelocity(wind);
}

void Cloth::SetAdditionalWind(PxVec3 wind) {
	additionalWind = wind;
}

void Cloth::ApplyWind(PxVec3 gustWind) {
	cloth->setWindVelocity(additionalWind + gustWind);
}