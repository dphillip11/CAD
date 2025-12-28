#include <cstddef>

struct alignas(16) UniformBuffer {
  float lineColor[4] = {0.f, 1.f, 0.f, 1.f};
  float faceColor[4] = {1.f, 0.f, 1.f, 1.f};
};
