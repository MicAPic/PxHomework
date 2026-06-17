#include "FlagBuilder.h"
#include <cmath>

using namespace physx;

namespace {

constexpr uint32_t INVALID_ID = 0xFFFFFFFFu;

// Generic grid builder. A quad (r, c) is emitted only when quadPresent returns
// true; vertices referenced by no emitted quad are dropped and indices remapped,
// so the resulting mesh has no stray particles. A vertex is pinned (invMass 0)
// when pinned returns true. The world position of each grid node comes from
// position(row, col), which lets callers rotate/reorient the mesh freely.
template <typename QuadPredicate, typename PinPredicate, typename PositionFn>
FlagMesh BuildGrid(uint32_t cols, uint32_t rows,
	QuadPredicate quadPresent, PinPredicate pinned, PositionFn position) {
	std::vector<uint32_t> remap(cols * rows, INVALID_ID);
	FlagMesh mesh;

	auto useVertex = [&](uint32_t row, uint32_t col) -> uint32_t {
		uint32_t gid = row * cols + col;
		if (remap[gid] == INVALID_ID) {
			remap[gid] = static_cast<uint32_t>(mesh.points.size());
			mesh.points.push_back(position(row, col));
			mesh.invMasses.push_back(pinned(row, col) ? 0.0f : 1.0f);
		}
		return remap[gid];
	};

	for (uint32_t r = 0; r + 1 < rows; r++) {
		for (uint32_t c = 0; c + 1 < cols; c++) {
			if (!quadPresent(r, c)) {
				continue;
			}
			uint32_t a = useVertex(r, c);
			uint32_t b = useVertex(r, c + 1);
			uint32_t cc = useVertex(r + 1, c);
			uint32_t d = useVertex(r + 1, c + 1);

			mesh.triangles.push_back(a);
			mesh.triangles.push_back(b);
			mesh.triangles.push_back(d);

			mesh.triangles.push_back(a);
			mesh.triangles.push_back(d);
			mesh.triangles.push_back(cc);
		}
	}

	return mesh;
}

} // namespace

FlagMesh FlagBuilder::BuildRectangle(PxVec3 origin, float width, float height, uint32_t cols, uint32_t rows) {
	const float dx = width / static_cast<float>(cols - 1);
	const float dy = height / static_cast<float>(rows - 1);

	auto allQuads = [](uint32_t, uint32_t) { return true; };
	// Proper flag: fixed to the pole along the hoist (left) edge at top and bottom.
	auto pinHoist = [rows](uint32_t row, uint32_t col) {
		return col == 0 && (row == 0 || row == rows - 1);
	};
	auto position = [=](uint32_t row, uint32_t col) {
		return PxVec3(origin.x + col * dx, origin.y - row * dy, origin.z);
	};
	return BuildGrid(cols, rows, allQuads, pinHoist, position);
}

FlagMesh FlagBuilder::BuildPennant(PxVec3 origin, float width, float height, uint32_t cols, uint32_t rows) {
	// Rotated 90 degrees relative to the rectangle: the cols axis runs downward
	// (vertical, length = width) and the rows axis runs horizontally
	// (the crossbar, span = height). The notch is cut from the far end of the
	// cols axis, so the swallowtail points straight down.
	const float dDown = width / static_cast<float>(cols - 1);
	const float dAcross = height / static_cast<float>(rows - 1);

	const float centerRow = (rows - 1) * 0.5f;
	const uint32_t notchDepthCols = (cols >= 8) ? 5u : (cols / 2);

	auto pennantQuads = [=](uint32_t r, uint32_t c) {
		uint32_t lastQuadCol = cols - 2;
		uint32_t distFromEnd = lastQuadCol - c;
		if (distFromEnd >= notchDepthCols) {
			return true;
		}
		float reach = static_cast<float>(notchDepthCols - distFromEnd);
		float halfRows = reach * 0.4f;
		float quadCenter = r + 0.5f;
		return std::fabs(quadCenter - centerRow) >= halfRows;
	};
	// Hangs from the crossbar: pinned at the two ends of the top (cols == 0) edge.
	auto pinCrossbar = [rows](uint32_t row, uint32_t col) {
		return col == 0 && (row == 0 || row == rows - 1);
	};
	auto position = [=](uint32_t row, uint32_t col) {
		return PxVec3(origin.x + row * dAcross, origin.y - col * dDown, origin.z);
	};
	return BuildGrid(cols, rows, pennantQuads, pinCrossbar, position);
}
