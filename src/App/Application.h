#pragma once

#include <memory>

#include "Model/Model.h"
#include "ModelView/ModelViewBuilder.h"
#include "ModelView/ModelViews.h"
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
  ModelViewBuilder viewBuilder{model};
  ModelViews views;
  RenderDeviceType device;
  std::unique_ptr<Renderer> renderer;
  FrameContext ctx;
};