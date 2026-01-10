#pragma once

#include <optional>

#include "ModelView/ModelViewBuilder.h"
#include "ModelView/ModelViews.h"
#include "Rendering/Camera.h"
#include "Rendering/FrameContext.h"
#include "Rendering/Passes/RenderPass.h"
#include "Rendering/Resources/RenderResources.h"

class Model;
class CommandStack;
class Input;

class Renderer {
 public:
  explicit Renderer(RenderDevice& device, Model& model);
  ~Renderer();

  void Initialise();

  void ProcessPendingUpdates(const FrameContext& context, Input& input);
  void Render(const FrameContext& context);

  void Resize(uint32_t width, uint32_t height);
  void MarkDirty() { shouldUpdateUniforms_ = true; }

  Camera& GetCamera() { return camera_; }

  // Get the 3D world position at framebuffer coordinates (returns nullopt if no geometry)
  std::optional<Vec3> GetPickedWorldPosition(uint32_t fbX, uint32_t fbY);

 private:
  void UpdateVertices();
  void UpdateEdgeIndices();
  void UpdateFaceIndices();
  void UpdateVolumeIndices();
  void UpdateFrameContext(const FrameContext& context);
  void HandleViewportResize(uint32_t width, uint32_t height);
  void HandlePick(Input& input);

  RenderDevice& device_;
  Model& model_;
  Camera camera_;
  ModelViewBuilder viewBuilder_;
  ModelViews views_;
  RenderResources resources_;

  bool shouldUpdateUniforms_ = true;
  uint32_t lastViewportWidth_ = 0;
  uint32_t lastViewportHeight_ = 0;

  RenderPass pointPass_;
  RenderPass linePass_;
  RenderPass facePass_;
  RenderPass groundPlanePass_;  // Render ground plane to world pos texture
  RenderPass worldPosPass_;     // Render world positions to texture
  RenderPass screenPass_;
  RenderPass debugPass_;

  // Pending pick request (to be processed after face rendering)
  bool hasPendingPick_ = false;
  uint32_t pendingPickX_ = 0;
  uint32_t pendingPickY_ = 0;
  uint32_t selectedFaceId_ = 0;
};
