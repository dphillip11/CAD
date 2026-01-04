#include "Commands/CommandStack.h"
#include "Rendering/Camera.h"

class Input;

class InputHandler {
 public:
  InputHandler(CommandStack& commandStack, Camera& camera)
      : commandStack_(commandStack), camera_(camera) {}

  void HandleInput(Input& input);

 private:
  CommandStack& commandStack_;
  Camera& camera_;
};