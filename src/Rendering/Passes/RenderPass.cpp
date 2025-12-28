#include "RenderPass.h"

#include "Rendering/Devices/RenderDevice.h"

void RenderPass::Execute(RenderDevice& device, size_t count) const {
  device.BindPipeline(pipeline);
  device.BindVertexBuffer(vertexBuffer);
  device.BindIndexBuffer(indexBuffer);
  device.BindFrameBuffer(frameBuffer);
  device.BindShader(shaderProgram);
  device.DrawIndexed(topology, count);
}
