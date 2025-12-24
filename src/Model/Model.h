#pragma once

#include <optional>
#include <span>

#include "Utilities/SparseSet.h"
#include "Core/Primitives.h"

class Model {
public:
    // ---- Vertex -------------------------------------------------
    VertexId CreateVertex(const Vec3& position);
    bool     RemoveVertex(VertexId id);

    const Vertex& GetVertex(VertexId id) const;

    // ---- Edge ---------------------------------------------------
    std::optional<EdgeId>
    CreateEdge(VertexId a, VertexId b);

    bool RemoveEdge(EdgeId id);

    const Edge& GetEdge(EdgeId id) const;

    // ---- Face ---------------------------------------------------
    std::optional<FaceId>
    CreateFace(std::span<const EdgeId> edges);

    bool RemoveFace(FaceId id);

    const Face& GetFace(FaceId id) const;

    // ---- Volume -------------------------------------------------
    std::optional<VolumeId>
    CreateVolume(std::span<const FaceId> faces);

    bool RemoveVolume(VolumeId id);

    const Volume& GetVolume(VolumeId id) const;

    // ---- Queries -----------------------------------------------
    bool ContainsVertex(VertexId id) const;
    bool ContainsEdge(EdgeId id) const;
    bool ContainsFace(FaceId id) const;
    bool ContainsVolume(VolumeId id) const;

    // ---- Iteration (read-only views) ---------------------------
    std::span<const Vertex> Vertices() const;
    std::span<const Edge>   Edges() const;
    std::span<const Face>   Faces() const;
    std::span<const Volume> Volumes() const;

private:
    SparseSet<Vertex> vertices_;
    SparseSet<Edge>   edges_;
    SparseSet<Face>   faces_;
    SparseSet<Volume> volumes_;

    // Centralized validation hooks
    bool CanCreateEdge(VertexId a, VertexId b) const;
    bool CanCreateFace(std::span<const EdgeId> edges) const;
    bool CanCreateVolume(std::span<const FaceId> faces) const;
};