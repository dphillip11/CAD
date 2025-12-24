#pragma once

#include <cmath>

struct Vec3 {
  constexpr explicit Vec3(float a) : Vec3(a, a, a) {}
  constexpr Vec3(float a, float b, float c) : x(a), y(b), z(c) {}
  constexpr Vec3() : Vec3(0){}

  [[nodiscard]] constexpr auto operator+(const Vec3& other) const -> Vec3;
  auto operator+=(const Vec3& other) -> Vec3&;

  [[nodiscard]] auto operator-(const Vec3& other) const -> Vec3;
  auto operator-=(const Vec3& other) -> Vec3&;

  [[nodiscard]] auto operator*(float scalar) const -> Vec3;
  auto operator*=(float scalar) -> Vec3&;

  [[nodiscard]] auto operator/(float divisor) const -> Vec3;
  auto operator/=(float divisor) -> Vec3&;

  [[nodiscard]] auto Dot(const Vec3& other) const -> float;
  [[nodiscard]] auto Cross(const Vec3& other) const -> Vec3;

  [[nodiscard]] auto LengthSquared() const -> float;
  [[nodiscard]] auto Length() const -> float;

  [[nodiscard]] auto Normalized() const -> Vec3;
  auto Normalize() -> Vec3&;

  float x;
  float y;
  float z;
};

inline constexpr Vec3 Right{1, 0, 0};
inline constexpr Vec3 Up{0, 1, 0};
inline constexpr Vec3 Forward{0, 0, 1};

inline auto operator*(float scalar, const Vec3& vec3) -> Vec3 {
  return vec3 * scalar;
}

inline constexpr auto IsEqual(const float a, const float b, const float epsilon = 0.0001 ) -> bool
{
  return abs(a - b) < epsilon;
}

inline constexpr auto IsEqual(const Vec3& a, const Vec3& b, const float epsilon = 0.0001 ) -> bool
{
  return abs((a - b).LengthSquared()) < epsilon;
}
