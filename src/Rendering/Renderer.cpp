#include "Renderer.h"

#include <iostream>

#include "Model/Model.h"
#include "ModelView/ModelViews.h"
#include "Rendering/Devices/RenderDevice.h"
#include "Rendering/FrameContext.h"
#include "Rendering/Resources/RenderResources.h"
#include "Utilities/Vec3.h"

Renderer::Renderer(RenderDevice& device) : device_(device) { Initialise(); }

Renderer::~Renderer() = default;

void Renderer::Initialise() {
  resources_.LoadResources(device_);
  pointPass_ = resources_.BuildPointPass();
  facePass_ = resources_.BuildFacePass();
  linePass_ = resources_.BuildLinePass();
  screenPass_ = resources_.BuildScreenPass();
}

void Renderer::Render(const ModelViews& views, const Model& model) {
  device_.BeginFrame();

  pointPass_.Execute(device_, model.Vertices().size());
  facePass_.Execute(device_, views.faces.vertexIndices.size());
  linePass_.Execute(device_, views.lines.vertexIndices.size());
  screenPass_.Execute(device_, 6);

  device_.EndFrame();
}

void Renderer::UpdateVertices(const Model& model) {
  const auto& vertices = model.Vertices();
  device_.UpdateVertexBuffer(resources_.vertexBuffer, vertices.size() * sizeof(Vertex),
                             vertices.data());
}

void Renderer::UpdateEdgeIndices(const ModelViews& views) {
  // Upload edge indices
  if (!views.lines.vertexIndices.empty()) {
    device_.UpdateIndexBuffer(resources_.edgeIndexBuffer, views.lines.vertexIndices);
  }
}

void Renderer::UpdateFaceIndices(const ModelViews& views) {
  // Upload face indices
  if (!views.faces.vertexIndices.empty()) {
    device_.UpdateIndexBuffer(resources_.faceIndexBuffer, views.faces.vertexIndices);
  }
}

void Renderer::UpdateVolumeIndices(const ModelViews& views) {
  // Upload volume indices
  if (!views.volumes.vertexIndices.empty()) {
    device_.UpdateIndexBuffer(resources_.volumeIndexBuffer, views.volumes.vertexIndices);
  }
}

void Renderer::UpdateFrameContext(const FrameContext& context) {}
