#pragma once

#include <memory>

#include "ModelView/ModelViewBuilder.h"
#include "ModelView/ModelViews.h"
#include "Rendering/Passes/RenderPass.h"
#include "Rendering/Resources/RenderResources.h"

class FrameContext;
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

  RenderDevice& device_;
  const Model& model_;
  ModelViewBuilder viewBuilder_;
  ModelViews views_;
  RenderResources resources_;

  RenderPass pointPass_;
  RenderPass linePass_;
  RenderPass facePass_;
  RenderPass screenPass_;
};
