#pragma once

#include <set>

#include "Core/Primitives.h"

enum class KEYS {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  CTRL,
  SHIFT,
  ALT,
  Z,
  Y,
  SPACE,
  ESCAPE,
  MOUSE_LEFT,
  MOUSE_RIGHT,
  MOUSE_MIDDLE,
  UNDO,
  REDO,
  DEBUG
};

enum class INTERACTION_MODE { DEFAULT, FACE };

struct MouseState {
  float x = 0.0f;
  float y = 0.0f;
  float deltaX = 0.0f;
  float deltaY = 0.0f;
  float scrollDelta = 0.0f;
};

class Input {
 public:
  INTERACTION_MODE GetInteractionMode() const { return interactionMode_; }
  // only true on intital key pressed frame
  bool IsPressed(KEYS key) const { return keysPressed_.contains(key); }
  // only true on frame where key down is released
  bool IsReleased(KEYS key) const { return keysReleased_.contains(key); }
  // true whenever key is dowwn, pressed or held
  bool IsDown(KEYS key) const { return keysDown_.contains(key); }

  // combo, ctrl + key pressed
  bool IsCtrlPressed(KEYS key) const { return IsDown(KEYS::CTRL) && IsPressed(key); }
  // combo, ctrl + shift + key pressed
  bool IsCtrlShiftPressed(KEYS key) const {
    return IsDown(KEYS::CTRL) && IsDown(KEYS::SHIFT) && IsPressed(key);
  }
  // combo, held ctrl + key down
  bool IsCtrlDown(KEYS key) const { return IsDown(KEYS::CTRL) && IsDown(key); }
  // combo, ctrl + shift + key down
  bool IsCtrlShiftDown(KEYS key) const {
    return IsDown(KEYS::CTRL) && IsDown(KEYS::SHIFT) && IsDown(key);
  }

  // ===== Mouse State =====

  const MouseState& GetMouse() const { return mouse_; }
  float GetMouseX() const { return mouse_.x; }
  float GetMouseY() const { return mouse_.y; }
  float GetMouseDeltaX() const { return mouse_.deltaX; }
  float GetMouseDeltaY() const { return mouse_.deltaY; }
  float GetScrollDelta() const { return mouse_.scrollDelta; }

  // ===== Update Functions =====

  // Called each frame with current hardware state
  void Update(const std::set<KEYS>& keysCurrentlyDown, const MouseState& mouseState) {
    UpdateKeys(keysCurrentlyDown);
    UpdateMouse(mouseState);
  }

  void SetPendingPick(int x, int y) {
    pendingPick_[0] = x;
    pendingPick_[1] = y;
  }

  int32_t GetSelectedFaceId() const { return selectedFaceId_; }

  void SetSelectedFaceId(FaceId id) {
    selectedFaceId_ = id;
    pendingPick_[0] = -1;
    pendingPick_[1] = -1;
  }

  // Clear all input state (useful for focus loss, mode changes)
  void Clear() {
    keysPressed_.clear();
    keysDown_.clear();
    keysReleased_.clear();
    mouse_ = MouseState{};
  }

 private:
  void UpdateKeys(const std::set<KEYS>& keysCurrentlyDown) {
    // Clear per-frame states
    keysPressed_.clear();
    keysReleased_.clear();

    // Determine pressed keys
    for (KEYS key : keysCurrentlyDown) {
      if (!keysDown_.contains(key)) {
        keysPressed_.insert(key);
      }
    }

    // Determine released keys
    for (KEYS key : keysDown_) {
      if (!keysCurrentlyDown.contains(key)) {
        keysReleased_.insert(key);
      }
    }

    keysDown_ = keysCurrentlyDown;
  }

  void UpdateMouse(const MouseState& newState) {
    // Calculate deltas
    mouse_.deltaX = newState.x - mouse_.x;
    mouse_.deltaY = newState.y - mouse_.y;

    // Update position
    mouse_.x = newState.x;
    mouse_.y = newState.y;

    // Update scroll (usually accumulated externally)
    mouse_.scrollDelta = newState.scrollDelta;
  }

  std::set<KEYS> keysPressed_;
  std::set<KEYS> keysDown_;
  std::set<KEYS> keysReleased_;
  INTERACTION_MODE interactionMode_;

  int pendingPick_[2] = {-1, -1};
  int32_t selectedFaceId_ = -1;
  int32_t selectedVertexId_ = -1;

  MouseState mouse_;
};
