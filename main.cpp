#include <iostream>

#include "App/Application.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

// Global application instance for Emscripten main loop
Application* g_app = nullptr;

// Main loop iteration function for Emscripten
void emscripten_main_loop() {
  if (g_app && !g_app->Run()) {
    emscripten_cancel_main_loop();
    if (!g_app->Exit()) {
      std::cout << "application failed to terminate gracefully" << std::endl;
    }
    std::cout << "application EXITTED successfully" << std::endl;
    delete g_app;
    g_app = nullptr;
  }
}
#endif

auto main() -> int {
#ifdef __EMSCRIPTEN__
  // For Emscripten, allocate on heap to ensure lifetime persists
  g_app = new Application();

  std::cout << "application STARTING" << std::endl;
  if (!g_app->Start()) {
    std::cout << "failed to start application" << std::endl;
    delete g_app;
    g_app = nullptr;
    return 1;
  }

  std::cout << "application INITIALISED" << std::endl;

  // Use the main loop callback instead of a while loop
  emscripten_set_main_loop(emscripten_main_loop, 0, 1);
  // Note: emscripten_set_main_loop never returns when simulate_infinite_loop=1
  return 0;
#else
  // Native platform: use stack allocation and traditional while loop
  auto app = Application();

  std::cout << "application STARTING" << std::endl;
  if (!app.Start()) {
    std::cout << "failed to start application" << std::endl;
    return 1;
  }

  std::cout << "application INITIALISED" << std::endl;

  int count = 0;
  while (app.Run()) {
    ++count;
  }
  std::cout << "application STOPPED running after " << count << " loops" << std::endl;

  if (!app.Exit()) {
    std::cout << "application failed to terminate gracefully" << std::endl;
    return 1;
  }

  std::cout << "application EXITTED successfully" << std::endl;

  return 0;
#endif
}
