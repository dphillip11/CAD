// test_command_stack.cpp

#include <gtest/gtest.h>

#include "Model/Commands/CommandStack.h"
#include "Model/Commands/ICommand.h"
#include "Model/Model.h"

class IncrementCommand final : public ICommand {
public:
    IncrementCommand(int& count):count_(count){};
    
    void Execute(Model& model) override {
        count_++;
    }

    void Undo(Model& model) override {
        count_--;
    }
private:
    int& count_;
};

class DecrementCommand final : public ICommand {
public:
    DecrementCommand(int& count):count_(count){};

    void Execute(Model& model) override {
        count_ --;
    }

    void Undo(Model& model) override {
        count_ ++;
    }

private:
    int& count_;
};

class CommandStackTest : public ::testing::Test {
protected:
    Model model;
    CommandStack stack{model};
    int count = 0;
};

TEST_F(CommandStackTest, ExecuteCommand)
{
    EXPECT_EQ(count, 0);

    stack.Do<IncrementCommand>(count);

    EXPECT_EQ(count, 1);
    EXPECT_TRUE(stack.CanUndo());
    EXPECT_FALSE(stack.CanRedo());
}

TEST_F(CommandStackTest, UndoCommand)
{
    stack.Do<IncrementCommand>(count);
    stack.Do<IncrementCommand>(count);

    EXPECT_EQ(count, 2);

    stack.Undo();

    EXPECT_EQ(count, 1);
    EXPECT_TRUE(stack.CanRedo());
}

TEST_F(CommandStackTest, RedoCommand)
{
    stack.Do<IncrementCommand>(count);
    stack.Undo();

    EXPECT_EQ(count, 0);

    stack.Redo();

    EXPECT_EQ(count, 1);
}

TEST_F(CommandStackTest, RedoInvalidatedAfterNewCommand)
{
    stack.Do<IncrementCommand>(count);
    stack.Undo();

    EXPECT_TRUE(stack.CanRedo());

    stack.Do<IncrementCommand>(count);

    EXPECT_FALSE(stack.CanRedo());
    EXPECT_EQ(count, 1);
}

TEST_F(CommandStackTest, MultipleUndoRedo)
{
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






