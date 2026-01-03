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

  // Handle picking on right mouse click
  static bool wasRightMouseDown = false;
  if (context.input.mouseRightDown) {
    // Only pick on first frame of click (detect click event)
    if (!wasRightMouseDown) {
      std::cout << "Right click detected at (" << context.input.mouseX << ", "
                << context.input.mouseY << ")" << std::endl;
      // Store pick request to be processed after rendering
      hasPendingPick_ = true;
      pendingPickX_ = static_cast<uint32_t>(context.input.mouseX);
      pendingPickY_ = static_cast<uint32_t>(context.input.mouseY);
    }
    wasRightMouseDown = true;
  } else {
    wasRightMouseDown = false;
  }

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
  // Always render if there's a pending pick (need fresh framebuffer data)
  if (!model_.ShouldRender() && !shouldUpdateUniforms_ && !hasPendingPick_) {
    return;
  }

  device_.BeginFrame();

  // Render geometry to framebuffers
  pointPass_.Execute(device_, model_.Vertices().size());
  facePass_.Execute(device_, views_.faces.vertices.size());  // Use vertices count (non-indexed)
  linePass_.Execute(device_, views_.lines.vertexIndices.size());

  // Handle pending pick AFTER geometry is rendered but BEFORE screen composite
  if (hasPendingPick_) {
    HandlePick(pendingPickX_, pendingPickY_);
    hasPendingPick_ = false;
  }

  // Composite to screen
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

void Renderer::HandlePick(uint32_t mouseX, uint32_t mouseY) {
  // we use top right and left for face selection
  float normalizedX = 2.0f * static_cast<float>(mouseX) / static_cast<float>(lastViewportWidth_);
  float normalizedY = 2.0f * static_cast<float>(mouseY) / static_cast<float>(lastViewportHeight_);
  if (normalizedX > 1.0f) normalizedX -= 1.0f;
  if (normalizedY > 1.0f) normalizedY -= 1.0f;

  // Convert to framebuffer pixel coordinates
  // Note: Framebuffer might be different resolution than window (e.g., retina displays)
  uint32_t fbX = static_cast<uint32_t>(normalizedX * lastViewportWidth_);
  uint32_t fbY =
      static_cast<uint32_t>((1.0f - normalizedY) * lastViewportHeight_);  // Flip Y for OpenGL

  std::cout << "Pick at window(" << mouseX << "," << mouseY << ") -> "
            << "normalized(" << normalizedX << "," << normalizedY << ") -> "
            << "fb(" << fbX << "," << fbY << ")" << std::endl;

  // Bind the face framebuffer to read from (texture2)
  device_.BindFrameBuffer(resources_.framebuffer2);

  // Read RGBA pixel as bytes
  uint8_t pixel[4];
  device_.ReadPixel(fbX, fbY, pixel);

  std::cout << "Pixel bytes: R=" << (int)pixel[0] << " G=" << (int)pixel[1]
            << " B=" << (int)pixel[2] << " A=" << (int)pixel[3] << std::endl;

  uint32_t faceId = (pixel[0] << 8) | pixel[1];

  if (faceId != 0) {
    std::cout << "Picked face ID: " << faceId << std::endl;
  } else {
    std::cout << "No face at pick location (background)" << std::endl;
  }
}
