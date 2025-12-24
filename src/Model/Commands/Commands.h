#pragma once

#include <optional>
#include <span>
#include <vector>

#include "ICommand.h"
#include "Utilities/Vec3.h"
#include "Core/Primitives.h"

// =================================================
// Vertex Commands
// =================================================

class CreateVertexCommand final : public ICommand {
public:
    explicit CreateVertexCommand(const Vec3& position);

    void Execute(Model& model) override;
    void Undo(Model& model) override;

private:
    Vec3 position_;
    std::optional<VertexId> createdVertex_;
};

class RemoveVertexCommand final : public ICommand {
public:
    explicit RemoveVertexCommand(VertexId id);

    void Execute(Model& model) override;
    void Undo(Model& model) override;

private:
    VertexId id_;

    // For undo (captured on execute)
    std::optional<Vertex> removedVertex_;
};


// =================================================
// Edge Commands
// =================================================

class CreateEdgeCommand final : public ICommand {
public:
    CreateEdgeCommand(VertexId a, VertexId b);

    void Execute(Model& model) override;
    void Undo(Model& model) override;

private:
    VertexId a_, b_;
    std::optional<EdgeId> createdEdge_;
};

class RemoveEdgeCommand final : public ICommand {
public:
    explicit RemoveEdgeCommand(EdgeId id);

    void Execute(Model& model) override;
    void Undo(Model& model) override;

private:
    EdgeId id_;
    std::optional<Edge> removedEdge_;
};


// =================================================
// Face Commands
// =================================================

class CreateFaceCommand final : public ICommand {
public:
    explicit CreateFaceCommand(std::vector<EdgeId> edges);

    void Execute(Model& model) override;
    void Undo(Model& model) override;

private:
    std::vector<EdgeId> edges_;
    std::optional<FaceId> createdFace_;
};

class RemoveFaceCommand final : public ICommand {
public:
    explicit RemoveFaceCommand(FaceId id);

    void Execute(Model& model) override;
    void Undo(Model& model) override;

private:
    FaceId id_;
    std::optional<Face> removedFace_;
};


// =================================================
// Volume Commands
// =================================================

class CreateVolumeCommand final : public ICommand {
public:
    explicit CreateVolumeCommand(std::vector<FaceId> faces);

    void Execute(Model& model) override;
    void Undo(Model& model) override;

private:
    std::vector<FaceId> faces_;
    std::optional<VolumeId> createdVolume_;
};

class RemoveVolumeCommand final : public ICommand {
public:
    explicit RemoveVolumeCommand(VolumeId id);

    void Execute(Model& model) override;
    void Undo(Model& model) override;

private:
    VolumeId id_;
    std::optional<Volume> removedVolume_;
};
