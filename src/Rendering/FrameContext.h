#pragma once

#include <cstdint>

#include "Utilities/Mat4.h"

struct FrameContext {
  // ---- Camera ------------------------------------------------
  Mat4 view;
  Mat4 projection;

  // Precomputed for convenience
  Mat4 viewProjection;

  // ---- Viewport ----------------------------------------------
  uint32_t viewportWidth = 0;
  uint32_t viewportHeight = 0;

  bool debug = false;
};
