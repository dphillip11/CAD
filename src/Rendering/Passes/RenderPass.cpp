#include "RenderPass.h"

#include <iostream>

#include "Rendering/Devices/RenderDevice.h"

void RenderPass::Execute(RenderDevice& device, size_t count) const {
  device.BindPipeline(pipeline);
  device.BindVertexBuffer(vertexBuffer);
  if (indexBuffer != 0) {
    device.BindIndexBuffer(indexBuffer);
  }
  device.BindFrameBuffer(frameBuffer);

  // Set viewport if specified
  if (viewportWidth > 0 && viewportHeight > 0) {
    device.SetViewport(viewportX, viewportY, viewportWidth, viewportHeight);
  }

  // Apply clear settings if configured
  if (clearOnBind) {
    device.SetClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    device.Clear();
  }

  device.BindShader(shaderProgram);

  if (indexBuffer != 0) {
    device.DrawIndexed(topology, count);
  } else {
    device.Draw(topology, count);
  }
}
