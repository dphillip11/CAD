#pragma once

#include "Model/Model.h"
#include "Rendering/FrameContext.h"
#include "Rendering/Renderer.h"

#ifdef USE_OPENGL
#include "Rendering/Devices/OpenGLRenderDevice.h"
using RenderDeviceType = OpenGLRenderDevice;
#endif

class Application {
 public:
  Application();
  ~Application();
  bool Start();
  bool Run();
  bool Exit();

 private:
  Model model;
  RenderDeviceType device;
  Renderer renderer;
  FrameContext ctx;
};