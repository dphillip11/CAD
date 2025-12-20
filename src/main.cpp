#include <iostream>
#include "hello.h"

int main() {
  std::cout << hello::get_message() << std::endl;
#ifdef __EMSCRIPTEN__
  // When compiled with Emscripten, also print a JS-friendly message
  std::cout << "(running in the browser via Emscripten)" << std::endl;
#endif
  return 0;
}
