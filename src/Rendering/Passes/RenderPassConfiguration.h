#pragma once
#include <cstdint>

#include "Rendering/PrimitiveTopology.h"

struct RenderPassConfiguration {
  uint32_t shaderHandle;
  uint32_t frameBufferHandle;
  uint32_t vertexBufferHandle;
  uint32_t indexBufferHandle;
  PrimitiveTopology topology;
  Vec3 rgb;
};