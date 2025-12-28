#include <gtest/gtest.h>

#include <span>
#include <vector>

#include "ModelView/ModelViews.h"
#include "Rendering/Devices/RenderDevice.h"
#include "Rendering/FrameContext.h"
#include "Rendering/Passes/FacePass.h"
#include "Rendering/Pipeline.h"
#include "Rendering/Resources/IndexBuffer.h"
#include "Rendering/Resources/ShaderProgram.h"
#include "Rendering/Resources/VertexBuffer.h"
#include "Utilities/Mat4.h"
#include "Utilities/Vec3.h"

// Simple mock RenderDevice that records calls
class MockRenderDevice : public RenderDevice {
 public:
  MockRenderDevice() : nextHandle_(1) {}

  uint32_t CreateVertexBuffer(std::size_t sizeBytes) override {
    uint32_t h = nextHandle_++;
    vertexBuffers_[h] = std::vector<std::byte>(sizeBytes);
    return h;
  }
  uint32_t CreateIndexBuffer(std::size_t sizeBytes) override {
    uint32_t h = nextHandle_++;
    indexBuffers_[h] = std::vector<uint32_t>();
    return h;
  }
  uint32_t CreateShader(const std::string& vertexSource,
                        const std::string& fragmentSource) override {
    (void)vertexSource;
    (void)fragmentSource;
    return nextHandle_++;
  }

  void DestroyBuffer(uint32_t handle) override { (void)handle; }
  void DestroyShader(uint32_t handle) override { (void)handle; }

  void UpdateVertexBuffer(uint32_t handle, std::span<const std::byte> data) override {
    lastVertexBufferUpdateHandle = handle;
    lastVertexBufferData.assign(data.begin(), data.end());
  }
  void UpdateIndexBuffer(uint32_t handle, std::span<const uint32_t> indices) override {
    lastIndexBufferUpdateHandle = handle;
    lastIndexData.assign(indices.begin(), indices.end());
  }

  void BindVertexBuffer(uint32_t handle) override { lastBoundVertexBuffer = handle; }
  void BindIndexBuffer(uint32_t handle) override { lastBoundIndexBuffer = handle; }

  void BindPipeline(const IPipeline& pipeline) override {
    lastPipelineTopology = pipeline.Topology();
    pipelineBound = true;
  }
  void BindShader(uint32_t shaderHandle) override { lastBoundShader = shaderHandle; }

  void SetUniform(const std::string& name, const Mat4& matrix) override {
    lastUniformMatrixName = name;
    lastUniformMatrix = matrix;
  }
  void SetUniform(const std::string& name, int value) override {
    lastUniformIntName = name;
    lastUniformInt = value;
  }

  void DrawIndexed(PrimitiveTopology topology, std::size_t indexCount) override {
    lastDrawTopology = topology;
    lastDrawIndexCount = indexCount;
    drawCalled = true;
  }

  void BeginFrame() override {}
  void EndFrame() override {}

  // Recorded state
  uint32_t nextHandle_;
  std::unordered_map<uint32_t, std::vector<std::byte>> vertexBuffers_;
  std::unordered_map<uint32_t, std::vector<uint32_t>> indexBuffers_;

  uint32_t lastVertexBufferUpdateHandle = 0;
  std::vector<std::byte> lastVertexBufferData;

  uint32_t lastIndexBufferUpdateHandle = 0;
  std::vector<uint32_t> lastIndexData;

  uint32_t lastBoundVertexBuffer = 0;
  uint32_t lastBoundIndexBuffer = 0;

  PrimitiveTopology lastPipelineTopology = PrimitiveTopology::Triangles;
  bool pipelineBound = false;

  uint32_t lastBoundShader = 0;

  std::string lastUniformMatrixName;
  Mat4 lastUniformMatrix;

  std::string lastUniformIntName;
  int lastUniformInt = -1;

  PrimitiveTopology lastDrawTopology = PrimitiveTopology::Triangles;
  std::size_t lastDrawIndexCount = 0;
  bool drawCalled = false;
};

TEST(PipelineFlowTest, FacePassInvokesDeviceDraw) {
  MockRenderDevice device;

  // Create a FacePass and a small triangle
  FacePass pass(device, FacePass::Mode::Faces);

  // Create vertex buffer with 3 vertices
  VertexBuffer vb(device);
  std::vector<Vec3> positions = {Vec3{0, 0, 0}, Vec3{1, 0, 0}, Vec3{0, 1, 0}};
  vb.Upload(positions);

  ModelView faceView;
  faceView.vertexIndices = {0, 1, 2};
  faceView.primitiveCount = 1;

  FrameContext ctx;
  ctx.viewProjection = Mat4::Identity();

  // Draw
  pass.Draw(faceView, vb, ctx);

  // Expectations: DrawIndexed called with 3 indices, topology triangles
  EXPECT_TRUE(device.drawCalled);
  EXPECT_EQ(device.lastDrawTopology, PrimitiveTopology::Triangles);
  EXPECT_EQ(device.lastDrawIndexCount, 3);

  // Shader and pipeline were bound
  EXPECT_TRUE(device.pipelineBound);
  EXPECT_NE(device.lastBoundShader, 0u);

  // Uniforms set
  EXPECT_EQ(device.lastUniformIntName, "uMode");
  EXPECT_EQ(device.lastUniformInt, static_cast<int>(FacePass::Mode::Faces));
  EXPECT_EQ(device.lastUniformMatrixName, "uViewProj");
}
