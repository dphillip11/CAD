#pragma once
#include <cstddef>

#include "Rendering/Devices/GpuHandle.h"
#include "Rendering/PrimitiveTopology.h"

class RenderDevice;
class RenderResources;

struct RenderPass {
  void Execute(RenderDevice& device, size_t count) const;

  GpuHandle pipeline = 0;
  GpuHandle vertexBuffer = 0;
  GpuHandle indexBuffer = 0;
  GpuHandle frameBuffer = 0;
  GpuHandle shaderProgram = 0;
  PrimitiveTopology topology;

  // Clear settings
  bool clearOnBind = true;
  float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
};