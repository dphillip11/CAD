#pragma once

#include <vector>

#include "Core/Primitives.h"

template <typename T>
struct ModelView {
  std::vector<VertexId> vertexIndices;
  std::vector<T> primitiveIds;
  std::size_t primitiveCount;

  void Clear() {
    vertexIndices.clear();
    primitiveIds.clear();
    primitiveCount = 0;
  }
};

using VolumeView = ModelView<VolumeId>;
using FaceView = ModelView<FaceId>;
using LineView = ModelView<EdgeId>;

struct ModelViews {
  FaceView faces;
  VolumeView volumes;
  LineView lines;
};
