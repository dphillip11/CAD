#ifndef __EMSCRIPTEN__

#include <iostream>
#include <stdexcept>

#include "RenderDevice.h"

RenderDevice::RenderDevice(int width, int height) : width_(width), height_(height) {
  InitializePlatform();
}

void RenderDevice::InitializePlatform() {
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  // Set GLFW options for native OpenGL
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // Create window
  window_ = glfwCreateWindow(width_, height_, "CAD Application", nullptr, nullptr);
  if (!window_) {
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }

  glfwShowWindow(window_);
  glfwMakeContextCurrent(window_);

  // Initialize GLEW (native only - not needed on Emscripten)
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    glfwTerminate();
    throw std::runtime_error("Failed to initialize GLEW");
  }

  // Enable depth testing
  glEnable(GL_DEPTH_TEST);
  glFrontFace(GL_CW);  // Clockwise faces are front faces

  // Enable alpha blending for transparency
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Set viewport to framebuffer size for retina displays
  glfwGetFramebufferSize(window_, &fbWidth_, &fbHeight_);
  glViewport(0, 0, fbWidth_, fbHeight_);

  std::cout << "OpenGL initialized successfully" << std::endl;
  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cout << "Window size: " << width_ << "x" << height_ << std::endl;
  std::cout << "Framebuffer size: " << fbWidth_ << "x" << fbHeight_ << std::endl;
}

#endif  // __EMSCRIPTEN__
