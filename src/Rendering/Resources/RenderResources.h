#pragma once

#include "Rendering/Devices/GpuHandle.h"

class RenderDevice;
class RenderPass;

class RenderResources {
 public:
  void LoadResources(RenderDevice& device);

  const RenderPass BuildLinePass();
  const RenderPass BuildFacePass();
  const RenderPass BuildScreenPass();

  // vertices
  GpuHandle vertexBuffer;
  GpuHandle fullscreenQuadVertexBuffer;
  // indices
  GpuHandle edgeIndexBuffer;
  GpuHandle faceIndexBuffer;
  GpuHandle volumeIndexBuffer;
  GpuHandle fullscreenQuadIndexBuffer;
  // shaders
  GpuHandle basicShader;
  GpuHandle lineShader;
  GpuHandle screenShader;
  // render textures
  GpuHandle framebuffer0;
  GpuHandle texture0;
  GpuHandle framebuffer1;
  GpuHandle texture1;
  GpuHandle framebuffer2;
  GpuHandle texture2;
  // vao
  GpuHandle geometryPipeline;
  GpuHandle screenPipeline;
  // uniforms
  GpuHandle frameUniformBuffer;

  // framebuffer dimensions
  int fbWidth = 0;
  int fbHeight = 0;
};