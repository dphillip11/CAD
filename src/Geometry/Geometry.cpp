#include "Geometry.h"

#include <cmath>
#include <vector>

#include "Utilities/Vec3.h"

namespace Geometry {

bool AreColinear(std::span<const Vec3> points) {
  if (points.size() < 2) return false;

  if (points.size() == 2) return true;

  const Vec3& p0 = points[0];
  const Vec3 direction = points[1] - p0;

  // All points identical
  if (direction.LengthSquared() == 0.0f) {
    for (std::size_t i = 2; i < points.size(); ++i) {
      if ((points[i] - p0).LengthSquared() != 0.0f) return false;
    }
    return true;
  }

  for (std::size_t i = 2; i < points.size(); ++i) {
    const Vec3 v = points[i] - p0;

    // v must be parallel to direction
    if (!IsEqual(direction.Cross(v).LengthSquared(), 0.0f)) return false;
  }

  return true;
}

bool AreCoplanar(std::span<const Vec3> points) {
  if (points.size() < 3) return true;

  const Vec3& p0 = points[0];

  // Find first non-collinear triplet
  Vec3 normal{};
  bool foundPlane = false;

  for (std::size_t i = 1; i + 1 < points.size(); ++i) {
    normal = (points[i] - p0).Cross(points[i + 1] - p0);
    if (!IsEqual(normal.LengthSquared(), 0.0f)) {
      foundPlane = true;
      break;
    }
  }

  // All points collinear → coplanar
  if (!foundPlane) return true;

  for (std::size_t i = 1; i < points.size(); ++i) {
    const Vec3 v = points[i] - p0;
    if (!IsEqual(normal.Dot(v), 0.0f)) return false;
  }

  return true;
}

}  // namespace Geometry
