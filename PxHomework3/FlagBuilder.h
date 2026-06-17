#pragma once

#include <vector>
#include <cstdint>
#include "PxPhysicsAPI.h"

struct FlagMesh {
	std::vector<physx::PxVec3> points;
	std::vector<uint32_t> triangles;
	std::vector<float> invMasses;
};

// Builds cloth meshes for flags. Both flags are pinned only at their two upper
// corners so they hang and flap. The pennant removes a triangular notch from its
// free (fly) edge so its silhouette is not a rectangle (swallowtail banner).
class FlagBuilder {
public:
	// Plain rectangular flag. cols x rows grid (>= 50 vertices for the defaults).
	static FlagMesh BuildRectangle(physx::PxVec3 origin, float width, float height, uint32_t cols, uint32_t rows);

	// Swallowtail banner: a triangular wedge is cut out of the fly edge.
	static FlagMesh BuildPennant(physx::PxVec3 origin, float width, float height, uint32_t cols, uint32_t rows);
};
