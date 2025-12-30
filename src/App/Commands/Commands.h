#pragma once

#include <optional>
#include <span>
#include <variant>
#include <vector>

#include "Core/Primitives.h"
#include "Utilities/Vec3.h"

class Model;

// =================================================
// Vertex Commands
// =================================================

struct CreateVertexCommand {
  Vec3 position;
  std::optional<VertexId> createdVertex;

  void Execute(Model& model);
  void Undo(Model& model);
};

struct RemoveVertexCommand {
  VertexId id;
  std::optional<Vertex> removedVertex;

  void Execute(Model& model);
  void Undo(Model& model);
};

// =================================================
// Edge Commands
// =================================================

struct CreateEdgeCommand {
  VertexId a, b;
  std::optional<EdgeId> createdEdge;

  void Execute(Model& model);
  void Undo(Model& model);
};

struct RemoveEdgeCommand {
  EdgeId id;
  std::optional<Edge> removedEdge;

  void Execute(Model& model);
  void Undo(Model& model);
};

// =================================================
// Face Commands
// =================================================

struct CreateFaceCommand {
  std::vector<EdgeId> edges;
  std::optional<FaceId> createdFace;

  void Execute(Model& model);
  void Undo(Model& model);
};

struct RemoveFaceCommand {
  FaceId id;
  std::optional<Face> removedFace;

  void Execute(Model& model);
  void Undo(Model& model);
};

// =================================================
// Volume Commands
// =================================================

struct CreateVolumeCommand {
  std::vector<FaceId> faces;
  std::optional<VolumeId> createdVolume;

  void Execute(Model& model);
  void Undo(Model& model);
};

struct RemoveVolumeCommand {
  VolumeId id;
  std::optional<Volume> removedVolume;

  void Execute(Model& model);
  void Undo(Model& model);
};

// =================================================
// Command Variant
// =================================================

using Command =
    std::variant<CreateVertexCommand, RemoveVertexCommand, CreateEdgeCommand, RemoveEdgeCommand,
                 CreateFaceCommand, RemoveFaceCommand, CreateVolumeCommand, RemoveVolumeCommand>;

// Helper visitors for Execute/Undo
struct ExecuteVisitor {
  Model& model;

  template <typename T>
  void operator()(T& cmd) {
    cmd.Execute(model);
  }
};

struct UndoVisitor {
  Model& model;

  template <typename T>
  void operator()(T& cmd) {
    cmd.Undo(model);
  }
};
