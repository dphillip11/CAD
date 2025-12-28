#pragma once
#include <memory>

#include "Rendering/Devices/GpuHandle.h"

class VertexBuffer;
class IndexBuffer;
class ShaderProgram;
class RenderDevice;
class RenderTexture;

class RenderResources {
 public:
  void LoadResources(RenderDevice& device);
  ~RenderResources();

  // vertices
  std::unique_ptr<VertexBuffer> vertexBuffer;
  std::unique_ptr<VertexBuffer> fullscreenQuadVertexBuffer;
  // indices
  std::unique_ptr<IndexBuffer> edgeIndexBuffer;
  std::unique_ptr<IndexBuffer> faceIndexBuffer;
  std::unique_ptr<IndexBuffer> volumeIndexBuffer;
  std::unique_ptr<IndexBuffer> fullscreenQuadIndexBuffer;
  // shaders
  std::unique_ptr<ShaderProgram> basicShader;
  std::unique_ptr<ShaderProgram> lineShader;
  std::unique_ptr<ShaderProgram> renderToScreenShader;
  // render textures
  GpuHandle combinedFramebufferHandle;
  GpuHandle combinedRenderTextureHandle;
  // vao
  GpuHandle sharedVertexPipeline;
  GpuHandle screenVertexPipeline;
};