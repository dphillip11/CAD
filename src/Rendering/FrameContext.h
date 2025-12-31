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

  // ---- Input State -------------------------------------------
  struct InputState {
    // Mouse state
    float mouseX = 0.0f;       // Current mouse X position
    float mouseY = 0.0f;       // Current mouse Y position
    float mouseDeltaX = 0.0f;  // Mouse movement delta X
    float mouseDeltaY = 0.0f;  // Mouse movement delta Y
    float scrollDelta = 0.0f;  // Mouse wheel delta

    // Button states
    bool mouseLeftDown = false;    // Left mouse button currently down
    bool mouseMiddleDown = false;  // Middle mouse button currently down
    bool mouseRightDown = false;   // Right mouse button currently down

    // Keyboard modifiers
    bool shiftDown = false;
    bool ctrlDown = false;
    bool altDown = false;
  } input;
};
