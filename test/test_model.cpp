#include <gtest/gtest.h>

#include "Model/Model.h"
#include "Utilities/Vec3.h"

class ModelTest : public ::testing::Test {
 protected:
  Model model;
};

TEST_F(ModelTest, CreateVertex_InsertsVertex) {
  Vec3 p{1.0f, 2.0f, 3.0f};
  VertexId id = model.CreateVertex(p);

  ASSERT_TRUE(model.ContainsVertex(id));
  EXPECT_TRUE(IsEqual(model.GetVertex(id).position, p));
}

TEST_F(ModelTest, RemoveVertex_RemovesVertex) {
  VertexId id = model.CreateVertex({0, 0, 0});

  ASSERT_TRUE(model.RemoveVertex(id));
  EXPECT_FALSE(model.ContainsVertex(id));
}

TEST_F(ModelTest, CreateEdge_ValidVertices) {
  VertexId a = model.CreateVertex({0, 0, 0});
  VertexId b = model.CreateVertex({1, 0, 0});

  auto edgeId = model.CreateEdge(a, b);

  ASSERT_TRUE(edgeId.has_value());
  EXPECT_TRUE(model.ContainsEdge(*edgeId));

  const Edge& e = model.GetEdge(*edgeId);
  EXPECT_EQ(e.a, a);
  EXPECT_EQ(e.b, b);
}

TEST_F(ModelTest, CreateEdge_FailsForSameVertex) {
  VertexId a = model.CreateVertex({0, 0, 0});

  auto edgeId = model.CreateEdge(a, a);
  EXPECT_FALSE(edgeId.has_value());
}

TEST_F(ModelTest, CreateEdge_FailsForDuplicateEdge) {
  VertexId a = model.CreateVertex({0, 0, 0});
  VertexId b = model.CreateVertex({1, 0, 0});

  ASSERT_TRUE(model.CreateEdge(a, b).has_value());
  EXPECT_FALSE(model.CreateEdge(a, b).has_value());
}

TEST_F(ModelTest, CreateFace_Triangle) {
  VertexId v0 = model.CreateVertex({0, 0, 0});
  VertexId v1 = model.CreateVertex({1, 0, 0});
  VertexId v2 = model.CreateVertex({0, 1, 0});

  auto e0 = model.CreateEdge(v0, v1);
  auto e1 = model.CreateEdge(v1, v2);
  auto e2 = model.CreateEdge(v2, v0);

  ASSERT_TRUE(e0 && e1 && e2);

  std::vector<EdgeId> edges{*e0, *e1, *e2};
  auto faceId = model.CreateFace(edges);

  ASSERT_TRUE(faceId.has_value());
  EXPECT_TRUE(model.ContainsFace(*faceId));

  const Face& f = model.GetFace(*faceId);
  EXPECT_EQ(f.edges.size(), 3u);
}

TEST_F(ModelTest, CreateFace_FailsForNonPlane) {
  VertexId v0 = model.CreateVertex({0, 0, 0});
  VertexId v1 = model.CreateVertex({1, 0, 0});
  VertexId v2 = model.CreateVertex({0, 1, 0});
  VertexId v3 = model.CreateVertex({0, 1, 1});

  auto e0 = model.CreateEdge(v0, v1);
  auto e1 = model.CreateEdge(v1, v2);
  auto e2 = model.CreateEdge(v2, v3);
  auto e3 = model.CreateEdge(v3, v0);

  ASSERT_TRUE(e0 && e1 && e2 && e3);

  std::vector<EdgeId> edges{*e0, *e1, *e2, *e3};
  EXPECT_FALSE(model.CreateFace(edges));
}

TEST_F(ModelTest, CreateFace_FailsForNonLoop) {
  VertexId v0 = model.CreateVertex({0, 0, 0});
  VertexId v1 = model.CreateVertex({1, 0, 0});
  VertexId v2 = model.CreateVertex({0, 1, 0});

  auto e0 = model.CreateEdge(v0, v1);
  auto e1 = model.CreateEdge(v1, v2);

  ASSERT_TRUE(e0 && e1);

  std::vector<EdgeId> edges{*e0, *e1};  // only 2 edges
  EXPECT_FALSE(model.CreateFace(edges).has_value());
}

TEST_F(ModelTest, CreateVolume_Tetrahedron) {
  // Vertices
  VertexId v0 = model.CreateVertex({0, 0, 0});
  VertexId v1 = model.CreateVertex({1, 0, 0});
  VertexId v2 = model.CreateVertex({0, 1, 0});
  VertexId v3 = model.CreateVertex({0, 0, 1});

  // Edges
  auto e01 = model.CreateEdge(v0, v1);
  auto e02 = model.CreateEdge(v0, v2);
  auto e03 = model.CreateEdge(v0, v3);
  auto e12 = model.CreateEdge(v1, v2);
  auto e13 = model.CreateEdge(v1, v3);
  auto e23 = model.CreateEdge(v2, v3);

  ASSERT_TRUE(e01 && e02 && e03 && e12 && e13 && e23);

  // Faces
  std::array<EdgeId, 3> edges0{*e01, *e12, *e02};
  auto f0 = model.CreateFace(edges0);
  std::array<EdgeId, 3> edges1{*e01, *e13, *e03};
  auto f1 = model.CreateFace(edges1);
  std::array<EdgeId, 3> edges2{*e02, *e23, *e03};
  auto f2 = model.CreateFace(edges2);
  std::array<EdgeId, 3> edges3{*e12, *e23, *e13};
  auto f3 = model.CreateFace(edges3);

  ASSERT_TRUE(f0 && f1 && f2 && f3);

  std::array<FaceId, 4> faces{*f0, *f1, *f2, *f3};
  auto volumeId = model.CreateVolume(faces);
  ASSERT_TRUE(volumeId.has_value());
  EXPECT_TRUE(model.ContainsVolume(*volumeId));
}

TEST_F(ModelTest, IterationReturnsCorrectCounts) {
  model.CreateVertex({0, 0, 0});
  model.CreateVertex({1, 0, 0});

  EXPECT_EQ(model.Vertices().size(), 2u);
  EXPECT_EQ(model.Edges().size(), 0u);
  EXPECT_EQ(model.Faces().size(), 0u);
  EXPECT_EQ(model.Volumes().size(), 0u);
}
