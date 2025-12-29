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

  void Render(const ModelViews& views, const Model& model, const FrameContext& ctx);

  void Resize(uint32_t width, uint32_t height);

  void SetVerticesDirty() { verticesDirty = true; }

  void SetIndicesDirty();

  RenderResources resources;

 private:
  RenderDevice& device_;
  RenderResources resources_;

  RenderPass pointPass_;
  RenderPass linePass_;
  RenderPass facePass_;
  RenderPass screenPass_;

  bool verticesDirty;
  bool indicesDirty;

  void UploadVerticesIfNeeded(const Model& model);
  void UploadIndicesIfNeeded(const ModelViews& views);
};
