#ifdef __EMSCRIPTEN__

#include <iostream>
#include <stdexcept>

#include "RenderDevice.h"
#include "Rendering/FrameContext.h"

RenderDevice::RenderDevice(int width, int height) : width_(width), height_(height) {
  InitializePlatform();
}

RenderDevice::~RenderDevice() {
  if (window_) {
    glfwDestroyWindow(window_);
    glfwTerminate();
  }
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

  glEnable(GL_LINE_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

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

// ----- Frame control -----
void RenderDevice::EndFrame() {
  // With Emscripten, we don't explicitly swap buffers
  // The browser handles this automatically
}

bool RenderDevice::ShouldClose() const {
  // In a web context, the window doesn't "close" in the traditional sense
  // Return false to keep the main loop running
  return false;
}

void RenderDevice::PollEvents() {
  // Emscripten handles event polling automatically via the main loop
  // No explicit polling needed
}

// ----- Input handling -----

void RenderDevice::MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  auto* device = static_cast<RenderDevice*>(glfwGetWindowUserPointer(window));
  if (device) {
    device->scrollAccumulator_ += static_cast<float>(yoffset);
  }
}

void RenderDevice::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
  auto* device = static_cast<RenderDevice*>(glfwGetWindowUserPointer(window));
  if (device) {
    device->fbWidth_ = width;
    device->fbHeight_ = height;
    device->framebufferResized_ = true;
    glViewport(0, 0, width, height);
  }
}

void RenderDevice::CaptureInput(FrameContext& context) {
  // Update viewport dimensions if framebuffer was resized
  if (framebufferResized_) {
    context.viewportWidth = static_cast<uint32_t>(fbWidth_);
    context.viewportHeight = static_cast<uint32_t>(fbHeight_);
    framebufferResized_ = false;
  }

  // Get current mouse position
  double mouseX, mouseY;
  glfwGetCursorPos(window_, &mouseX, &mouseY);

  // Calculate deltas from previous frame
  context.input.mouseDeltaX = static_cast<float>(mouseX - lastMouseX_);
  context.input.mouseDeltaY = static_cast<float>(mouseY - lastMouseY_);

  // Update stored position for next frame
  lastMouseX_ = mouseX;
  lastMouseY_ = mouseY;

  // Update current position
  context.input.mouseX = static_cast<float>(mouseX);
  context.input.mouseY = static_cast<float>(mouseY);

  // Capture accumulated scroll delta and reset
  context.input.scrollDelta = scrollAccumulator_;
  scrollAccumulator_ = 0.0f;

  // Button states
  context.input.mouseLeftDown = glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
  context.input.mouseMiddleDown =
      glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
  context.input.mouseRightDown = glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

  // Keyboard modifiers
  context.input.shiftDown = glfwGetKey(window_, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                            glfwGetKey(window_, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
  context.input.ctrlDown = glfwGetKey(window_, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
                           glfwGetKey(window_, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS;
  context.input.altDown = glfwGetKey(window_, GLFW_KEY_LEFT_ALT) == GLFW_PRESS ||
                          glfwGetKey(window_, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS;
}

#endif  // __EMSCRIPTEN__
