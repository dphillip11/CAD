#pragma once

#include <unordered_map>

#include "Core/Primitives.h"
#include "Topology/Tools.h"

namespace Topology {
// -------------------------------------------------
// Validate face topology
// -------------------------------------------------
template <typename EdgeContainer>
bool IsValidFace(const Face& face, const EdgeContainer& edges) {
  if (face.edges.size() < 3) return false;

  // Count vertex degrees
  std::unordered_map<VertexId, int> degree;

  for (EdgeId eid : face.edges) {
    const Edge& e = edges.Get(eid);
    degree[e.a]++;
    degree[e.b]++;
  }

  // In a closed loop, each vertex has degree 2
  for (const auto& [_, count] : degree) {
    if (count != 2) return false;
  }

  // Ensure edges form a single loop
  auto verts = ExtractVertices(face, edges);
  return verts.size() >= 3;
}

// -------------------------------------------------
// Validate volume topology
// -------------------------------------------------
template <typename FaceContainer>
bool IsValidVolume(const Volume& volume, const FaceContainer& faces) {
  if (volume.faces.size() < 4) return false;

  std::unordered_map<EdgeId, int> edgeUsage;

  for (FaceId fid : volume.faces) {
    const Face& face = faces.Get(fid);
    for (EdgeId eid : face.edges) {
      edgeUsage[eid]++;
    }
  }

  // Each edge in a closed volume must belong to exactly 2 faces
  for (const auto& [_, count] : edgeUsage) {
    if (count != 2) return false;
  }

  return true;
}
}  // namespace Topology