#pragma once

#include <cstddef>
#include <memory>
#include <vector>

#include "ICommand.h"

class Model;

class CommandStack {
 public:
  explicit CommandStack(Model& model);

  // Execute a command and record it if successful
  template <typename CommandT, typename... Args>
  bool Do(Args&&... args);

  // Undo / Redo
  bool CanUndo() const noexcept;
  bool CanRedo() const noexcept;

  bool Undo();
  bool Redo();

  // History management
  void Clear();

  std::size_t UndoCount() const noexcept;
  std::size_t RedoCount() const noexcept;

 private:
  Model& model_;

  std::vector<std::unique_ptr<ICommand>> undoStack_;
  std::vector<std::unique_ptr<ICommand>> redoStack_;

  void PushUndo(std::unique_ptr<ICommand> cmd);
};

template <typename CommandT, typename... Args>
bool CommandStack::Do(Args&&... args) {
  auto cmd = std::make_unique<CommandT>(std::forward<Args>(args)...);

  cmd->Execute(model_);

  redoStack_.clear();        // Invalidate redo history
  PushUndo(std::move(cmd));  // Record command

  return true;
}
