#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

class RenderDevice;

class IndexBuffer {
 public:
  explicit IndexBuffer(RenderDevice& device);
  ~IndexBuffer();

  // Non-copyable
  IndexBuffer(const IndexBuffer&) = delete;
  IndexBuffer& operator=(const IndexBuffer&) = delete;

  // Upload indices
  void Upload(std::span<const uint32_t> indices);

  // Bind for drawing
  void Bind() const;

  std::size_t IndexCount() const noexcept;

  uint32_t GetHandle() const noexcept { return handle_; }

 private:
  RenderDevice& device_;

  // Backend-specific handle
  uint32_t handle_ = 0;
  std::size_t indexCount_ = 0;
};
