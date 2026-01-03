#pragma once

#include "ModelView/ModelViewBuilder.h"
#include "ModelView/ModelViews.h"
#include "Rendering/Camera.h"
#include "Rendering/FrameContext.h"
#include "Rendering/Passes/RenderPass.h"
#include "Rendering/Resources/RenderResources.h"

class Model;

class Renderer {
 public:
  explicit Renderer(RenderDevice& device, const Model& model);
  ~Renderer();

  void Initialise();

  void ProcessPendingUpdates(const FrameContext& context);
  void Render();

  void Resize(uint32_t width, uint32_t height);

 private:
  void UpdateVertices();
  void UpdateEdgeIndices();
  void UpdateFaceIndices();
  void UpdateVolumeIndices();
  void UpdateFrameContext(const FrameContext& context);
  void ProcessInput(const FrameContext::InputState& input);
  void HandleViewportResize(uint32_t width, uint32_t height);
  void HandlePick(uint32_t mouseX, uint32_t mouseY);

  RenderDevice& device_;
  const Model& model_;
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
  RenderPass screenPass_;

  // Pending pick request (to be processed after face rendering)
  bool hasPendingPick_ = false;
  uint32_t pendingPickX_ = 0;
  uint32_t pendingPickY_ = 0;
};
