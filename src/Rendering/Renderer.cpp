#include "Renderer.h"

#include "Model/Model.h"
#include "ModelView/ModelViews.h"
#include "Rendering/Devices/RenderDevice.h"
#include "Rendering/FrameContext.h"
#include "Rendering/Passes/FacePass.h"
#include "Rendering/Passes/LinePass.h"
#include "Rendering/Passes/ScreenPass.h"
#include "Rendering/Resources/IndexBuffer.h"
#include "Rendering/Resources/RenderResources.h"
#include "Rendering/Resources/ShaderProgram.h"
#include "Rendering/Resources/VertexBuffer.h"
#include "Utilities/Vec3.h"

Renderer::Renderer(RenderDevice& device) : device_(device) { Initialise(); }

Renderer::~Renderer() = default;

void Renderer::Initialise() {
  resources_.LoadResources(device_);
  linePass_ = std::make_unique<LinePass>(device_, resources_);
  facePass_ = std::make_unique<FacePass>(device_, resources_);
  screenPass_ = std::make_unique<ScreenPass>(device_, resources_);
}

void Renderer::Render(const ModelViews& views, const Model& model, const FrameContext& ctx) {
  device_.BeginFrame();
  device_.BindPipeline(resources_.sharedVertexPipeline);
  UploadVerticesIfNeeded(model);
  UploadIndicesIfNeeded(views);

  device_.BindFrameBuffer(resources_.combinedFramebufferHandle);

  device_.BindFrameBuffer(0);
  linePass_->Execute(device_, resources_, ctx);
  facePass_->Execute(device_, resources_, ctx);

  device_.BindPipeline(resources_.screenVertexPipeline);
  screenPass_->Execute(device_, resources_, ctx);

  device_.EndFrame();
}

void Renderer::UploadVerticesIfNeeded(const Model& model) {
  if (!verticesDirty) return;
  auto& vertexBuffer = *resources_.vertexBuffer;

  const auto& vertices = model.Vertices();
  static_assert(sizeof(Vertex) == sizeof(Vec3),
                "Vertex must have same size as Vec3 for zero-copy upload");
  std::span<const Vec3> positions(reinterpret_cast<const Vec3*>(vertices.data()), vertices.size());
  vertexBuffer.Upload(positions);
  verticesDirty = false;
}

void Renderer::SetIndicesDirty() { indicesDirty = true; }

void Renderer::UploadIndicesIfNeeded(const ModelViews& views) {
  if (!indicesDirty) {
    return;
  }

  // Upload edge indices
  if (!views.lines.vertexIndices.empty()) {
    auto& edgeBuffer = *resources_.edgeIndexBuffer;
    edgeBuffer.Upload(views.lines.vertexIndices);
  }

  // Upload face indices
  if (!views.faces.vertexIndices.empty()) {
    auto& faceBuffer = *resources_.faceIndexBuffer;
    faceBuffer.Upload(views.faces.vertexIndices);
  }

  // Upload volume indices
  if (!views.volumes.vertexIndices.empty()) {
    auto& volumeBuffer = *resources_.volumeIndexBuffer;
    volumeBuffer.Upload(views.volumes.vertexIndices);
  }

  indicesDirty = false;
}
