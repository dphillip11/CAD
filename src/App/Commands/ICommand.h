#pragma once

class Model;

class ICommand {
 public:
  virtual ~ICommand() = default;

  virtual void Execute(Model& model) = 0;
  virtual void Undo(Model& model) = 0;
};