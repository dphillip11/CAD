#include "VertexBuffer.h"

#include <cstring>

#include "Rendering/Devices/RenderDevice.h"

VertexBuffer::VertexBuffer(RenderDevice& device, const std::span<VertexAttribute>& vertexLayout)
    : device_(device) {
  handle_ = device_.CreateVertexBuffer(0);
  device.SetVertexAttributes(handle_, vertexLayout);
}

VertexBuffer::~VertexBuffer() { device_.DestroyBuffer(handle_); }

void VertexBuffer::Upload(std::span<const Vec3> positions) {
  vertexCount_ = positions.size();
  std::size_t sizeBytes = positions.size() * sizeof(Vec3);

  // Reinterpret Vec3 span as bytes
  auto data =
      std::span<const std::byte>(reinterpret_cast<const std::byte*>(positions.data()), sizeBytes);

  device_.UpdateVertexBuffer(handle_, data);
  dirty_ = false;
}

void VertexBuffer::Upload(std::span<const float> data, std::size_t vertexSize) {
  vertexCount_ = data.size() / vertexSize;
  std::size_t sizeBytes = data.size() * sizeof(float);

  auto byteData =
      std::span<const std::byte>(reinterpret_cast<const std::byte*>(data.data()), sizeBytes);

  device_.UpdateVertexBuffer(handle_, byteData);
  dirty_ = false;
}

void VertexBuffer::Bind() const { device_.BindVertexBuffer(handle_); }

std::size_t VertexBuffer::VertexCount() const noexcept { return vertexCount_; }

bool VertexBuffer::IsDirty() const noexcept { return dirty_; }