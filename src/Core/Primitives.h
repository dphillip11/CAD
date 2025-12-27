#pragma once

#include <cstdint>

#include "Utilities/Vec3.h"

// Vertex

using VertexId = uint32_t;

struct Vertex {
  Vec3 position = Vec3{0};
};

// Edge

using EdgeId = uint32_t;

struct Edge {
  VertexId a;
  VertexId b;

  bool operator==(const Edge& other) const {
    return (a == other.a && b == other.b) || (a == other.b && b == other.a);
  }
};

// Face

using FaceId = uint32_t;

struct Face {
  std::vector<EdgeId> edges;
};

// Volume

using VolumeId = uint32_t;

struct Volume {
  std::vector<FaceId> faces;
};
