#pragma once

#include <array>
#include <cmath>

class Vec3;

class Mat4 {
 public:
  Mat4();
  explicit Mat4(const std::array<float, 16>& data);

  // Identity matrix
  static Mat4 Identity();

  // Perspective projection matrix
  static Mat4 Perspective(float fovY, float aspect, float near, float far);

  // Look-at matrix
  static Mat4 LookAt(const Vec3& eye, const Vec3& center, const Vec3& up);

  // Matrix multiplication
  Mat4 operator*(const Mat4& other) const;

  // Matrix inversion
  Mat4 Inverse() const;

  // Transform a 3D point (w=1)
  Vec3 TransformPoint(const Vec3& point) const;

  // Access element at row, col (for debugging)
  float At(int row, int col) const { return data_[col * 4 + row]; }

  // Get raw data
  const float* Data() const { return data_.data(); }

 private:
  std::array<float, 16> data_;
};