#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <unordered_map>
#include <vector>

#include "Rendering/Devices/GpuHandle.h"
#include "Rendering/FrameContext.h"
#include "Rendering/PrimitiveTopology.h"

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <emscripten.h>
#define GLFW_INCLUDE_ES3
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

class VertexBuffer;
class IndexBuffer;
class Mat4;
struct VertexAttribute;
class Vec3;
class Input;

class RenderDevice {
 public:
  RenderDevice(int width = 800, int height = 600);
  ~RenderDevice();

  // ----- Resource creation -----
  GpuHandle CreatePipeline();
  GpuHandle CreateBuffer();
  GpuHandle CreateFrameBuffer(GpuHandle colorHandle, GpuHandle depthHandle,
                              GpuHandle stencilHandle);
  GpuHandle CreateTexture1D(uint32_t width);
  GpuHandle CreateTexture2D(float width, float height, bool generateMipmaps);
  GpuHandle CreateFloatTexture2D(float width, float height);  // RGB32F texture
  GpuHandle CreateDepthTexture2D(float width, float height);
  GpuHandle CreateShader(const std::string& vertexSource, const std::string& fragmentSource,
                         const std::string& geometrySource = "");

  void DestroyBuffer(GpuHandle handle);
  void DestroyShader(GpuHandle handle);
  void DestroyTexture(GpuHandle handle);

  // ----- Buffer updates -----
  void UpdateVertexBuffer(GpuHandle handle, size_t bytes, const void* data);
  void UpdateUniformBuffer(GpuHandle handle, size_t bytes, const void* data, uint32_t position);
  void UpdateIndexBuffer(GpuHandle handle, std::span<const uint32_t> indices);
  void UpdateTexture1D(GpuHandle textureHandle, std::span<const uint32_t> data);

  // ----- Binding -----
  void BindPipeline(GpuHandle handle);
  void BindVertexBuffer(GpuHandle handle);
  void SetVertexAttributes(GpuHandle handle, const std::span<VertexAttribute>& attributes);
  void BindIndexBuffer(GpuHandle handle);
  void BindTexture(GpuHandle handle, uint32_t index);
  void BindFrameBuffer(GpuHandle handle);
  void BindShader(GpuHandle shaderHandle);

  // ----- Picking -----
  void ReadPixel(uint32_t x, uint32_t y, uint8_t* rgba);
  void ReadFloatPixel(uint32_t x, uint32_t y, float* rgba);  // Read RGBA32F pixel
  float ReadDepthPixel(uint32_t x, uint32_t y);

  // ----- Uniforms -----
  void SetUniform(const std::string& name, const Vec3& vec);
  void SetUniform(const std::string& name, float valueA, float valueB);
  void SetUniform(const std::string& name, const Mat4& matrix);
  void SetUniform(const std::string& name, int value);
  void SetUniform(const std::string& name, float value);

  // ----- Draw -----
  void DrawIndexed(PrimitiveTopology topology, std::size_t indexCount);
  void Draw(PrimitiveTopology topology, std::size_t vertexCount);

  // ----- Viewport -----
  void SetViewport(int x, int y, int width, int height);

  // ----- Clear -----
  void SetClearColor(float r, float g, float b, float a);
  void Clear();

  // ----- Blending -----
  void EnableBlending();
  void DisableBlending();

  // ----- Frame control -----
  void BeginFrame();
  void EndFrame();

  // ----- Window management -----
  GLFWwindow* GetWindow() const { return window_; }
  bool ShouldClose() const;
  void PollEvents();

  // ----- Input capture -----
  void CaptureFrameContext(FrameContext& context);
  void CaptureInput(Input& input);

  // ----- Framebuffer dimensions -----
  int GetFramebufferWidth() const { return fbWidth_; }
  int GetFramebufferHeight() const { return fbHeight_; }

 private:
  // Platform-specific initialization
  void InitializePlatform();

  // Input handling
  static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
  static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

  // Common state
  GLFWwindow* window_ = nullptr;
  int width_ = 800;
  int height_ = 600;
  int fbWidth_ = 1920;
  int fbHeight_ = 1080;
  GLuint currentShader_ = 0;

  // Input state
  float scrollAccumulator_ = 0.0f;
  bool framebufferResized_ = false;

  // Utility functions
  GLuint CompileShader(GLenum type, const std::string& source);
  GLuint CreateShaderProgram(const std::string& vertexSource, const std::string& fragmentSource,
                             const std::string& geometrySource);
  GLenum TopologyToGLenum(PrimitiveTopology topology) const;
  std::string GetErrorString(GLenum error) const;
  GLint GetUniformLocation(const std::string& name);
};
