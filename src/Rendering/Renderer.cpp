#include "Renderer.h"

#include <iostream>

#include "Model/Model.h"
#include "Rendering/Devices/RenderDevice.h"
#include "Rendering/Passes/RenderPassConfiguration.h"
#include "Rendering/Resources/IndexBuffer.h"
#include "Rendering/Resources/RenderResources.h"
#include "Rendering/Resources/ShaderProgram.h"
#include "Rendering/Resources/VertexBuffer.h"
#include "Utilities/Vec3.h"

Renderer::Renderer(RenderDevice& device) : device_(device), resources_(), passes_() {
  std::cerr << "Renderer constructor called" << std::endl;
  Initialise();
}

void Renderer::Initialise() {
  resources_.LoadResources(device_);

  auto passConfig = RenderPassConfiguration();
  passConfig.vertexBufferHandle = resources_.vertexBuffer->GetHandle();
  passConfig.indexBufferHandle = resources_.faceIndexBuffer->GetHandle();
  passConfig.topology = PrimitiveTopology::Triangles;
  passConfig.shaderHandle = resources_.basicShader->GetHandle();
  passConfig.rgb = {0, 0, 1};
  passes_.emplace_back(device_, passConfig);

  passConfig.indexBufferHandle = resources_.edgeIndexBuffer->GetHandle();
  passConfig.topology = PrimitiveTopology::Lines;
  passConfig.shaderHandle = resources_.lineShader->GetHandle();
  passConfig.rgb = {0, 1, 0};
  passes_.emplace_back(device_, passConfig);

  device_.BindShader(passConfig.shaderHandle);
  device_.SetUniform("uThickness", 2.0f);
  device_.SetUniform("uViewportSize", 400.0, 400.0);
}

void Renderer::Render(const ModelViews& views, const Model& model, const FrameContext& ctx) {
  device_.BeginFrame();
  UploadVerticesIfNeeded(model);
  UploadIndicesIfNeeded(views);

  for (auto& pass : passes_) {
    const auto count = (pass.topology == PrimitiveTopology::Points) ? model.Vertices().size()
                       : (pass.topology == PrimitiveTopology::Lines)
                           ? views.lines.vertexIndices.size()
                           : views.faces.vertexIndices.size();

    pass.Execute(ctx, count);
  }

  device_.EndFrame();
}

void Renderer::UploadVerticesIfNeeded(const Model& model) {
  if (!verticesDirty) return;
  auto& vertexBuffer = *resources_.vertexBuffer;

  const auto& vertices = model.Vertices();
  std::vector<Vec3> positions;
  positions.reserve(vertices.size());
  for (const auto& v : vertices) {
    positions.push_back(v.position);
  }

  std::cerr << "Uploading " << positions.size() << " vertices" << std::endl;
  vertexBuffer.Upload(positions);
  std::cerr << "Vertices uploaded successfully" << std::endl;
  verticesDirty = false;
}

void Renderer::SetIndicesDirty() {
  std::cout << "marking indices dirty" << std::endl;
  indicesDirty = true;
}

void Renderer::UploadIndicesIfNeeded(const ModelViews& views) {
  if (!indicesDirty) {
    return;
  }

  // Upload edge indices (lines)
  if (!views.lines.vertexIndices.empty()) {
    std::cerr << "Uploading " << views.lines.vertexIndices.size() << " edge indices" << std::endl;
    auto& edgeBuffer = *resources_.edgeIndexBuffer;
    edgeBuffer.Upload(views.lines.vertexIndices);
    std::cerr << "Edge indices uploaded successfully" << std::endl;
  } else {
    std::cerr << "No edge indices to upload" << std::endl;
  }

  // Upload face indices
  if (!views.faces.vertexIndices.empty()) {
    std::cerr << "Uploading " << views.faces.vertexIndices.size() << " face indices" << std::endl;
    auto& faceBuffer = *resources_.faceIndexBuffer;
    faceBuffer.Upload(views.faces.vertexIndices);
    std::cerr << "Face indices uploaded successfully" << std::endl;
  } else {
    std::cerr << "No face indices to upload" << std::endl;
  }

  // Upload volume indices
  if (!views.volumes.vertexIndices.empty()) {
    std::cerr << "Uploading " << views.volumes.vertexIndices.size() << " volume indices"
              << std::endl;
    auto& volumeBuffer = *resources_.volumeIndexBuffer;
    volumeBuffer.Upload(views.volumes.vertexIndices);
    std::cerr << "Volume indices uploaded successfully" << std::endl;
  } else {
    std::cerr << "No volume indices to upload" << std::endl;
  }

  indicesDirty = false;
  std::cerr << "UploadIndicesIfNeeded: COMPLETED" << std::endl;
}
