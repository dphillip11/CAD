#include "Renderer.h"

#include <iostream>

#include "Model/Model.h"
#include "ModelView/ModelViews.h"
#include "Rendering/Devices/RenderDevice.h"
#include "Rendering/FrameContext.h"
#include "Rendering/Resources/RenderResources.h"
#include "Utilities/Vec3.h"

Renderer::Renderer(RenderDevice& device, const Model& model)
    : device_(device), model_(model), viewBuilder_(model) {
  Initialise();
}

Renderer::~Renderer() = default;

void Renderer::Initialise() {
  resources_.LoadResources(device_);
  pointPass_ = resources_.BuildPointPass();
  facePass_ = resources_.BuildFacePass();
  linePass_ = resources_.BuildLinePass();
  screenPass_ = resources_.BuildScreenPass();
}

void Renderer::ProcessPendingUpdates(const FrameContext& context) {
  if (model_.IsVerticesDirty()) {
    UpdateVertices();
  }
  if (model_.IsEdgesDirty()) {
    viewBuilder_.BuildLineView(views_.lines);
    UpdateEdgeIndices();
  }
  if (model_.IsFacesDirty()) {
    viewBuilder_.BuildFaceView(views_.faces);
    UpdateFaceIndices();
  }
  if (model_.IsVolumesDirty()) {
    viewBuilder_.BuildVolumeView(views_.volumes);
    UpdateVolumeIndices();
  }
  if (model_.IsFrameContextDirty()) {
    UpdateFrameContext(context);
  }
}

void Renderer::Render() {
  if (!model_.ShouldRender()) {
    return;
  }

  device_.BeginFrame();

  pointPass_.Execute(device_, model_.Vertices().size());
  facePass_.Execute(device_, views_.faces.vertexIndices.size());
  linePass_.Execute(device_, views_.lines.vertexIndices.size());
  screenPass_.Execute(device_, 6);

  device_.EndFrame();
}

void Renderer::UpdateVertices() {
  const auto& vertices = model_.Vertices();
  device_.UpdateVertexBuffer(resources_.vertexBuffer, vertices.size() * sizeof(Vertex),
                             vertices.data());
}

void Renderer::UpdateEdgeIndices() {
  // Upload edge indices
  if (!views_.lines.vertexIndices.empty()) {
    device_.UpdateIndexBuffer(resources_.edgeIndexBuffer, views_.lines.vertexIndices);
  }
}

void Renderer::UpdateFaceIndices() {
  // Upload face indices
  if (!views_.faces.vertexIndices.empty()) {
    device_.UpdateIndexBuffer(resources_.faceIndexBuffer, views_.faces.vertexIndices);
  }
}

void Renderer::UpdateVolumeIndices() {
  // Upload volume indices
  if (!views_.volumes.vertexIndices.empty()) {
    device_.UpdateIndexBuffer(resources_.volumeIndexBuffer, views_.volumes.vertexIndices);
  }
}

void Renderer::UpdateFrameContext(const FrameContext& context) {}
