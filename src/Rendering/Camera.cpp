#include "Camera.h"

#include <algorithm>
#include <cmath>

Camera::Camera()
    : position_(0, 0, 5),
      target_(0, 0, 0),
      up_(0, 1, 0),
      fov_(45.0f),
      aspect_(16.0f / 9.0f),
      near_(0.1f),
      far_(1000.0f),
      viewDirty_(true),
      projectionDirty_(true),
      dirty_(true) {}

void Camera::SetPosition(const Vec3& position) {
  position_ = position;
  viewDirty_ = true;
  dirty_ = true;
}

void Camera::SetTarget(const Vec3& target) {
  target_ = target;
  viewDirty_ = true;
  dirty_ = true;
}

void Camera::SetUp(const Vec3& up) {
  up_ = up;
  viewDirty_ = true;
  dirty_ = true;
}

void Camera::SetFieldOfView(float fovDegrees) {
  fov_ = fovDegrees;
  projectionDirty_ = true;
  dirty_ = true;
}

void Camera::SetAspectRatio(float aspect) {
  aspect_ = aspect;
  projectionDirty_ = true;
  dirty_ = true;
}

void Camera::SetNearFar(float near, float far) {
  near_ = near;
  far_ = far;
  projectionDirty_ = true;
  dirty_ = true;
}

void Camera::Orbit(float deltaX, float deltaY) {
  // Convert mouse deltas to rotation angles (scale as needed)
  const float sensitivity = 0.005f;
  float deltaYaw = -deltaX * sensitivity;
  float deltaPitch = deltaY * sensitivity;

  // Get current camera direction
  Vec3 direction = (position_ - target_).Normalized();

  // Calculate current spherical coordinates
  float distance = (position_ - target_).Length();
  float pitch = std::asin(direction.y);
  float yaw = std::atan2(direction.x, direction.z);

  // Update angles
  yaw += deltaYaw;
  pitch += deltaPitch;

  // Clamp pitch to avoid gimbal lock
  const float maxPitch = 3.14159f * 0.49f;  // ~89 degrees
  pitch = std::max(-maxPitch, std::min(maxPitch, pitch));

  // Convert back to cartesian coordinates
  position_.x = target_.x + distance * std::cos(pitch) * std::sin(yaw);
  position_.y = target_.y + distance * std::sin(pitch);
  position_.z = target_.z + distance * std::cos(pitch) * std::cos(yaw);

  viewDirty_ = true;
  dirty_ = true;
}

void Camera::Zoom(float delta) {
  const float zoomSpeed = 0.1f;
  Vec3 direction = (target_ - position_).Normalized();
  float distance = (position_ - target_).Length();

  // Zoom in/out along the view direction
  distance *= (1.0f - delta * zoomSpeed);

  // Clamp distance to reasonable bounds
  distance = std::max(0.1f, std::min(1000.0f, distance));

  position_ = target_ - direction * distance;
  viewDirty_ = true;
  dirty_ = true;
}

void Camera::Pan(float deltaX, float deltaY) {
  // Calculate camera's right and up vectors in world space
  Vec3 forward = (target_ - position_).Normalized();
  Vec3 right = forward.Cross(up_).Normalized();
  Vec3 cameraUp = right.Cross(forward).Normalized();

  const float panSpeed = 0.01f;
  Vec3 panVector = right * (-deltaX * panSpeed) + cameraUp * (deltaY * panSpeed);

  // Move both position and target
  position_ = position_ + panVector;
  target_ = target_ + panVector;

  viewDirty_ = true;
  dirty_ = true;
}

Mat4 Camera::GetViewMatrix() const {
  if (viewDirty_) {
    UpdateViewMatrix();
    viewDirty_ = false;
  }
  return viewMatrix_;
}

Mat4 Camera::GetProjectionMatrix() const {
  if (projectionDirty_) {
    UpdateProjectionMatrix();
    projectionDirty_ = false;
  }
  return projectionMatrix_;
}

Mat4 Camera::GetViewProjectionMatrix() const { return GetProjectionMatrix() * GetViewMatrix(); }

void Camera::UpdateViewMatrix() const { viewMatrix_ = Mat4::LookAt(position_, target_, up_); }

void Camera::UpdateProjectionMatrix() const {
  projectionMatrix_ = Mat4::Perspective(fov_ * 3.14159f / 180.0f, aspect_, near_, far_);
}