#include <iostream>

#include "App/Application.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

auto main() -> int {
  auto app = Application();

  std::cout << "application starting" << std::endl;
  if (!app.Start())
  {
    std::cout << "failed to start application" << std::endl;
    return 1;
  }

  std::cout << "application initialised" << std::endl;
  int count = 0;
  while(app.Run())
  {
    ++count;
  }

  std::cout << "application stopped running after " << count << " loops" << std::endl;

  if (!app.Exit())
  {
    std::cout << "application failed to terminate gracefully" << std::endl;
    return 1;
  }

  std::cout << "application exitted successfully" << std::endl;

  return 0;
}
