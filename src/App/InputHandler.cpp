#include "InputHandler.h"

#include "Core/Primitives.h"
#include "Input.h"

void InputHandler::HandleInput(Input& input) {
  if ((input.IsDown(KEYS::UNDO) || input.IsCtrlDown(KEYS::Z)) && commandStack_.CanUndo()) {
    commandStack_.Undo();
  }

  if ((input.IsDown(KEYS::REDO) || input.IsCtrlShiftDown(KEYS::Z) || input.IsCtrlDown(KEYS::Y)) &&
      commandStack_.CanRedo()) {
    commandStack_.Redo();
  }

  // Orbit with left mouse button
  if (input.IsDown(KEYS::MOUSE_LEFT) &&
      (input.GetMouseDeltaX() != 0.0f || input.GetMouseDeltaY() != 0.0f)) {
    camera_.Orbit(input.GetMouseDeltaX(), input.GetMouseDeltaY());
  }

  // Pan with middle mouse button
  if (input.IsDown(KEYS::MOUSE_MIDDLE) &&
      (input.GetMouseDeltaX() != 0.0f || input.GetMouseDeltaY() != 0.0f)) {
    camera_.Pan(input.GetMouseDeltaX(), input.GetMouseDeltaY());
  }

  // Zoom with scroll wheel (if no face selected)
  if (input.GetScrollDelta() != 0.0f && input.GetSelectedFaceId() == 0) {
    camera_.Zoom(input.GetScrollDelta());
  }
  // Extrude selected face with scroll wheel
  else if (input.GetScrollDelta() != 0.0f && input.GetSelectedFaceId() != 0) {
    // Use command stack for extrude
    FaceId faceId = static_cast<FaceId>(input.GetSelectedFaceId() - 1);
    commandStack_.Do<ExtrudeFaceCommand>(faceId, input.GetScrollDelta());
  }

  // Handle picking on right mouse click
  if (input.IsPressed(KEYS::MOUSE_RIGHT)) {
    input.SetPendingPick(input.GetMouseX(), input.GetMouseY());
  }
}