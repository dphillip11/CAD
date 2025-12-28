#include "App/Application.h"

#include <array>
#include <chrono>

#include "Utilities/Mat4.h"
#include "Utilities/Vec3.h"

Application::Application() : device(), renderer(std::make_unique<Renderer>(device)) {}

Application::~Application() = default;

bool Application::Start() {
  // Create a simple cube - make it bigger
  auto v0 = model.CreateVertex(Vec3{-.5, -.5, -.5});
  auto v1 = model.CreateVertex(Vec3{.5, -.5, -.5});
  auto v2 = model.CreateVertex(Vec3{.5, .5, -.5});
  auto v3 = model.CreateVertex(Vec3{-.5, .5, -.5});
  auto v4 = model.CreateVertex(Vec3{-.75, -.5, .5});
  auto v5 = model.CreateVertex(Vec3{.5, -.5, .5});
  auto v6 = model.CreateVertex(Vec3{.5, .75, .5});
  auto v7 = model.CreateVertex(Vec3{-.75, .75, .5});

  // Create edges for the cube
  auto e0 = *model.CreateEdge(v0, v1);
  auto e1 = *model.CreateEdge(v1, v2);
  auto e2 = *model.CreateEdge(v2, v3);
  auto e3 = *model.CreateEdge(v3, v0);
  auto e4 = *model.CreateEdge(v4, v5);
  auto e5 = *model.CreateEdge(v5, v6);
  auto e6 = *model.CreateEdge(v6, v7);
  auto e7 = *model.CreateEdge(v7, v4);
  auto e8 = *model.CreateEdge(v0, v4);
  auto e9 = *model.CreateEdge(v1, v5);
  auto e10 = *model.CreateEdge(v2, v6);
  auto e11 = *model.CreateEdge(v3, v7);

  // Create faces
  std::array<EdgeId, 4> bottomEdges = {e0, e1, e2, e3};
  model.CreateFace(bottomEdges);

  std::array<EdgeId, 4> topEdges = {e4, e5, e6, e7};
  model.CreateFace(topEdges);

  std::array<EdgeId, 4> frontEdges = {e0, e9, e4, e8};
  model.CreateFace(frontEdges);

  std::array<EdgeId, 4> rightEdges = {e1, e10, e5, e9};
  model.CreateFace(rightEdges);

  std::array<EdgeId, 4> backEdges = {e2, e11, e6, e10};
  model.CreateFace(backEdges);

  std::array<EdgeId, 4> leftEdges = {e3, e8, e7, e11};
  model.CreateFace(leftEdges);

  // Build views
  viewBuilder.BuildFaceView(views.faces);
  viewBuilder.BuildLineView(views.lines);

  renderer->SetVerticesDirty();
  renderer->SetIndicesDirty();

  ctx.viewportWidth = 800;
  ctx.viewportHeight = 600;
  return true;
}

int count = 1;
bool Application::Run() {
  device.PollEvents();
  if (device.ShouldClose()) {
    return false;
  }

  if (count > 0) {
    renderer->Render(views, model, ctx);
    count--;
  }

  return true;
}

bool Application::Exit() { return true; }