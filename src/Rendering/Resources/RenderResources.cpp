#include "RenderResources.h"

#include <vector>

#include "Core/Constants.h"
#include "Rendering/Devices/RenderDevice.h"
#include "Rendering/Passes/RenderPass.h"
#include "Rendering/Resources/UniformBuffer.h"
#include "Rendering/Resources/VertexAttribute.h"
#include "Utilities/IO.h"

void RenderResources::LoadResources(RenderDevice& device) {
  geometryPipeline = device.CreatePipeline();

  // vertices
  std::vector<VertexAttribute> attr = {{"aPos", 0, 3, 0}};
  vertexBuffer = device.CreateBuffer();
  device.SetVertexAttributes(vertexBuffer, attr);

  // indices
  edgeIndexBuffer = device.CreateBuffer();
  faceIndexBuffer = device.CreateBuffer();
  volumeIndexBuffer = device.CreateBuffer();

  // uniforms
  frameUniformBuffer = device.CreateBuffer();
  UniformBuffer uniforms;

  // shaders
  const std::string vertexSource = IO::LoadSource("basicVertexShader.glsl");
  const std::string geometrySource = IO::LoadSource("lineThicknessGeometryShader.glsl");
  const std::string fragmentSource = IO::LoadSource("basicFragmentShader.glsl");
  const std::string textureVertexSource = IO::LoadSource("textureToScreenVertexShader.glsl");
  const std::string renderTexFragmentSource = IO::LoadSource("textureToScreenFragmentShader.glsl");

  basicShader = device.CreateShader(vertexSource, fragmentSource);
  device.UpdateUniformBuffer(frameUniformBuffer, sizeof(UniformBuffer), &uniforms, 0);
  device.SetUniform("tex0", 0);
  device.SetUniform("tex1", 1);
  device.SetUniform("tex2", 2);

  lineShader = device.CreateShader(vertexSource, fragmentSource, geometrySource);
  device.UpdateUniformBuffer(frameUniformBuffer, sizeof(UniformBuffer), &uniforms, 0);
  device.SetUniform("tex0", 0);
  device.SetUniform("tex1", 1);
  device.SetUniform("tex2", 2);

  screenShader = device.CreateShader(textureVertexSource, renderTexFragmentSource);
  device.UpdateUniformBuffer(frameUniformBuffer, sizeof(UniformBuffer), &uniforms, 0);
  device.SetUniform("tex0", 0);
  device.SetUniform("tex1", 1);
  device.SetUniform("tex2", 2);

  // render textures
  texture0 = device.CreateTexture2D(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, false);
  device.BindTexture(texture0, 0);
  texture1 = device.CreateTexture2D(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, false);
  device.BindTexture(texture1, 1);
  texture2 = device.CreateTexture2D(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, false);
  device.BindTexture(texture2, 2);

  framebuffer0 = device.CreateFrameBuffer(texture0, 0, 0);
  framebuffer1 = device.CreateFrameBuffer(texture1, 0, 0);
  framebuffer2 = device.CreateFrameBuffer(texture2, 0, 0);

  //   screenPipeline = device.CreatePipeline();

  //   fullscreenQuadVertexBuffer = device.CreateVertexBuffer(0);
  //   fullscreenQuadIndexBuffer = device.CreateIndexBuffer(0);

  //   std::vector<float> quadVertices = {
  //       // Positions         // Texture Coords
  //       -1.0f,  0.0f, 0.0f,  0.0f, 1.0f,  // Top-left
  //       -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,  // Bottom-left
  //        0.0f, -1.0f, 0.0f,  1.0f, 0.0f,  // Bottom-right
  //        0.0f,  0.0f, 0.0f,  1.0f, 1.0f   // Top-right
  //   };

  //   std::vector<uint32_t> quadIndices = {
  //       0, 1, 2,  // First triangle
  //       0, 2, 3   // Second triangle
  //   };

  //   // Upload data first
  //   device.UpdateVertexBuffer(fullscreenQuadVertexBuffer, quadVertices.size() * sizeof(float),
  //   quadVertices.data()); device.UpdateIndexBuffer(fullscreenQuadIndexBuffer, quadIndices);

  // Then set up attributes after data is uploaded
  //   std::vector<VertexAttribute> quadAttr = {{"aPos", 0, 3, 0},
  //                                            {"aTexCoord", 1, 2, sizeof(float) * 3}};
  //   device.SetVertexAttributes(fullscreenQuadVertexBuffer, quadAttr);
}

const RenderPass RenderResources::BuildLinePass() {
  RenderPass pass;

  pass.pipeline = geometryPipeline;
  pass.vertexBuffer = vertexBuffer;
  pass.indexBuffer = edgeIndexBuffer;
  pass.topology = PrimitiveTopology::Lines;
  pass.shaderProgram = basicShader;
  pass.frameBuffer = framebuffer0;

  return pass;
}

const RenderPass RenderResources::BuildFacePass() {
  RenderPass pass;

  pass.pipeline = geometryPipeline;
  pass.vertexBuffer = vertexBuffer;
  pass.indexBuffer = faceIndexBuffer;
  pass.topology = PrimitiveTopology::Triangles;
  pass.shaderProgram = basicShader;
  pass.frameBuffer = framebuffer1;

  return pass;
}

const RenderPass RenderResources::BuildScreenPass() {
  RenderPass pass;

  pass.pipeline = screenPipeline;
  pass.vertexBuffer = fullscreenQuadVertexBuffer;
  pass.indexBuffer = fullscreenQuadIndexBuffer;
  pass.topology = PrimitiveTopology::Triangles;
  pass.shaderProgram = screenShader;

  return pass;
}
