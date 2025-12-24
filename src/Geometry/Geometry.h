#pragma once
#include <span>

struct Vec3;
struct Vertex;

namespace Geometry {
    bool AreColinear(std::span<const Vec3> points);
    bool AreCoplanar(std::span<const Vec3> points);
}
