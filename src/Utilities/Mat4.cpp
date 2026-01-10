#include "Mat4.h"

#include "Vec3.h"

Mat4::Mat4() : data_{0} {}

Mat4::Mat4(const std::array<float, 16>& data) : data_(data) {}

Mat4 Mat4::Identity() { return Mat4({1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}); }

Mat4 Mat4::Perspective(float fovY, float aspect, float near, float far) {
  float tanHalfFovY = std::tan(fovY / 2.0f);
  float range = far - near;

  Mat4 result;
  result.data_[0] = 1.0f / (aspect * tanHalfFovY);
  result.data_[5] = 1.0f / tanHalfFovY;
  result.data_[10] = -(far + near) / range;
  result.data_[11] = -1.0f;
  result.data_[14] = -(2.0f * far * near) / range;
  result.data_[15] = 0.0f;

  return result;
}

Mat4 Mat4::LookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
  Vec3 f = (center - eye).Normalized();
  Vec3 s = f.Cross(up).Normalized();
  Vec3 u = s.Cross(f);

  Mat4 result = Mat4::Identity();
  result.data_[0] = s.x;
  result.data_[1] = u.x;
  result.data_[2] = -f.x;
  result.data_[4] = s.y;
  result.data_[5] = u.y;
  result.data_[6] = -f.y;
  result.data_[8] = s.z;
  result.data_[9] = u.z;
  result.data_[10] = -f.z;
  result.data_[12] = -s.Dot(eye);
  result.data_[13] = -u.Dot(eye);
  result.data_[14] = f.Dot(eye);

  return result;
}

Mat4 Mat4::operator*(const Mat4& other) const {
  Mat4 result;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      result.data_[i * 4 + j] = 0;
      for (int k = 0; k < 4; ++k) {
        result.data_[i * 4 + j] += data_[i * 4 + k] * other.data_[k * 4 + j];
      }
    }
  }
  return result;
}

Mat4 Mat4::Inverse() const {
  // Using cofactor expansion for 4x4 matrix inversion
  const float* m = data_.data();

  float inv[16];
  inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] +
           m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
  inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] -
           m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
  inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] +
           m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
  inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] -
            m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
  inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] -
           m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
  inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] +
           m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
  inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] -
           m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
  inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] +
            m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
  inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] +
           m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
  inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] -
           m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
  inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] +
            m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
  inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] -
            m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
  inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] -
           m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
  inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] +
           m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
  inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] -
            m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
  inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] +
            m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

  float det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

  if (det == 0) {
    return Mat4::Identity();  // Return identity if non-invertible
  }

  det = 1.0f / det;
  Mat4 result;
  for (int i = 0; i < 16; i++) {
    result.data_[i] = inv[i] * det;
  }

  return result;
}

Vec3 Mat4::TransformPoint(const Vec3& point) const {
  // For column-major matrix storage (OpenGL convention):
  // Column 0: [0,1,2,3], Column 1: [4,5,6,7], Column 2: [8,9,10,11], Column 3: [12,13,14,15]
  // Matrix multiplication: result = M * vec, where vec is treated as column vector with w=1
  float x = data_[0] * point.x + data_[4] * point.y + data_[8] * point.z + data_[12] * 1.0f;
  float y = data_[1] * point.x + data_[5] * point.y + data_[9] * point.z + data_[13] * 1.0f;
  float z = data_[2] * point.x + data_[6] * point.y + data_[10] * point.z + data_[14] * 1.0f;
  float w = data_[3] * point.x + data_[7] * point.y + data_[11] * point.z + data_[15] * 1.0f;

  // Perform perspective divide if w != 1
  if (w != 0.0f && w != 1.0f) {
    return Vec3(x / w, y / w, z / w);
  }
  return Vec3(x, y, z);
}