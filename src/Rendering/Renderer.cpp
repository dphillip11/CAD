#include "Renderer.h"

#include <iostream>

#include "App/Commands/CommandStack.h"
#include "App/Commands/Commands.h"
#include "App/Input.h"
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

void Renderer::ProcessPendingUpdates(const FrameContext& context, Input& input) {
  // Check for viewport resize
  if (context.viewportWidth != 0 && context.viewportHeight != 0) {
    if (context.viewportWidth != lastViewportWidth_ ||
        context.viewportHeight != lastViewportHeight_) {
      HandleViewportResize(context.viewportWidth, context.viewportHeight);
    }
  }

  // Check if camera changed
  if (camera_.IsDirty()) {
    shouldUpdateUniforms_ = true;
    camera_.ClearDirty();
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

  // Handle pending pick after geometry updates
  HandlePick(input);

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
  device_.BindTexture(resources_.faceMaterialTexture, 7);
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

  // Upload face material data to texture (one texel per face)
  if (!views_.faces.colorIndices.empty()) {
    size_t faceCount = views_.faces.colorIndices.size();
    if (faceCount != resources_.faceMaterialTextureWidth) {
      if (resources_.faceMaterialTextureWidth > 0) {
        device_.DestroyTexture(resources_.faceMaterialTexture);
      }
      resources_.faceMaterialTexture = device_.CreateTexture2D(faceCount, 1, false);
      resources_.faceMaterialTextureWidth = faceCount;
    }
    std::vector<uint8_t> materialData;
    materialData.reserve(faceCount * 4);
    for (size_t i = 0; i < faceCount; ++i) {
      materialData.push_back(views_.faces.colorIndices[i]);
      materialData.push_back(views_.faces.roughness[i]);
      materialData.push_back(views_.faces.metallicity[i]);
      materialData.push_back(255);  // Alpha
    }
    device_.UpdateTexture2D(resources_.faceMaterialTexture, faceCount, 1, materialData);
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
  uniforms.maxFaces = views_.faces.primitiveCount;

  device_.UpdateUniformBuffer(resources_.frameUniformBuffer, sizeof(UniformBuffer), &uniforms, 0);
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

void Renderer::HandlePick(Input& input) {
  // Check if there's a pending pick request
  if (!input.IsPressed(KEYS::MOUSE_RIGHT)) {
    return;
  }

  uint32_t mouseX = static_cast<uint32_t>(input.GetMouseX());
  uint32_t mouseY = static_cast<uint32_t>(input.GetMouseY());

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

  uint32_t pickedFaceId = (pixel[0] << 8) | pixel[1];

  // Update both renderer's internal state and the input state
  selectedFaceId_ = pickedFaceId;
  input.SetSelectedFaceId(pickedFaceId);
  shouldUpdateUniforms_ = true;

  if (pickedFaceId != 0) {
    std::cout << "Picked face ID: " << pickedFaceId - 1 << std::endl;
  }
}
