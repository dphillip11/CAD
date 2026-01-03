#include "ModelViewBuilder.h"

#include "Model/Model.h"
#include "ModelViewBuilder.h"
#include "Topology/Tools.h"

void ModelViewBuilder::BuildLineView(LineView& outLines) {
  outLines.Clear();

  const auto& edges = model_.Edges();

  uint32_t edgeIndex = 0;
  for (const auto& e : edges) {
    uint32_t i0 = model_.VertexIdToIndex(e.a);
    uint32_t i1 = model_.VertexIdToIndex(e.b);

    outLines.vertexIndices.push_back(i0);
    outLines.vertexIndices.push_back(i1);

    // same primitive index for both vertices
    outLines.primitiveIds.push_back(edgeIndex);

    ++edgeIndex;
  }

  outLines.primitiveCount = edgeIndex;
}

void ModelViewBuilder::BuildFaceView(FaceView& outFaces) {
  outFaces.Clear();

  const auto& faces = model_.Faces();
  const auto& edges = model_.Edges();
  const auto& vertices = model_.Vertices();

  uint32_t faceIndex = 0;
  for (const auto& f : faces) {
    // Get the actual FaceId for this face
    FaceId faceId = model_.FaceIndexToId(faceIndex);

    auto verts = Topology::ExtractVertices(f, edges);
    if (verts.size() < 3) {
      ++faceIndex;
      continue;
    }

    // fan triangulation - expand vertices (no indexing)
    for (size_t i = 1; i + 1 < verts.size(); ++i) {
      // Add the 3 vertices of the triangle
      uint32_t idx0 = model_.VertexIdToIndex(verts[0]);
      uint32_t idx1 = model_.VertexIdToIndex(verts[i]);
      uint32_t idx2 = model_.VertexIdToIndex(verts[i + 1]);

      outFaces.vertices.push_back(vertices[idx0].position);
      outFaces.vertices.push_back(vertices[idx1].position);
      outFaces.vertices.push_back(vertices[idx2].position);

      // Store the actual Face ID once per vertex (3 times)
      outFaces.primitiveIds.push_back(faceId + 1);
      outFaces.primitiveIds.push_back(faceId + 1);
      outFaces.primitiveIds.push_back(faceId + 1);
    }

    ++faceIndex;
  }

  outFaces.primitiveCount = faceIndex;
}

void ModelViewBuilder::BuildVolumeView(VolumeView& outVolumes) {
  outVolumes.Clear();

  const auto& volumes = model_.Volumes();
  const auto& faces = model_.Faces();
  const auto& edges = model_.Edges();
  const auto& vertices = model_.Vertices();

  uint32_t volumeIndex = 0;
  for (const auto& vol : volumes) {
    for (FaceId fid : vol.faces) {
      const Face& f = faces[fid];
      auto verts = Topology::ExtractVertices(f, edges);
      if (verts.size() < 3) continue;

      for (size_t i = 1; i + 1 < verts.size(); ++i) {
        // Add the 3 vertices of the triangle
        uint32_t idx0 = model_.VertexIdToIndex(verts[0]);
        uint32_t idx1 = model_.VertexIdToIndex(verts[i]);
        uint32_t idx2 = model_.VertexIdToIndex(verts[i + 1]);

        outVolumes.vertices.push_back(vertices[idx0].position);
        outVolumes.vertices.push_back(vertices[idx1].position);
        outVolumes.vertices.push_back(vertices[idx2].position);

        // Store the Face ID once per vertex (3 times) - use the face ID, not volume index
        outVolumes.primitiveIds.push_back(fid);
        outVolumes.primitiveIds.push_back(fid);
        outVolumes.primitiveIds.push_back(fid);
      }
    }

    ++volumeIndex;
  }

  outVolumes.primitiveCount = volumeIndex;
}
