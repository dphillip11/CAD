#include "IndexBuffer.h"

#include "Rendering/Devices/RenderDevice.h"

IndexBuffer::IndexBuffer(RenderDevice& device) : device_(device) {
  handle_ = device_.CreateIndexBuffer(0);
}

IndexBuffer::~IndexBuffer() { device_.DestroyBuffer(handle_); }

void IndexBuffer::Upload(std::span<const uint32_t> indices) {
  indexCount_ = indices.size();
  device_.UpdateIndexBuffer(handle_, indices);
}

void IndexBuffer::Bind() const { device_.BindIndexBuffer(handle_); }

std::size_t IndexBuffer::IndexCount() const noexcept { return indexCount_; }