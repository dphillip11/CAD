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

  // Get raw data
  const float* Data() const { return data_.data(); }

 private:
  std::array<float, 16> data_;
};