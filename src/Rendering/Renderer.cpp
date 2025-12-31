#include "Renderer.h"

#include <iostream>

#include "Model/Model.h"
#include "ModelView/ModelViews.h"
#include "Rendering/Devices/RenderDevice.h"
#include "Rendering/FrameContext.h"
#include "Rendering/Resources/RenderResources.h"
#include "Rendering/Resources/UniformBuffer.h"
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
  // Process input first
  ProcessInput(context.input);

  // Check for viewport resize
  if (context.viewportWidth != 0 && context.viewportHeight != 0) {
    if (context.viewportWidth != lastViewportWidth_ ||
        context.viewportHeight != lastViewportHeight_) {
      HandleViewportResize(context.viewportWidth, context.viewportHeight);
    }
  }

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

  if (shouldUpdateUniforms_) {
    UpdateFrameContext(context);
  }
}

void Renderer::Render() {
  if (!model_.ShouldRender() && !shouldUpdateUniforms_) {
    return;
  }

  device_.BeginFrame();

  pointPass_.Execute(device_, model_.Vertices().size());
  facePass_.Execute(device_, views_.faces.vertexIndices.size());
  linePass_.Execute(device_, views_.lines.vertexIndices.size());
  screenPass_.Execute(device_, 6);

  device_.EndFrame();

  shouldUpdateUniforms_ = false;
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

void Renderer::UpdateFrameContext(const FrameContext& context) {
  // Update camera matrices and viewport size in uniform buffer
  UniformBuffer uniforms;
  uniforms.viewMatrix = camera_.GetViewMatrix();
  uniforms.projectionMatrix = camera_.GetProjectionMatrix();

  // Update viewport size for geometry shaders
  uniforms.viewPortSize[0] = static_cast<float>(context.viewportWidth);
  uniforms.viewPortSize[1] = static_cast<float>(context.viewportHeight);

  device_.UpdateUniformBuffer(resources_.frameUniformBuffer, sizeof(UniformBuffer), &uniforms, 0);
}

void Renderer::ProcessInput(const FrameContext::InputState& input) {
  // Orbit with left mouse button
  if (input.mouseLeftDown && (input.mouseDeltaX != 0.0f || input.mouseDeltaY != 0.0f)) {
    camera_.Orbit(input.mouseDeltaX, input.mouseDeltaY);
    shouldUpdateUniforms_ = true;
  }

  // Pan with middle mouse button
  if (input.mouseMiddleDown && (input.mouseDeltaX != 0.0f || input.mouseDeltaY != 0.0f)) {
    camera_.Pan(input.mouseDeltaX, input.mouseDeltaY);
    shouldUpdateUniforms_ = true;
  }

  // Zoom with scroll wheel
  if (input.scrollDelta != 0.0f) {
    camera_.Zoom(input.scrollDelta);
    shouldUpdateUniforms_ = true;
  }
}

void Renderer::HandleViewportResize(uint32_t width, uint32_t height) {
  // Update screen pass viewport to match window size
  screenPass_.viewportWidth = width;
  screenPass_.viewportHeight = height;

  // Update camera aspect ratio to match window
  float aspect = static_cast<float>(width) / static_cast<float>(height);
  camera_.SetAspectRatio(aspect);

  // Track dimensions and flag uniforms for update
  lastViewportWidth_ = width;
  lastViewportHeight_ = height;
  shouldUpdateUniforms_ = true;
}
