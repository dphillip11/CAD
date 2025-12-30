#include "CommandStack.h"

#include "Commands.h"
#include "Model/Model.h"

// -------------------------------------------------
// Constructor
// -------------------------------------------------

CommandStack::CommandStack(Model& model) : model_(model) {}

// -------------------------------------------------
// Undo / Redo state
// -------------------------------------------------

bool CommandStack::CanUndo() const noexcept { return !undoStack_.empty(); }

bool CommandStack::CanRedo() const noexcept { return !redoStack_.empty(); }

std::size_t CommandStack::UndoCount() const noexcept { return undoStack_.size(); }

std::size_t CommandStack::RedoCount() const noexcept { return redoStack_.size(); }

// -------------------------------------------------
// Undo / Redo operations
// -------------------------------------------------

bool CommandStack::Undo() {
  if (undoStack_.empty()) return false;

  Command cmd = std::move(undoStack_.back());
  undoStack_.pop_back();

  std::visit(UndoVisitor{model_}, cmd);
  redoStack_.push_back(std::move(cmd));
  return true;
}

bool CommandStack::Redo() {
  if (redoStack_.empty()) return false;

  Command cmd = std::move(redoStack_.back());
  redoStack_.pop_back();

  std::visit(ExecuteVisitor{model_}, cmd);
  undoStack_.push_back(std::move(cmd));
  return true;
}

// -------------------------------------------------
// History management
// -------------------------------------------------

void CommandStack::Clear() {
  undoStack_.clear();
  redoStack_.clear();
}
