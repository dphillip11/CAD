#pragma once
#include <cstdint>
#include <string>

struct VertexAttribute {
  std::string name;
  uint32_t location;
  uint32_t size;
  uint32_t offset;
};