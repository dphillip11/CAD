#include "RenderPass.h"

#include <iostream>

#include "Rendering/Devices/RenderDevice.h"

void RenderPass::Execute(RenderDevice& device, size_t count) const {
  device.BindPipeline(pipeline);
  device.BindVertexBuffer(vertexBuffer);
  device.BindIndexBuffer(indexBuffer);
  device.BindFrameBuffer(frameBuffer);

  // Apply clear settings if configured
  if (clearOnBind && frameBuffer != 0) {
    std::cout << "Clearing FBO " << frameBuffer << " to (" << clearColor[0] << ", " << clearColor[1]
              << ", " << clearColor[2] << ", " << clearColor[3] << ")" << std::endl;
    device.SetClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    device.Clear();
  }

  device.BindShader(shaderProgram);
  device.DrawIndexed(topology, count);
}
