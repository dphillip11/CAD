#pragma once

#include <cstddef>
#include <vector>

#include "Commands.h"

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

  std::vector<Command> undoStack_;
  std::vector<Command> redoStack_;
};

template <typename CommandT, typename... Args>
bool CommandStack::Do(Args&&... args) {
  // Construct command directly in the variant
  Command cmd = CommandT{std::forward<Args>(args)...};

  // Execute
  std::visit(ExecuteVisitor{model_}, cmd);

  // Record
  redoStack_.clear();                    // Invalidate redo history
  undoStack_.push_back(std::move(cmd));  // Record command

  return true;
}
