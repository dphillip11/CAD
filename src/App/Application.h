#pragma once

#include "Model/Model.h"
#include "Rendering/Devices/RenderDevice.h"
#include "Rendering/FrameContext.h"
#include "Rendering/Renderer.h"

class Application {
 public:
  Application();
  ~Application();
  bool Start();
  void Debug();
  bool Run();
  bool Exit();

 private:
  Model model;
  RenderDevice device;
  Renderer renderer;
  FrameContext ctx;
};