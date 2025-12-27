#include <iostream>

#include "App/Application.h"

auto main() -> int {
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
}
