#pragma once

#include <cstddef>
#include <string>

#include "Utilities/Mat4.h"

struct alignas(16) UniformBuffer {
  float pointColor[4] = {0.f, 1.f, 0.f, 1.f};
  float lineColor[4] = {0.8f, 0.f, 0.5f, 1.f};
  float faceColor[4] = {1.f, 1.f, 0.5f, 1.f};
  float viewPortSize[2] = {800, 600};
  float lineThickness = 5.0;
  float axisLength = 0.15f;
  Mat4 viewMatrix = Mat4::Identity();
  Mat4 projectionMatrix = Mat4::Identity();
};

// GLSL uniform block definition generated from the struct above
// This ensures CPU and GPU layouts stay in sync
namespace ShaderCommon {

constexpr const char* GLSL_VERSION_DESKTOP = "#version 330 core\n";
constexpr const char* GLSL_VERSION_ES = "#version 300 es\n";

constexpr const char* GLSL_UNIFORMS = R"(
layout(std140) uniform GlobalUniforms
{
    vec4 pointColor;
    vec4 lineColor;
    vec4 faceColor;
    vec2 viewPortSize;
    float lineThickness;
    float axisLength;
    mat4 viewMatrix;
    mat4 projectionMatrix;
};
)";

// Helper to get the appropriate version string based on platform
#ifdef __EMSCRIPTEN__
constexpr const char* GetGLSLVersion() { return GLSL_VERSION_ES; }
#else
constexpr const char* GetGLSLVersion() { return GLSL_VERSION_DESKTOP; }
#endif

// Complete shader preamble (version + uniforms)
inline std::string GetShaderPreamble() { return std::string(GetGLSLVersion()) + GLSL_UNIFORMS; }

}  // namespace ShaderCommon
