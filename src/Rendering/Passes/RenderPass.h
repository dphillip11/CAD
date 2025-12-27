#pragma once
#include <cstdint>

#include "Rendering/PrimitiveTopology.h"
#include "Utilities/Vec3.h"

class RenderDevice;
class FrameContext;
class RenderPassConfiguration;

class RenderPass {
 public:
  RenderPass() = delete;
  explicit RenderPass(RenderDevice& device, const RenderPassConfiguration& config);
  void Execute(const FrameContext& context, uint32_t count);

  const uint32_t shaderHandle;
  const uint32_t frameBufferHandle;
  const uint32_t vertexBufferHandle;
  const uint32_t indexBufferHandle;
  const PrimitiveTopology topology;
  const Vec3 rgb;

 private:
  RenderDevice& device_;
};