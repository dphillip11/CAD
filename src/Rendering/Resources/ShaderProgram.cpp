#include "ShaderProgram.h"

#include <fstream>
#include <sstream>

#include "Rendering/Devices/RenderDevice.h"

ShaderProgram::ShaderProgram(RenderDevice& device, const std::string& vertexSource,
                             const std::string& fragmentSource)
    : device_(device) {
  handle_ = device_.CreateShader(vertexSource, fragmentSource);
}

ShaderProgram::ShaderProgram(RenderDevice& device, const std::string& vertexSource,
                             const std::string& geometrySource, const std::string& fragmentSource)
    : device_(device) {
  handle_ = device_.CreateShader(vertexSource, geometrySource, fragmentSource);
}

ShaderProgram::~ShaderProgram() { device_.DestroyShader(handle_); }

std::string ShaderProgram::LoadSource(const std::string path) {
  std::ifstream file("src/Rendering/GLSL/" + path);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open shader source file: " + path);
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}
