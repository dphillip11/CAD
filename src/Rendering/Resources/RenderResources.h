#pragma once
#include <memory>

class VertexBuffer;
class IndexBuffer;
class ShaderProgram;
class RenderDevice;

class RenderResources {
 public:
  void LoadResources(RenderDevice& device);
  ~RenderResources();

  // vertices
  std::unique_ptr<VertexBuffer> vertexBuffer;
  // indices
  std::unique_ptr<IndexBuffer> edgeIndexBuffer;
  std::unique_ptr<IndexBuffer> faceIndexBuffer;
  std::unique_ptr<IndexBuffer> volumeIndexBuffer;
  // shaders
  std::unique_ptr<ShaderProgram> basicShader;
  std::unique_ptr<ShaderProgram> lineShader;
};