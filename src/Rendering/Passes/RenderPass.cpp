#include "RenderPass.h"

#include "RenderPassConfiguration.h"
#include "Rendering/Devices/RenderDevice.h"

RenderPass::RenderPass(RenderDevice& device, const RenderPassConfiguration& config)
    : device_(device),
      shaderHandle(config.shaderHandle),
      frameBufferHandle(config.frameBufferHandle),
      vertexBufferHandle(config.vertexBufferHandle),
      indexBufferHandle(config.indexBufferHandle),
      topology(config.topology),
      rgb(config.rgb) {}

void RenderPass::Execute(const FrameContext& context, uint32_t count) {
  device_.BindShader(shaderHandle);
  device_.SetUniform("Color", rgb);
  device_.BindVertexBuffer(vertexBufferHandle);
  device_.BindIndexBuffer(indexBufferHandle);
  // if (frameBufferHandle != 0)
  // {
  //     device_.BindFrameBuffer(frameBufferHandle);
  // }
  device_.DrawIndexed(topology, count);
  // TODO: Handle non indexed
}
