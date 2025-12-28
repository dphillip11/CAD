#pragma once

#include "Rendering/Devices/RenderDevice.h"
#include "Rendering/Resources/IndexBuffer.h"
#include "Utilities/Vec3.h"

class RenderDevice;
class FrameContext;
class RenderResources;

class FacePass {
 public:
  FacePass(RenderDevice& device, RenderResources& resources);
  void Execute(RenderDevice& device, RenderResources& resources, const FrameContext& context);

 private:
  Vec3 color_;
  GpuHandle vertexBufferHandle_;
  GpuHandle shaderHandle_;
  IndexBuffer& indexBuffer_;
};