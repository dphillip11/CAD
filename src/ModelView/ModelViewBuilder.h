#pragma once
#include "ModelViews.h"

class Model;

class ModelViewBuilder {
 public:
  explicit ModelViewBuilder(const Model& model) : model_(model) {}

  void BuildLineView(LineView& outLines);
  void BuildFaceView(FaceView& outFaces);
  void BuildVolumeView(FaceView& outVolumes);

 private:
  const Model& model_;
};