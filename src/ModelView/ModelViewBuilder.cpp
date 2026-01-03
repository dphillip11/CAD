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

  uint32_t faceIndex = 0;
  for (const auto& f : faces) {
    auto verts = Topology::ExtractVertices(f, edges);
    if (verts.size() < 3) {
      ++faceIndex;
      continue;
    }

    // fan triangulation
    for (size_t i = 1; i + 1 < verts.size(); ++i) {
      outFaces.vertexIndices.push_back(model_.VertexIdToIndex(verts[0]));
      outFaces.vertexIndices.push_back(model_.VertexIdToIndex(verts[i]));
      outFaces.vertexIndices.push_back(model_.VertexIdToIndex(verts[i + 1]));

      outFaces.primitiveIds.push_back(faceIndex);
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

  uint32_t volumeIndex = 0;
  for (const auto& vol : volumes) {
    for (FaceId fid : vol.faces) {
      const Face& f = faces[fid];
      auto verts = Topology::ExtractVertices(f, edges);
      if (verts.size() < 3) continue;

      for (size_t i = 1; i + 1 < verts.size(); ++i) {
        outVolumes.vertexIndices.push_back(model_.VertexIdToIndex(verts[0]));
        outVolumes.vertexIndices.push_back(model_.VertexIdToIndex(verts[i]));
        outVolumes.vertexIndices.push_back(model_.VertexIdToIndex(verts[i + 1]));

        outVolumes.primitiveIds.push_back(volumeIndex);
      }
    }

    ++volumeIndex;
  }

  outVolumes.primitiveCount = volumeIndex;
}
