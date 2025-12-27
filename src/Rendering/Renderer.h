#pragma once

#include "Model/Model.h"
#include "ModelView/ModelViews.h"
#include "Rendering/FrameContext.h"
#include "Rendering/Passes/RenderPass.h"
#include "Rendering/Resources/RenderResources.h"

class Renderer {
 public:
  explicit Renderer(RenderDevice& device);

  void Initialise();

  void Render(const ModelViews& views, const Model& model, const FrameContext& ctx);

  void Resize(uint32_t width, uint32_t height);

  void SetVerticesDirty() { verticesDirty = true; }

  void SetIndicesDirty();

  RenderResources resources;

 private:
  RenderDevice& device_;
  RenderResources resources_;
  std::vector<RenderPass> passes_;
  bool verticesDirty;
  bool indicesDirty;

  void UploadVerticesIfNeeded(const Model& model);
  void UploadIndicesIfNeeded(const ModelViews& views);
};
