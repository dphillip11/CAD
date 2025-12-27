#pragma once
#include <cstdint>
#include <string>

class RenderDevice;

class ShaderProgram {
 public:
  ShaderProgram() = delete;

  explicit ShaderProgram(RenderDevice& device, const std::string& vertexSource,
                         const std::string& fragmentSource);
  explicit ShaderProgram(RenderDevice& device, const std::string& vertexSource,
                         const std::string& geometrySource, const std::string& fragmentSource);

  ~ShaderProgram();

  ShaderProgram(const ShaderProgram&) = delete;
  ShaderProgram& operator=(const ShaderProgram&) = delete;

  uint32_t GetHandle() const noexcept { return handle_; }

  static std::string LoadSource(const std::string path);

 private:
  RenderDevice& device_;
  uint32_t handle_ = 0;
};