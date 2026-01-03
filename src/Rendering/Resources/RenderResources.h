#pragma once

#include "Rendering/Devices/GpuHandle.h"

class RenderDevice;
class RenderPass;

class RenderResources {
 public:
  void LoadResources(RenderDevice& device);

  const RenderPass BuildPointPass();
  const RenderPass BuildLinePass();
  const RenderPass BuildFacePass();
  const RenderPass BuildScreenPass();

  // vertices
  GpuHandle vertexBuffer;
  GpuHandle faceVertexBuffer;  // Expanded vertices for faces (non-indexed)
  GpuHandle fullscreenQuadVertexBuffer;
  GpuHandle pointCrossTemplateBuffer;  // Template mesh for point crosses (instanced)
  GpuHandle facePrimitiveIdBuffer;     // Buffer for per-vertex primitive IDs
  // indices
  GpuHandle edgeIndexBuffer;
  GpuHandle faceIndexBuffer;
  GpuHandle volumeIndexBuffer;
  GpuHandle fullscreenQuadIndexBuffer;
  GpuHandle pointCrossIndexBuffer;  // Indices for point cross template
  // shaders
  GpuHandle pointShader;
  GpuHandle basicShader;
  GpuHandle lineShader;
  GpuHandle screenShader;
  // render textures
  GpuHandle framebuffer0;
  GpuHandle texture0;
  GpuHandle depthTexture0;
  GpuHandle framebuffer1;
  GpuHandle texture1;
  GpuHandle depthTexture1;
  GpuHandle framebuffer2;
  GpuHandle texture2;
  GpuHandle depthTexture2;
  // vao
  GpuHandle geometryPipeline;
  GpuHandle facePipeline;  // Pipeline for faces with expanded vertices
  GpuHandle screenPipeline;
  // uniforms
  GpuHandle frameUniformBuffer;

  // framebuffer dimensions
  int fbWidth = 0;
  GpuHandle pointCrossPipeline;  // Pipeline for instanced point crosses

  // Point cross template info
  int pointCrossVertexCount = 0;
  int pointCrossIndexCount = 0;

  int fbHeight = 0;
};