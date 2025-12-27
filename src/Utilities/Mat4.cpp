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