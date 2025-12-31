#ifdef __EMSCRIPTEN__

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

  // Set GLFW options for WebGL/Emscripten
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

  // Create window
  window_ = glfwCreateWindow(width_, height_, "CAD Application", nullptr, nullptr);
  if (!window_) {
    glfwTerminate();
    throw std::runtime_error("Failed to create GLFW window");
  }

  glfwShowWindow(window_);
  glfwMakeContextCurrent(window_);

  // No GLEW on Emscripten - WebGL context is ready to use

  // Enable depth testing
  glEnable(GL_DEPTH_TEST);
  glFrontFace(GL_CW);  // Clockwise faces are front faces

  // Enable alpha blending for transparency
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Set viewport to framebuffer size
  glfwGetFramebufferSize(window_, &fbWidth_, &fbHeight_);
  glViewport(0, 0, fbWidth_, fbHeight_);

  // Set up input callbacks
  glfwSetWindowUserPointer(window_, this);
  glfwSetScrollCallback(window_, MouseScrollCallback);
  glfwSetFramebufferSizeCallback(window_, FramebufferSizeCallback);

  // Trigger initial viewport setup by marking framebuffer as resized
  framebufferResized_ = true;

  std::cout << "WebGL initialized successfully" << std::endl;
  std::cout << "WebGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL ES version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cout << "Window size: " << width_ << "x" << height_ << std::endl;
  std::cout << "Framebuffer size: " << fbWidth_ << "x" << fbHeight_ << std::endl;
}

#endif  // __EMSCRIPTEN__
