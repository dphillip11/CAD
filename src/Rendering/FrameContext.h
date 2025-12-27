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

  // ---- Interaction -------------------------------------------
  bool pickingEnabled = false;

  // Mouse position in framebuffer space (for picking)
  uint32_t mouseX = 0;
  uint32_t mouseY = 0;
};
