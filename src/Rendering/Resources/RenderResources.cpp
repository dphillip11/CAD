#include "RenderResources.h"

#include <vector>

#include "IndexBuffer.h"
#include "Rendering/Devices/RenderDevice.h"
#include "ShaderProgram.h"
#include "VertexBuffer.h"

void RenderResources::LoadResources(RenderDevice& device) {
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

  basicShader = std::make_unique<ShaderProgram>(device, vertexSource, fragmentSource);
  lineShader =
      std::make_unique<ShaderProgram>(device, vertexSource, fragmentSource, geometrySource);
}

RenderResources::~RenderResources() = default;