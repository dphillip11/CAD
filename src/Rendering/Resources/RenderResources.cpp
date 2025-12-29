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
  const std::string lineFragmentSource = IO::LoadSource("lineFragmentShader.glsl");
  const std::string textureVertexSource = IO::LoadSource("textureToScreenVertexShader.glsl");
  const std::string renderTexFragmentSource = IO::LoadSource("textureToScreenFragmentShader.glsl");

  basicShader = device.CreateShader(vertexSource, fragmentSource);
  lineShader = device.CreateShader(vertexSource, lineFragmentSource, geometrySource);
  screenShader = device.CreateShader(textureVertexSource, renderTexFragmentSource);
  device.BindShader(screenShader);
  device.SetUniform("tex0", 0);
  device.SetUniform("tex1", 1);
  device.SetUniform("tex2", 2);

  // render textures
  texture0 = device.CreateTexture2D(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, false);
  texture1 = device.CreateTexture2D(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, false);
  texture2 = device.CreateTexture2D(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, false);

  device.BindTexture(texture0, 0);
  device.BindTexture(texture1, 1);
  device.BindTexture(texture2, 2);

  framebuffer0 = device.CreateFrameBuffer(texture0, 0, 0);
  framebuffer1 = device.CreateFrameBuffer(texture1, 0, 0);
  framebuffer2 = device.CreateFrameBuffer(texture2, 0, 0);

  screenPipeline = device.CreatePipeline();

  fullscreenQuadVertexBuffer = device.CreateBuffer();
  fullscreenQuadIndexBuffer = device.CreateBuffer();

  std::vector<float> quadVertices = {
      // Positions         // Texture Coords
      -1.0f, 1.0f,  0.0f, 0.0f, 2.0f,  // Top-left
      -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // Bottom-left
      1.0f,  -1.0f, 0.0f, 2.0f, 0.0f,  // Bottom-right
      1.0f,  1.0f,  0.0f, 2.0f, 2.0f   // Top-right
  };

  std::vector<uint32_t> quadIndices = {
      0, 1, 2,  // First triangle
      0, 2, 3   // Second triangle
  };

  // Upload data first
  device.UpdateVertexBuffer(fullscreenQuadVertexBuffer, quadVertices.size() * sizeof(float),
                            quadVertices.data());
  device.UpdateIndexBuffer(fullscreenQuadIndexBuffer, quadIndices);

  //   Then set up attributes after data is uploaded
  std::vector<VertexAttribute> quadAttr = {{"aPos", 0, 3, 0},
                                           {"aTexCoord", 1, 2, sizeof(float) * 3}};
  device.SetVertexAttributes(fullscreenQuadVertexBuffer, quadAttr);
}

const RenderPass RenderResources::BuildFacePass() {
  RenderPass pass;

  pass.pipeline = geometryPipeline;
  pass.vertexBuffer = vertexBuffer;
  pass.indexBuffer = faceIndexBuffer;
  pass.topology = PrimitiveTopology::Triangles;
  pass.shaderProgram = basicShader;
  pass.frameBuffer = framebuffer0;
  pass.clearOnBind = true;
  pass.clearColor[0] = 0.0f;  // R
  pass.clearColor[1] = 0.0f;  // G
  pass.clearColor[2] = 1.0f;  // B
  pass.clearColor[3] = 0.0f;  // A

  return pass;
}

const RenderPass RenderResources::BuildLinePass() {
  RenderPass pass;

  pass.pipeline = geometryPipeline;
  pass.vertexBuffer = vertexBuffer;
  pass.indexBuffer = edgeIndexBuffer;
  pass.topology = PrimitiveTopology::Lines;
  pass.shaderProgram = lineShader;
  pass.frameBuffer = framebuffer1;
  pass.clearOnBind = true;
  pass.clearColor[0] = 1.0f;  // R
  pass.clearColor[1] = 0.0f;  // G
  pass.clearColor[2] = 0.0f;  // B
  pass.clearColor[3] = 0.0f;  // A

  return pass;
}

const RenderPass RenderResources::BuildScreenPass() {
  RenderPass pass;

  pass.pipeline = screenPipeline;
  pass.vertexBuffer = fullscreenQuadVertexBuffer;
  pass.indexBuffer = fullscreenQuadIndexBuffer;
  pass.topology = PrimitiveTopology::Triangles;
  pass.shaderProgram = screenShader;
  pass.frameBuffer = 0;      // Bind to default framebuffer to render to screen
  pass.clearOnBind = false;  // Don't clear the screen pass (already cleared in BeginFrame)

  return pass;
}
