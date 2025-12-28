#include "LinePass.h"

#include <cassert>
#include <iostream>

#include "Rendering/FrameContext.h"
#include "Rendering/Resources/RenderResources.h"
#include "Rendering/Resources/ShaderProgram.h"
#include "Rendering/Resources/VertexBuffer.h"

LinePass::LinePass(RenderDevice& device, RenderResources& resources)
    : shaderHandle_(resources.lineShader->GetHandle()),
      color_({0, 1, 0}),
      vertexBufferHandle_(resources.vertexBuffer->GetHandle()),
      indexBuffer_(*resources.edgeIndexBuffer) {
  device.BindShader(shaderHandle_);
  device.SetUniform("Color", color_);
  device.SetUniform("uThickness", 2.0f);
  device.SetUniform("uViewportSize", 400.0, 400.0);
}

void LinePass::Execute(RenderDevice& device, RenderResources& resources,
                       const FrameContext& context) {
  device.BindShader(shaderHandle_);
  device.BindVertexBuffer(vertexBufferHandle_);
  device.BindIndexBuffer(indexBuffer_.GetHandle());
  device.DrawIndexed(PrimitiveTopology::Lines, indexBuffer_.IndexCount());
}