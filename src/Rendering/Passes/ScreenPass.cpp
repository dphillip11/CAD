#include "ScreenPass.h"

#include <cassert>
#include <iostream>

#include "Rendering/FrameContext.h"
#include "Rendering/Resources/RenderResources.h"
#include "Rendering/Resources/ShaderProgram.h"
#include "Rendering/Resources/VertexBuffer.h"

ScreenPass::ScreenPass(RenderDevice& device, RenderResources& resources)
    : shaderHandle_(resources.renderToScreenShader->GetHandle()),
      color_({0, 1, 1}),
      vertexBufferHandle_(resources.fullscreenQuadVertexBuffer->GetHandle()),
      indexBuffer_(*resources.fullscreenQuadIndexBuffer) {
  device.BindShader(shaderHandle_);
  device.SetUniform("textureSampler", 0);
}

void ScreenPass::Execute(RenderDevice& device, RenderResources& resources,
                         const FrameContext& context) {
  device.BindTexture(resources.combinedRenderTextureHandle);
  device.BindShader(shaderHandle_);
  device.BindVertexBuffer(vertexBufferHandle_);
  device.BindIndexBuffer(indexBuffer_.GetHandle());
  device.DrawIndexed(PrimitiveTopology::Triangles, indexBuffer_.IndexCount());
}