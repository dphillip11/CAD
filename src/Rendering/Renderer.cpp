#include "Renderer.h"

#include <iostream>

#include "App/Commands/CommandStack.h"
#include "App/Commands/Commands.h"
#include "Model/Model.h"
#include "ModelView/ModelViews.h"
#include "Rendering/Devices/RenderDevice.h"
#include "Rendering/FrameContext.h"
#include "Rendering/Resources/RenderResources.h"
#include "Rendering/Resources/UniformBuffer.h"
#include "Utilities/Vec3.h"

Renderer::Renderer(RenderDevice& device, Model& model)
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
  debugPass_ = resources_.BuildDebugPass();
}

void Renderer::ProcessPendingUpdates(const FrameContext& context, CommandStack& commandStack) {
  // Process input first
  ProcessInput(context.input, commandStack);

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
  if (model_.IsFacesDirty() || model_.IsVerticesDirty()) {
    viewBuilder_.BuildFaceView(views_.faces);
    UpdateFaceIndices();
  }
  if (model_.IsVolumesDirty()) {
    viewBuilder_.BuildVolumeView(views_.volumes);
    UpdateVolumeIndices();
  }

  // Handle pending pick AFTER geometry is rendered but BEFORE screen composite
  if (hasPendingPick_) {
    HandlePick(pendingPickX_, pendingPickY_);
    hasPendingPick_ = false;
  }

  if (shouldUpdateUniforms_) {
    UpdateFrameContext(context);
  }
}

void Renderer::Render(const FrameContext& context) {
  // Always render if there's a pending pick (need fresh framebuffer data)
  if (!model_.ShouldRender() && !shouldUpdateUniforms_ && !hasPendingPick_) {
    return;
  }

  device_.BeginFrame();

  // Render geometry to framebuffers
  pointPass_.Execute(device_, model_.Vertices().size());
  facePass_.Execute(device_, views_.faces.vertices.size());  // Use vertices count (non-indexed)
  linePass_.Execute(device_, views_.lines.vertexIndices.size());

  // Composite to screen
  screenPass_.Execute(device_, 6);

  if (context.debug) {
    debugPass_.Execute(device_, 6);
  }

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
  // Upload expanded face vertices (non-indexed rendering now)
  if (!views_.faces.vertices.empty()) {
    // Create a temporary buffer to match face vertices to the main vertex buffer
    // We need to upload these as Vec3 positions
    device_.UpdateVertexBuffer(resources_.faceVertexBuffer,
                               views_.faces.vertices.size() * sizeof(Vec3),
                               views_.faces.vertices.data());
  }

  // Upload face primitive IDs as vertex attribute (one per vertex)
  // Convert to float for GL_FLOAT vertex attribute
  if (!views_.faces.primitiveIds.empty()) {
    std::vector<float> primitiveIdsFloat;
    primitiveIdsFloat.reserve(views_.faces.primitiveIds.size());
    for (uint32_t id : views_.faces.primitiveIds) {
      primitiveIdsFloat.push_back(static_cast<float>(id));
    }
    device_.UpdateVertexBuffer(resources_.facePrimitiveIdBuffer,
                               primitiveIdsFloat.size() * sizeof(float), primitiveIdsFloat.data());
  }
}

void Renderer::UpdateVolumeIndices() {
  // Upload volume vertices (expanded geometry, non-indexed)
  if (!views_.volumes.vertices.empty()) {
    device_.UpdateVertexBuffer(resources_.vertexBuffer,
                               views_.volumes.vertices.size() * sizeof(Vec3),
                               views_.volumes.vertices.data());

    // Upload primitive IDs as vertex attributes
    // Convert to float for GL_FLOAT vertex attribute
    if (!views_.volumes.primitiveIds.empty()) {
      std::vector<float> primitiveIdsFloat;
      primitiveIdsFloat.reserve(views_.volumes.primitiveIds.size());
      for (uint32_t id : views_.volumes.primitiveIds) {
        primitiveIdsFloat.push_back(static_cast<float>(id));
      }
      device_.UpdateVertexBuffer(resources_.facePrimitiveIdBuffer,
                                 primitiveIdsFloat.size() * sizeof(float),
                                 primitiveIdsFloat.data());
    }
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
  uniforms.selectedFace = selectedFaceId_;

  device_.UpdateUniformBuffer(resources_.frameUniformBuffer, sizeof(UniformBuffer), &uniforms, 0);
}

void Renderer::ProcessInput(const FrameContext::InputState& input, CommandStack& commandStack) {
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

  // Zoom with scroll wheel (if no face selected)
  if (input.scrollDelta != 0.0f && selectedFaceId_ == 0) {
    camera_.Zoom(input.scrollDelta);
    shouldUpdateUniforms_ = true;
  }
  // Extrude selected face with scroll wheel
  else if (input.scrollDelta != 0.0f && selectedFaceId_ != 0) {
    // Use command stack for extrude
    commandStack.Do<ExtrudeFaceCommand>(selectedFaceId_ - 1, input.scrollDelta);
    std::cout << "Extruding face " << (selectedFaceId_ - 1) << " by " << input.scrollDelta
              << std::endl;
  }

  // Handle picking on right mouse click
  static bool wasRightMouseDown = false;
  if (input.mouseRightDown) {
    // Only pick on first frame of click (detect click event)
    if (!wasRightMouseDown) {
      // Store pick request to be processed after rendering
      hasPendingPick_ = true;
      pendingPickX_ = static_cast<uint32_t>(input.mouseX);
      pendingPickY_ = static_cast<uint32_t>(input.mouseY);
    }
    wasRightMouseDown = true;
  } else {
    wasRightMouseDown = false;
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

void Renderer::HandlePick(uint32_t mouseX, uint32_t mouseY) {
#ifndef __EMSCRIPTEN__
  // Mac retina thing it seems
  mouseX *= 2;
  mouseY *= 2;
#endif

  uint32_t fbX = mouseX;
  uint32_t fbY = lastViewportHeight_ - mouseY;

  // Bind the face framebuffer to read from (texture2)
  device_.BindFrameBuffer(resources_.framebuffer2);

  // Read RGBA pixel as bytes
  uint8_t pixel[4];
  device_.ReadPixel(fbX, fbY, pixel);

  selectedFaceId_ = (pixel[0] << 8) | pixel[1];
  shouldUpdateUniforms_ = true;

  if (selectedFaceId_ != 0) {
    std::cout << "Picked face ID: " << selectedFaceId_ - 1 << std::endl;
  }
}
