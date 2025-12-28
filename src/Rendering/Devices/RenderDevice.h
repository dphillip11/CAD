#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

#include "Rendering/PrimitiveTopology.h"

class VertexBuffer;
class IndexBuffer;
class Mat4;
struct VertexAttribute;
class Vec3;

#include "Rendering/Devices/GpuHandle.h"

class RenderDevice {
 public:
  virtual ~RenderDevice() = default;

  // ----- Resource creation -----
  virtual GpuHandle CreatePipeline() = 0;
  virtual GpuHandle CreateVertexBuffer(std::size_t sizeBytes) = 0;
  virtual GpuHandle CreateIndexBuffer(std::size_t sizeBytes) = 0;
  virtual GpuHandle CreateFrameBuffer(GpuHandle colorHandle, GpuHandle depthHandle,
                                      GpuHandle stencilHandle) = 0;
  virtual GpuHandle CreateTexture2D(const float width, const float height,
                                    bool generateMipmaps) = 0;
  virtual GpuHandle CreateShader(const std::string& vertexSource, const std::string& fragmentSource,
                                 const std::string& geometrySource = "") = 0;

  virtual void DestroyBuffer(GpuHandle handle) = 0;
  virtual void DestroyShader(GpuHandle handle) = 0;

  // ----- Buffer updates -----
  virtual void UpdateVertexBuffer(GpuHandle handle, std::span<const std::byte> data) = 0;

  virtual void UpdateIndexBuffer(GpuHandle handle, std::span<const uint32_t> indices) = 0;

  // ----- Binding -----
  virtual void BindPipeline(GpuHandle handle) = 0;
  virtual void BindVertexBuffer(GpuHandle handle) = 0;
  virtual void SetVertexAttributes(GpuHandle handle,
                                   const std::span<VertexAttribute>& attributes) = 0;

  virtual void BindIndexBuffer(GpuHandle handle) = 0;
  virtual void BindTexture(GpuHandle handle) = 0;
  virtual void BindFrameBuffer(GpuHandle handle) = 0;
  virtual void BindShader(GpuHandle shaderHandle) = 0;

  // ----- Uniforms -----
  virtual void SetUniform(const std::string& name, const Vec3& vec) = 0;
  virtual void SetUniform(const std::string& name, const float valueA, const float valueB) = 0;
  virtual void SetUniform(const std::string& name, const Mat4& matrix) = 0;
  virtual void SetUniform(const std::string& name, int value) = 0;
  virtual void SetUniform(const std::string& name, float value) = 0;

  // ----- Draw -----
  virtual void DrawIndexed(PrimitiveTopology topology, std::size_t indexCount) = 0;

  // ----- Frame control -----
  virtual void BeginFrame() = 0;
  virtual void EndFrame() = 0;
};
