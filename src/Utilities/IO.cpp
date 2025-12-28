#include "IO.h"

#include <fstream>
#include <sstream>

namespace IO {
std::string LoadSource(const std::string path) {
  std::ifstream file("src/Rendering/GLSL/" + path);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open shader source file: " + path);
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}
}  // namespace IO
