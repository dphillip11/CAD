#include <cstddef>

struct alignas(16) UniformBuffer {
  float pointColor[4] = {0.f, 1.f, 0.f, 1.f};
  float lineColor[4] = {0.8f, 0.f, 0.5f, 1.f};
  float faceColor[4] = {1.f, 1.f, 0.5f, 1.f};
  float viewPortSize[2] = {800, 600};
  float lineThickness = 5.0;
  float axisLength = 0.15f;
};
