#include "RenderResources.h"

#include <vector>

#include "IndexBuffer.h"
#include "Rendering/Devices/RenderDevice.h"
#include "ShaderProgram.h"
#include "VertexBuffer.h"

void RenderResources::LoadResources(RenderDevice& device) {
  sharedVertexPipeline = device.CreatePipeline();
  // vertices
  std::vector<VertexAttribute> attr = {{"aPos", 0, 3, 0}};
  vertexBuffer = std::make_unique<VertexBuffer>(device, attr);

  // indices
  edgeIndexBuffer = std::make_unique<IndexBuffer>(device);
  faceIndexBuffer = std::make_unique<IndexBuffer>(device);
  volumeIndexBuffer = std::make_unique<IndexBuffer>(device);

  // shaders
  const std::string vertexSource = ShaderProgram::LoadSource("basicVertexShader.glsl");
  const std::string geometrySource = ShaderProgram::LoadSource("lineThicknessGeometryShader.glsl");
  const std::string fragmentSource = ShaderProgram::LoadSource("basicFragmentShader.glsl");
  const std::string textureVertexSource =
      ShaderProgram::LoadSource("textureToScreenVertexShader.glsl");
  const std::string renderTexFragmentSource =
      ShaderProgram::LoadSource("textureToScreenFragmentShader.glsl");

  basicShader = std::make_unique<ShaderProgram>(device, vertexSource, fragmentSource);
  lineShader =
      std::make_unique<ShaderProgram>(device, vertexSource, fragmentSource, geometrySource);

  renderToScreenShader =
      std::make_unique<ShaderProgram>(device, textureVertexSource, renderTexFragmentSource);

  // render textures
  combinedRenderTextureHandle = device.CreateTexture2D(800, 400, false);
  combinedFramebufferHandle = device.CreateFrameBuffer(combinedRenderTextureHandle, 0, 0);

  screenVertexPipeline = device.CreatePipeline();
  // Fullscreen quad with texture coordinates
  std::vector<VertexAttribute> quadAttr = {{"aPos", 0, 3, 0},
                                           {"aTexCoord", 1, 2, sizeof(float) * 3}};
  fullscreenQuadVertexBuffer = std::make_unique<VertexBuffer>(device, quadAttr);

  fullscreenQuadIndexBuffer = std::make_unique<IndexBuffer>(device);

  std::vector<float> quadVertices = {
      // Positions         // Texture Coords
      -1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  // Top-left
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // Bottom-left
      1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,  // Bottom-right
      1.0f,  1.0f,  0.0f, 1.0f, 1.0f   // Top-right
  };

  std::vector<uint32_t> quadIndices = {
      0, 1, 2,  // First triangle
      0, 2, 3   // Second triangle
  };

  fullscreenQuadVertexBuffer->Upload(quadVertices, 5);  // 5 floats per vertex (3 pos + 2 texcoord)
  fullscreenQuadIndexBuffer->Upload(quadIndices);
}

RenderResources::~RenderResources() = default;