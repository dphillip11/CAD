#pragma once

#include "App/Commands/CommandStack.h"
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

  CommandStack& GetCommandStack() { return commandStack_; }

 private:
  Model model;
  CommandStack commandStack_;
  RenderDevice device;
  Renderer renderer;
  FrameContext ctx;
};