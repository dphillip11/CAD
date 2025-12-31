#include "Vec3.h"

#include <cassert>

auto Vec3::operator+=(const Vec3& other) -> Vec3& {
  x += other.x;
  y += other.y;
  z += other.z;
  return *this;
}

auto Vec3::operator-(const Vec3& other) const -> Vec3 {
  return {x - other.x, y - other.y, z - other.z};
}

auto Vec3::operator-=(const Vec3& other) -> Vec3& {
  x -= other.x;
  y -= other.y;
  z -= other.z;
  return *this;
}

auto Vec3::operator*(float scalar) const -> Vec3 { return {scalar * x, scalar * y, scalar * z}; }

auto Vec3::operator*=(float scalar) -> Vec3& {
  x *= scalar;
  y *= scalar;
  z *= scalar;
  return *this;
}

auto Vec3::operator/(float divisor) const -> Vec3 {
  assert(!IsEqual(divisor, 0.0f));
  return {x / divisor, y / divisor, z / divisor};
}

auto Vec3::operator/=(float divisor) -> Vec3& {
  assert(!IsEqual(divisor, 0.0f));
  x /= divisor;
  y /= divisor;
  z /= divisor;
  return *this;
}

auto Vec3::Dot(const Vec3& other) const -> float {
  return (x * other.x) + (y * other.y) + (z * other.z);
}

auto Vec3::Cross(const Vec3& other) const -> Vec3 {
  return {(y * other.z) - (z * other.y), (z * other.x) - (x * other.z),
          (x * other.y) - (y * other.x)};
}

auto Vec3::LengthSquared() const -> float { return (x * x) + (y * y) + (z * z); }

auto Vec3::Length() const -> float { return std::sqrt(LengthSquared()); }

auto Vec3::Normalized() const -> Vec3 {
  float length = Length();
  return length == 0.0F ? Vec3(0) : Vec3(x, y, z) / length;
}

auto Vec3::Normalize() -> Vec3& {
  float length = Length();
  if (length != 0.0F) {
    x /= length;
    y /= length;
    z /= length;
  }
  return *this;
}
