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
  GpuHandle CreatePipeline() override;
  GpuHandle CreateBuffer() override;
  GpuHandle CreateShader(const std::string& vertexSource, const std::string& fragmentSource,
                         const std::string& geometrySource = "") override;
  GpuHandle CreateTexture2D(const float width, const float height, bool generateMipmaps) override;
  GpuHandle CreateFrameBuffer(GpuHandle colorHandle, GpuHandle depthHandle,
                              GpuHandle stencilHandle) override;

  void DestroyBuffer(GpuHandle handle) override;
  void DestroyShader(GpuHandle handle) override;

  // ----- Buffer updates -----
  void UpdateVertexBuffer(GpuHandle handle, const size_t bytes, const void* data) override;
  void UpdateUniformBuffer(GpuHandle handle, const size_t bytes, const void* data,
                           const uint32_t position) override;
  void UpdateIndexBuffer(GpuHandle handle, std::span<const uint32_t> indices) override;

  // ----- Binding -----
  void BindPipeline(GpuHandle) override;
  void BindVertexBuffer(GpuHandle handle) override;
  void SetVertexAttributes(GpuHandle handle, const std::span<VertexAttribute>& attributes) override;
  void BindIndexBuffer(GpuHandle handle) override;
  void BindTexture(GpuHandle handle, const uint32_t index) override;
  void BindFrameBuffer(GpuHandle handle) override;
  void BindShader(GpuHandle shaderHandle) override;

  // ----- Uniforms -----
  // template <typename T>
  // UniformHandle SetUniform(const std::string& name, const T& vec);
  void SetUniform(const std::string& name, const Vec3& vec) override;
  void SetUniform(const std::string& name, const float valueA, const float valueB) override;
  void SetUniform(const std::string& name, const Mat4& matrix) override;
  void SetUniform(const std::string& name, int value) override;
  void SetUniform(const std::string& name, float value) override;

  // ----- Draw -----
  void DrawIndexed(PrimitiveTopology topology, std::size_t indexCount) override;

  // ----- Viewport -----
  void SetViewport(int x, int y, int width, int height) override;

  // ----- Clear -----
  void SetClearColor(float r, float g, float b, float a) override;
  void Clear() override;

  // ----- Frame control -----
  void BeginFrame() override;
  void EndFrame() override;

  // ----- Window management -----
  GLFWwindow* GetWindow() const { return window_; }
  bool ShouldClose() const;
  void PollEvents();

  // ----- Framebuffer size -----
  int GetFramebufferWidth() const { return fbWidth_; }
  int GetFramebufferHeight() const { return fbHeight_; }

  // ----- Emscripten WebGL initialization -----
  void InitializeWebGL();

 private:
  GLFWwindow* window_ = nullptr;
  int width_ = 800;
  int height_ = 600;
  int fbWidth_ = 800;
  int fbHeight_ = 600;
  GLuint currentShader_ = 0;

  void InitializeOpenGL();
  GLuint CompileShader(GLenum type, const std::string& source);
  GLuint CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource,
                             const std::string& geometrySource);
  GLenum TopologyToGLenum(PrimitiveTopology topology) const;
  std::string GetErrorString(GLenum error) const;
  GLint GetUniformLocation(const std::string& name);
};