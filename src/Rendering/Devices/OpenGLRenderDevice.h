#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "Rendering/Devices/RenderDevice.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#define GLFW_INCLUDE_ES3
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

struct VertexAttribute;

class OpenGLRenderDevice : public RenderDevice {
 public:
  OpenGLRenderDevice(int width = 800, int height = 600);
  ~OpenGLRenderDevice() override;

  // ----- Resource creation -----
  uint32_t CreateVertexBuffer(std::size_t sizeBytes) override;
  uint32_t CreateIndexBuffer(std::size_t sizeBytes) override;
  uint32_t CreateShader(const std::string& vertexSource, const std::string& fragmentSource,
                        const std::string& geometrySource = "") override;

  void DestroyBuffer(uint32_t handle) override;
  void DestroyShader(uint32_t handle) override;

  // ----- Buffer updates -----
  void UpdateVertexBuffer(uint32_t handle, std::span<const std::byte> data) override;
  void UpdateIndexBuffer(uint32_t handle, std::span<const uint32_t> indices) override;

  // ----- Binding -----
  void BindVertexBuffer(uint32_t handle) override;
  void SetVertexAttributes(uint32_t handle, const std::span<VertexAttribute>& attributes) override;
  void BindIndexBuffer(uint32_t handle) override;
  void BindShader(uint32_t shaderHandle) override;

  // ----- Uniforms -----
  void SetUniform(const std::string& name, const Vec3& vec) override;
  void SetUniform(const std::string& name, const float valueA, const float valueB) override;
  void SetUniform(const std::string& name, const Mat4& matrix) override;
  void SetUniform(const std::string& name, int value) override;
  void SetUniform(const std::string& name, float value) override;

  // ----- Draw -----
  void DrawIndexed(PrimitiveTopology topology, std::size_t indexCount) override;

  // ----- Frame control -----
  void BeginFrame() override;
  void EndFrame() override;

  // ----- Window management -----
  GLFWwindow* GetWindow() const { return window_; }
  bool ShouldClose() const;
  void PollEvents();

  // ----- Emscripten WebGL initialization -----
  void InitializeWebGL();

 private:
  struct BufferInfo {
    GLuint id = 0;
    GLenum target = 0;
    std::size_t size = 0;
  };

  struct ShaderInfo {
    GLuint program = 0;
    std::unordered_map<std::string, GLint> uniformLocations;
  };

  GLFWwindow* window_ = nullptr;
  int width_ = 800;
  int height_ = 600;
  std::unordered_map<uint32_t, BufferInfo> buffers_;
  std::unordered_map<uint32_t, ShaderInfo> shaders_;
  uint32_t nextHandle_ = 1;

  // Current state
  uint32_t currentVertexBuffer_ = 0;
  uint32_t currentIndexBuffer_ = 0;
  uint32_t currentShader_ = 0;
  uint32_t currentVAO_ = 0;

  void InitializeOpenGL();
  GLuint CompileShader(GLenum type, const std::string& source);
  GLuint CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource,
                             const std::string& geometrySource);
  GLenum TopologyToGLenum(PrimitiveTopology topology) const;
  std::string GetErrorString(GLenum error) const;
  GLint GetUniformLocation(const std::string& name);
};