#pragma once

#include <vector>

#include "Core/Primitives.h"

struct ModelView {
  std::vector<VertexId> vertexIndices;
  std::vector<EdgeId> primitiveIds;
  std::size_t primitiveCount;

  void Clear() {
    vertexIndices.clear();
    primitiveIds.clear();
    primitiveCount = 0;
  }
};

using FaceView = ModelView;
using LineView = ModelView;

struct ModelViews {
  FaceView faces;
  FaceView volumes;
  LineView lines;
};
