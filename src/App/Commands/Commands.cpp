#include "Commands.h"

#include <set>

#include "Model/Model.h"

// =================================================
// Vertex Commands
// =================================================

void CreateVertexCommand::Execute(Model& model) { createdVertex = model.CreateVertex(position); }

void CreateVertexCommand::Undo(Model& model) {
  if (createdVertex) {
    model.RemoveVertex(*createdVertex);
    createdVertex.reset();
  }
}

void RemoveVertexCommand::Execute(Model& model) {
  if (model.ContainsVertex(id)) {
    removedVertex = model.GetVertex(id);
    model.RemoveVertex(id);
  }
}

void RemoveVertexCommand::Undo(Model& model) {
  if (removedVertex) {
    // Note: This recreates with same ID - may need ID restoration logic
    model.CreateVertex(removedVertex->position);
    removedVertex.reset();
  }
}

// =================================================
// Edge Commands
// =================================================

void CreateEdgeCommand::Execute(Model& model) { createdEdge = model.CreateEdge(a, b); }

void CreateEdgeCommand::Undo(Model& model) {
  if (createdEdge) {
    model.RemoveEdge(*createdEdge);
    createdEdge.reset();
  }
}

void RemoveEdgeCommand::Execute(Model& model) {
  if (model.ContainsEdge(id)) {
    removedEdge = model.GetEdge(id);
    model.RemoveEdge(id);
  }
}

void RemoveEdgeCommand::Undo(Model& model) {
  if (removedEdge) {
    model.CreateEdge(removedEdge->a, removedEdge->b);
    removedEdge.reset();
  }
}

// =================================================
// Face Commands
// =================================================

void CreateFaceCommand::Execute(Model& model) { createdFace = model.CreateFace(edges); }

void CreateFaceCommand::Undo(Model& model) {
  if (createdFace) {
    model.RemoveFace(*createdFace);
    createdFace.reset();
  }
}

void RemoveFaceCommand::Execute(Model& model) {
  if (model.ContainsFace(id)) {
    removedFace = model.GetFace(id);
    model.RemoveFace(id);
  }
}

void RemoveFaceCommand::Undo(Model& model) {
  if (removedFace) {
    model.CreateFace(removedFace->edges);
    removedFace.reset();
  }
}

// =================================================
// Face Modification Commands
// =================================================

void ExtrudeFaceCommand::Execute(Model& model) {
  if (!model.ContainsFace(faceId)) {
    return;
  }

  // Store original positions for undo
  if (affectedVertices.empty()) {
    const Face& face = model.GetFace(faceId);
    std::set<VertexId> uniqueVertices;

    for (EdgeId edgeId : face.edges) {
      const Edge& edge = model.GetEdge(edgeId);
      uniqueVertices.insert(edge.a);
      uniqueVertices.insert(edge.b);
    }

    affectedVertices.assign(uniqueVertices.begin(), uniqueVertices.end());
    originalPositions.reserve(affectedVertices.size());

    for (VertexId vid : affectedVertices) {
      originalPositions.push_back(model.GetVertex(vid).position);
    }
  }

  // Perform the extrusion
  model.ExtrudeFace(faceId, delta);
}

void ExtrudeFaceCommand::Undo(Model& model) {
  // Restore original vertex positions
  for (size_t i = 0; i < affectedVertices.size(); ++i) {
    if (model.ContainsVertex(affectedVertices[i])) {
      model.SetVertexPosition(affectedVertices[i], originalPositions[i]);
    }
  }
}

// =================================================
// Volume Commands
// =================================================

void CreateVolumeCommand::Execute(Model& model) { createdVolume = model.CreateVolume(faces); }

void CreateVolumeCommand::Undo(Model& model) {
  if (createdVolume) {
    model.RemoveVolume(*createdVolume);
    createdVolume.reset();
  }
}

void RemoveVolumeCommand::Execute(Model& model) {
  if (model.ContainsVolume(id)) {
    removedVolume = model.GetVolume(id);
    model.RemoveVolume(id);
  }
}

void RemoveVolumeCommand::Undo(Model& model) {
  if (removedVolume) {
    model.CreateVolume(removedVolume->faces);
    removedVolume.reset();
  }
}
