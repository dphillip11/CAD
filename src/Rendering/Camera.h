#pragma once

#include "Utilities/Mat4.h"
#include "Utilities/Vec3.h"

class Camera {
 public:
  Camera();

  // Camera setup
  void SetPosition(const Vec3& position);
  void SetTarget(const Vec3& target);
  void SetUp(const Vec3& up);
  void SetFieldOfView(float fovDegrees);
  void SetAspectRatio(float aspect);
  void SetNearFar(float near, float far);

  // Camera controls
  void Orbit(float deltaX, float deltaY);  // Mouse drag in screen space
  void Zoom(float delta);                  // Mouse wheel
  void Pan(float deltaX, float deltaY);    // Mouse drag with middle button

  // Get matrices
  Mat4 GetViewMatrix() const;
  Mat4 GetProjectionMatrix() const;
  Mat4 GetViewProjectionMatrix() const;

  // Get camera properties
  Vec3 GetPosition() const { return position_; }
  Vec3 GetTarget() const { return target_; }
  Vec3 GetUp() const { return up_; }
  float GetFieldOfView() const { return fov_; }
  float GetAspectRatio() const { return aspect_; }
  float GetNear() const { return near_; }
  float GetFar() const { return far_; }

  // Dirty flag management
  bool IsDirty() const { return dirty_; }
  void ClearDirty() { dirty_ = false; }

 private:
  void UpdateViewMatrix() const;
  void UpdateProjectionMatrix() const;

  Vec3 position_;
  Vec3 target_;
  Vec3 up_;
  float fov_;  // in degrees
  float aspect_;
  float near_;
  float far_;

  mutable Mat4 viewMatrix_;
  mutable Mat4 projectionMatrix_;
  mutable bool viewDirty_;
  mutable bool projectionDirty_;
  mutable bool dirty_;  // Public dirty flag for renderer
};