#pragma once

#include <memory>

#include "Rendering/Passes/RenderPass.h"
#include "Rendering/Resources/RenderResources.h"

class FrameContext;
struct ModelViews;
class Model;

class Renderer {
 public:
  explicit Renderer(RenderDevice& device);
  ~Renderer();

  void Initialise();

  void Render(const ModelViews& views, const Model& model);

  RenderResources resources;

  void Resize(uint32_t width, uint32_t height);
  void UpdateVertices(const Model& model);
  void UpdateEdgeIndices(const ModelViews& views);
  void UpdateFaceIndices(const ModelViews& views);
  void UpdateVolumeIndices(const ModelViews& views);
  void UpdateFrameContext(const FrameContext& context);

 private:
  RenderDevice& device_;
  RenderResources resources_;

  RenderPass pointPass_;
  RenderPass linePass_;
  RenderPass facePass_;
  RenderPass screenPass_;
};
