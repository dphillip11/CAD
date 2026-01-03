#pragma once

#include <vector>

#include "Core/Primitives.h"

struct LineView {
  std::vector<VertexId> vertexIndices;
  std::vector<EdgeId> primitiveIds;
  std::size_t primitiveCount;

  void Clear() {
    vertexIndices.clear();
    primitiveIds.clear();
    primitiveCount = 0;
  }
};

struct FaceView {
  std::vector<Vec3> vertices;
  std::vector<FaceId> primitiveIds;
  std::size_t primitiveCount;

  void Clear() {
    vertices.clear();
    primitiveIds.clear();
    primitiveCount = 0;
  }
};

using VolumeView = FaceView;

struct ModelViews {
  FaceView faces;
  VolumeView volumes;
  LineView lines;
};
