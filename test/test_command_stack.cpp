// test_command_stack.cpp

#include <gtest/gtest.h>

#include "App/Commands/CommandStack.h"
#include "App/Commands/Commands.h"
#include "Model/Model.h"

// Test command structs (not in Commands.h)
struct IncrementCommand {
  int& count;

  void Execute(Model& model) { count++; }
  void Undo(Model& model) { count--; }
};

struct DecrementCommand {
  int& count;

  void Execute(Model& model) { count--; }
  void Undo(Model& model) { count++; }
};

// Extend Command variant for test commands
using TestCommand =
    std::variant<IncrementCommand, DecrementCommand, CreateVertexCommand, RemoveVertexCommand,
                 CreateEdgeCommand, RemoveEdgeCommand, CreateFaceCommand, RemoveFaceCommand,
                 CreateVolumeCommand, RemoveVolumeCommand>;

// Test-specific CommandStack that accepts test commands
class TestCommandStack {
 public:
  explicit TestCommandStack(Model& model) : model_(model) {}

  template <typename CommandT, typename... Args>
  bool Do(Args&&... args) {
    TestCommand cmd = CommandT{std::forward<Args>(args)...};
    std::visit(ExecuteVisitor{model_}, cmd);
    redoStack_.clear();
    undoStack_.push_back(std::move(cmd));
    return true;
  }

  bool CanUndo() const noexcept { return !undoStack_.empty(); }
  bool CanRedo() const noexcept { return !redoStack_.empty(); }

  bool Undo() {
    if (undoStack_.empty()) return false;
    TestCommand cmd = std::move(undoStack_.back());
    undoStack_.pop_back();
    std::visit(UndoVisitor{model_}, cmd);
    redoStack_.push_back(std::move(cmd));
    return true;
  }

  bool Redo() {
    if (redoStack_.empty()) return false;
    TestCommand cmd = std::move(redoStack_.back());
    redoStack_.pop_back();
    std::visit(ExecuteVisitor{model_}, cmd);
    undoStack_.push_back(std::move(cmd));
    return true;
  }

 private:
  Model& model_;
  std::vector<TestCommand> undoStack_;
  std::vector<TestCommand> redoStack_;
};

class CommandStackTest : public ::testing::Test {
 protected:
  Model model;
  TestCommandStack stack{model};
  int count = 0;
};

TEST_F(CommandStackTest, ExecuteCommand) {
  EXPECT_EQ(count, 0);

  stack.Do<IncrementCommand>(count);

  EXPECT_EQ(count, 1);
  EXPECT_TRUE(stack.CanUndo());
  EXPECT_FALSE(stack.CanRedo());
}

TEST_F(CommandStackTest, UndoCommand) {
  stack.Do<IncrementCommand>(count);
  stack.Do<IncrementCommand>(count);

  EXPECT_EQ(count, 2);

  stack.Undo();

  EXPECT_EQ(count, 1);
  EXPECT_TRUE(stack.CanRedo());
}

TEST_F(CommandStackTest, RedoCommand) {
  stack.Do<IncrementCommand>(count);
  stack.Undo();

  EXPECT_EQ(count, 0);

  stack.Redo();

  EXPECT_EQ(count, 1);
}

TEST_F(CommandStackTest, RedoInvalidatedAfterNewCommand) {
  stack.Do<IncrementCommand>(count);
  stack.Undo();

  EXPECT_TRUE(stack.CanRedo());

  stack.Do<IncrementCommand>(count);

  EXPECT_FALSE(stack.CanRedo());
  EXPECT_EQ(count, 1);
}

TEST_F(CommandStackTest, MultipleUndoRedo) {
  stack.Do<IncrementCommand>(count);
  stack.Do<IncrementCommand>(count);
  stack.Do<IncrementCommand>(count);

  EXPECT_EQ(count, 3);

  stack.Undo();
  stack.Undo();

  EXPECT_EQ(count, 1);

  stack.Redo();

  EXPECT_EQ(count, 2);
}
