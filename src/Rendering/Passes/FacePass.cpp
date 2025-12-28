#include "FacePass.h"

#include <cassert>
#include <iostream>

#include "Rendering/FrameContext.h"
#include "Rendering/Resources/RenderResources.h"
#include "Rendering/Resources/ShaderProgram.h"
#include "Rendering/Resources/VertexBuffer.h"

FacePass::FacePass(RenderDevice& device, RenderResources& resources)
    : shaderHandle_(resources.basicShader->GetHandle()),
      color_({0, 1, 1}),
      vertexBufferHandle_(resources.vertexBuffer->GetHandle()),
      indexBuffer_(*resources.faceIndexBuffer) {
  device.BindShader(shaderHandle_);
  device.SetUniform("Color", color_);
}

void FacePass::Execute(RenderDevice& device, RenderResources& resources,
                       const FrameContext& context) {
  device.BindShader(shaderHandle_);
  device.BindVertexBuffer(vertexBufferHandle_);
  device.BindIndexBuffer(indexBuffer_.GetHandle());
  device.DrawIndexed(PrimitiveTopology::Triangles, indexBuffer_.IndexCount());
}