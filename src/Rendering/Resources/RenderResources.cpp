#include "RenderResources.h"

#include <vector>

#include "Core/Constants.h"
#include "Rendering/Devices/RenderDevice.h"
#include "Rendering/Passes/RenderPass.h"
#include "Rendering/Resources/UniformBuffer.h"
#include "Rendering/Resources/VertexAttribute.h"
#include "Utilities/IO.h"

void RenderResources::LoadResources(RenderDevice& device) {
  // Store framebuffer dimensions
  fbWidth = device.GetFramebufferWidth();
  fbHeight = device.GetFramebufferHeight();

  geometryPipeline = device.CreatePipeline();

  // vertices
  std::vector<VertexAttribute> attr = {{"aPos", 0, 3, 0}};
  vertexBuffer = device.CreateBuffer();
  device.BindPipeline(geometryPipeline);  // Bind VAO before setting attributes
  device.SetVertexAttributes(vertexBuffer, attr);

  // Face rendering pipeline (separate for expanded vertices + primitive IDs)
  facePipeline = device.CreatePipeline();
  faceVertexBuffer = device.CreateBuffer();
  facePrimitiveIdBuffer = device.CreateBuffer();

  device.BindPipeline(facePipeline);
  device.SetVertexAttributes(faceVertexBuffer, attr);  // Position at location 0
  std::vector<VertexAttribute> primitiveIdAttr = {{"aPrimitiveId", 1, 1, 0}};
  device.SetVertexAttributes(facePrimitiveIdBuffer, primitiveIdAttr);  // ID at location 1

  // indices
  edgeIndexBuffer = device.CreateBuffer();
  faceIndexBuffer = device.CreateBuffer();
  volumeIndexBuffer = device.CreateBuffer();

  // uniforms
  frameUniformBuffer = device.CreateBuffer();
  UniformBuffer uniforms;

  // shaders
  const std::string vertexSource = IO::LoadSource("basicVertexShader.glsl");
  const std::string fragmentSource = IO::LoadSource("basicFragmentShader.glsl");
  const std::string lineFragmentSource = IO::LoadSource("lineFragmentShader.glsl");
  const std::string textureVertexSource = IO::LoadSource("textureToScreenVertexShader.glsl");
  const std::string renderTexFragmentSource = IO::LoadSource("textureToScreenFragmentShader.glsl");
  const std::string worldPosVertexSource = IO::LoadSource("worldPosVertexShader.glsl");
  const std::string worldPosFragmentSource = IO::LoadSource("worldPosFragmentShader.glsl");
  const std::string groundPlaneVertexSource = IO::LoadSource("groundPlaneVertexShader.glsl");
  const std::string groundPlaneFragmentSource = IO::LoadSource("groundPlaneFragmentShader.glsl");
  const std::string pointVertexSource = IO::LoadSource("pointVertex.glsl");
  const std::string pointFragmentSource = IO::LoadSource("pointFragment.glsl");
  const std::string debugVertexSource = IO::LoadSource("debugVertex.glsl");
  const std::string debugFragmentSource = IO::LoadSource("debugFragment.glsl");

#ifndef __EMSCRIPTEN__
  // Geometry shaders are not supported in WebGL/OpenGL ES
  const std::string geometrySource = IO::LoadSource("lineThicknessGeometryShader.glsl");
  const std::string pointGeometrySource = IO::LoadSource("pointGeometry.glsl");
  pointShader = device.CreateShader(pointVertexSource, pointFragmentSource, pointGeometrySource);
#else
  // For Emscripten, use simple point rendering
  const std::string pointVertexInstancedSource = IO::LoadSource("pointVertexInstanced.glsl");
  const std::string pointFragmentInstancedSource = IO::LoadSource("pointFragmentInstanced.glsl");
  pointShader = device.CreateShader(pointVertexInstancedSource, pointFragmentInstancedSource);
#endif

  device.BindShader(pointShader);
  device.UpdateUniformBuffer(frameUniformBuffer, sizeof(UniformBuffer), &uniforms, 0);
  basicShader = device.CreateShader(vertexSource, fragmentSource);
  device.BindShader(basicShader);
  device.UpdateUniformBuffer(frameUniformBuffer, sizeof(UniformBuffer), &uniforms, 0);
  device.SetUniform("faceIdTexture", 6);

#ifndef __EMSCRIPTEN__
  lineShader = device.CreateShader(vertexSource, lineFragmentSource, geometrySource);
#else
  lineShader = device.CreateShader(vertexSource, lineFragmentSource);
#endif

  device.BindShader(lineShader);
  device.UpdateUniformBuffer(frameUniformBuffer, sizeof(UniformBuffer), &uniforms, 0);
  screenShader = device.CreateShader(textureVertexSource, renderTexFragmentSource);
  worldPosShader = device.CreateShader(worldPosVertexSource, worldPosFragmentSource);
  groundPlaneShader = device.CreateShader(groundPlaneVertexSource, groundPlaneFragmentSource);
  device.BindShader(worldPosShader);
  device.UpdateUniformBuffer(frameUniformBuffer, sizeof(UniformBuffer), &uniforms, 0);
  device.BindShader(groundPlaneShader);
  device.UpdateUniformBuffer(frameUniformBuffer, sizeof(UniformBuffer), &uniforms, 0);
  device.BindShader(screenShader);
  device.UpdateUniformBuffer(frameUniformBuffer, sizeof(UniformBuffer), &uniforms, 0);

  device.SetUniform("tex0", 0);
  device.SetUniform("tex1", 1);
  device.SetUniform("tex2", 2);
  device.SetUniform("depth0", 3);
  device.SetUniform("depth1", 4);
  device.SetUniform("depth2", 5);

  debugShader = device.CreateShader(debugVertexSource, debugFragmentSource);
  device.BindShader(debugShader);
  device.UpdateUniformBuffer(frameUniformBuffer, sizeof(UniformBuffer), &uniforms, 0);

  device.SetUniform("tex0", 0);
  device.SetUniform("tex1", 1);
  device.SetUniform("tex2", 2);
  device.SetUniform("depth0", 3);
  device.SetUniform("depth1", 4);
  device.SetUniform("depth2", 5);

  // render textures - use actual framebuffer size
  texture0 = device.CreateTexture2D(fbWidth, fbHeight, false);
  texture1 = device.CreateTexture2D(fbWidth, fbHeight, false);
  texture2 = device.CreateTexture2D(fbWidth, fbHeight, false);
  texture3 = device.CreateFloatTexture2D(fbWidth, fbHeight);  // Float texture for world positions

  depthTexture0 = device.CreateDepthTexture2D(fbWidth, fbHeight);
  depthTexture1 = device.CreateDepthTexture2D(fbWidth, fbHeight);
  depthTexture2 = device.CreateDepthTexture2D(fbWidth, fbHeight);
  depthTexture3 = device.CreateDepthTexture2D(fbWidth, fbHeight);

  device.BindTexture(texture0, 0);
  device.BindTexture(texture1, 1);
  device.BindTexture(texture2, 2);
  device.BindTexture(depthTexture0, 3);
  device.BindTexture(depthTexture1, 4);
  device.BindTexture(depthTexture2, 5);

  framebuffer0 = device.CreateFrameBuffer(texture0, depthTexture0, 0);
  framebuffer1 = device.CreateFrameBuffer(texture1, depthTexture1, 0);
  framebuffer2 = device.CreateFrameBuffer(texture2, depthTexture2, 0);
  framebuffer3 = device.CreateFrameBuffer(texture3, depthTexture3, 0);

  screenPipeline = device.CreatePipeline();

  fullscreenQuadVertexBuffer = device.CreateBuffer();
  fullscreenQuadIndexBuffer = device.CreateBuffer();

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

  // Upload data first
  device.UpdateVertexBuffer(fullscreenQuadVertexBuffer, quadVertices.size() * sizeof(float),
                            quadVertices.data());
  device.UpdateIndexBuffer(fullscreenQuadIndexBuffer, quadIndices);

  //   Then set up attributes after data is uploaded
  std::vector<VertexAttribute> quadAttr = {{"aPos", 0, 3, 0},
                                           {"aTexCoord", 1, 2, sizeof(float) * 3}};
  device.BindPipeline(screenPipeline);  // Bind VAO before setting attributes
  device.SetVertexAttributes(fullscreenQuadVertexBuffer, quadAttr);
}

const RenderPass RenderResources::BuildPointPass() {
  RenderPass pass;

  pass.pipeline = geometryPipeline;
  pass.vertexBuffer = vertexBuffer;
  pass.topology = PrimitiveTopology::Points;
  pass.shaderProgram = pointShader;
  pass.frameBuffer = framebuffer0;
  pass.clearOnBind = true;
  pass.clearColor[0] = 0.0f;  // R
  pass.clearColor[1] = 0.0f;  // G
  pass.clearColor[2] = 0.0f;  // B
  pass.clearColor[3] = 0.0f;  // A
  pass.viewportX = 0;
  pass.viewportY = 0;
  pass.viewportWidth = fbWidth;
  pass.viewportHeight = fbHeight;

  return pass;
}

const RenderPass RenderResources::BuildFacePass() {
  RenderPass pass;

  pass.pipeline = facePipeline;          // Use face pipeline with expanded vertices
  pass.vertexBuffer = faceVertexBuffer;  // Use expanded vertex buffer
  pass.indexBuffer = 0;                  // No index buffer (non-indexed rendering)
  pass.topology = PrimitiveTopology::Triangles;
  pass.shaderProgram = basicShader;
  pass.frameBuffer = framebuffer2;
  pass.clearOnBind = true;
  pass.clearColor[0] = 0.0f;  // R
  pass.clearColor[1] = 0.0f;  // G
  pass.clearColor[2] = 0.0f;  // B
  pass.clearColor[3] = 0.0f;  // A
  pass.viewportX = 0;
  pass.viewportY = 0;
  pass.viewportWidth = fbWidth;
  pass.viewportHeight = fbHeight;

  return pass;
}

const RenderPass RenderResources::BuildGroundPlanePass() {
  RenderPass pass;

  pass.pipeline = screenPipeline;  // Use fullscreen quad pipeline
  pass.vertexBuffer = fullscreenQuadVertexBuffer;
  pass.indexBuffer = fullscreenQuadIndexBuffer;
  pass.topology = PrimitiveTopology::Triangles;
  pass.shaderProgram = groundPlaneShader;
  pass.frameBuffer = framebuffer3;
  pass.clearOnBind = true;
  pass.clearColor[0] = 0.0f;  // R
  pass.clearColor[1] = 0.0f;  // G
  pass.clearColor[2] = 0.0f;  // B
  pass.clearColor[3] = 0.0f;  // A = 0 means invalid (no geometry)
  pass.viewportX = 0;
  pass.viewportY = 0;
  pass.viewportWidth = fbWidth;
  pass.viewportHeight = fbHeight;

  return pass;
}

const RenderPass RenderResources::BuildWorldPosPass() {
  RenderPass pass;

  pass.pipeline = facePipeline;          // Use face pipeline with expanded vertices
  pass.vertexBuffer = faceVertexBuffer;  // Use expanded vertex buffer
  pass.indexBuffer = 0;                  // No index buffer (non-indexed rendering)
  pass.topology = PrimitiveTopology::Triangles;
  pass.shaderProgram = worldPosShader;
  pass.frameBuffer = framebuffer3;
  pass.clearOnBind = false;  // Don't clear - ground plane already rendered
  pass.viewportX = 0;
  pass.viewportY = 0;
  pass.viewportWidth = fbWidth;
  pass.viewportHeight = fbHeight;

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
  pass.clearColor[0] = 0.0f;  // R
  pass.clearColor[1] = 0.0f;  // G
  pass.clearColor[2] = 0.0f;  // B
  pass.clearColor[3] = 0.0f;  // A
  pass.viewportX = 0;
  pass.viewportY = 0;
  pass.viewportWidth = fbWidth;
  pass.viewportHeight = fbHeight;

  return pass;
}

const RenderPass RenderResources::BuildScreenPass() {
  RenderPass pass;

  pass.pipeline = screenPipeline;
  pass.vertexBuffer = fullscreenQuadVertexBuffer;
  pass.indexBuffer = fullscreenQuadIndexBuffer;
  pass.topology = PrimitiveTopology::Triangles;
  pass.shaderProgram = screenShader;
  pass.frameBuffer = 0;
  pass.clearOnBind = true;
  pass.clearColor[0] = .8f;   // R
  pass.clearColor[1] = .8f;   // G
  pass.clearColor[2] = .8f;   // B
  pass.clearColor[3] = 1.0f;  // A
  pass.viewportX = 0;
  pass.viewportY = 0;
  pass.viewportWidth = fbWidth;
  pass.viewportHeight = fbHeight;

  return pass;
}

const RenderPass RenderResources::BuildDebugPass() {
  RenderPass pass;

  pass.pipeline = screenPipeline;
  pass.vertexBuffer = fullscreenQuadVertexBuffer;
  pass.indexBuffer = fullscreenQuadIndexBuffer;
  pass.topology = PrimitiveTopology::Triangles;
  pass.shaderProgram = debugShader;
  pass.frameBuffer = 0;
  pass.clearOnBind = false;
  pass.blendEnabled = true;  // Enable alpha blending
  pass.viewportX = 0;
  pass.viewportY = 0;
  pass.viewportWidth = fbWidth;
  pass.viewportHeight = fbHeight;

  return pass;
}