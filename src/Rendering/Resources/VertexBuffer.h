#pragma once

#include <cstddef>
#include <span>
#include <string>

#include "Rendering/Devices/GpuHandle.h"
#include "Utilities/Vec3.h"

class RenderDevice;

struct VertexAttribute {
  std::string name;
  uint32_t location;
  uint32_t size;
  uint32_t offset;
};

class VertexBuffer {
 public:
  VertexBuffer(RenderDevice& device, const std::span<VertexAttribute>& vertexLayout);
  ~VertexBuffer();

  // Non-copyable
  VertexBuffer(const VertexBuffer&) = delete;
  VertexBuffer& operator=(const VertexBuffer&) = delete;

  // Upload full vertex buffer
  void Upload(std::span<const Vec3> positions);
  void Upload(std::span<const float> data, std::size_t vertexSize);

  // Bind for drawing
  void Bind() const;

  std::size_t VertexCount() const noexcept;

  bool IsDirty() const noexcept;

  // Mark buffer as needing re-upload
  void MarkDirty() { dirty_ = true; }

  uint32_t GetHandle() const noexcept { return handle_; }

 private:
  RenderDevice& device_;

  // Backend-specific handle hidden in implementation
  GpuHandle handle_ = 0;
  std::size_t vertexCount_ = 0;
  bool dirty_ = true;
};
