#include "OpenGLRenderDevice.h"

#include <cstring>
#include <iostream>
#include <vector>

#include "Rendering/Resources/VertexBuffer.h"
#include "Utilities/Mat4.h"
#include "Utilities/Vec3.h"

OpenGLRenderDevice::OpenGLRenderDevice(int width, int height) : width_(width), height_(height) {
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  // Set GLFW options
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // Create window
  window_ = glfwCreateWindow(width, height, "CAD Application", nullptr, nullptr);
  if (!window_) {
    glfwTerminate();
    throw std::runtime_error("Failed to initialize GLFW window");
  }

  glfwShowWindow(window_);
  glfwMakeContextCurrent(window_);

  // Initialize GLEW
  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    glfwTerminate();
    throw std::runtime_error("Failed to initialize GLEW");
  }

  InitializeOpenGL();
  std::cout << "OpenGL initialized successfully" << std::endl;
  std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void OpenGLRenderDevice::InitializeOpenGL() {
  // Enable depth testing
  glEnable(GL_DEPTH_TEST);
  // glEnable(GL_CULL_FACE); // Disable culling for debugging
  glFrontFace(GL_CW);  // Clockwise faces are front faces

  // Create and bind a VAO for modern OpenGL
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  currentVAO_ = vao;

  // Set viewport to framebuffer size for retina displays
  int fbWidth, fbHeight;
#ifndef __EMSCRIPTEN__
  glfwGetFramebufferSize(window_, &fbWidth, &fbHeight);
#else
  // For Emscripten, use the canvas size
  fbWidth = width_;
  fbHeight = height_;
#endif
  glViewport(0, 0, fbWidth, fbHeight);
}

OpenGLRenderDevice::~OpenGLRenderDevice() {
  // Clean up all resources
  for (auto& [handle, buffer] : buffers_) {
    glDeleteBuffers(1, &buffer.id);
  }
  buffers_.clear();

  for (auto& [handle, shader] : shaders_) {
    glDeleteProgram(shader.program);
  }
  shaders_.clear();

  if (window_) {
    glfwDestroyWindow(window_);
    glfwTerminate();
  }
}

uint32_t OpenGLRenderDevice::CreateVertexBuffer(std::size_t sizeBytes) {
  GLuint bufferId;
  glGenBuffers(1, &bufferId);

  BufferInfo info;
  info.id = bufferId;
  info.target = GL_ARRAY_BUFFER;
  info.size = sizeBytes;

  uint32_t handle = nextHandle_++;
  buffers_[handle] = info;

  return handle;
}

uint32_t OpenGLRenderDevice::CreateIndexBuffer(std::size_t sizeBytes) {
  GLuint bufferId;
  glGenBuffers(1, &bufferId);

  BufferInfo info;
  info.id = bufferId;
  info.target = GL_ELEMENT_ARRAY_BUFFER;
  info.size = sizeBytes;

  uint32_t handle = nextHandle_++;
  buffers_[handle] = info;

  return handle;
}

uint32_t OpenGLRenderDevice::CreateShader(const std::string& vertexSource,
                                          const std::string& fragmentSource,
                                          const std::string& geometrySource) {
  GLuint program = CreateShaderProgram(vertexSource, fragmentSource, geometrySource);
  if (program == 0) {
    std::cerr << "Failed to create shader program" << std::endl;
    return 0;
  }

  ShaderInfo info;
  info.program = program;

  uint32_t handle = nextHandle_++;
  shaders_[handle] = info;

  std::cout << "Created shader program with handle " << handle << std::endl;

  return handle;
}

void OpenGLRenderDevice::DestroyBuffer(uint32_t handle) {
  auto it = buffers_.find(handle);
  if (it != buffers_.end()) {
    glDeleteBuffers(1, &it->second.id);
    buffers_.erase(it);
  }
}

void OpenGLRenderDevice::DestroyShader(uint32_t handle) {
  auto it = shaders_.find(handle);
  if (it != shaders_.end()) {
    glDeleteProgram(it->second.program);
    shaders_.erase(it);
  }
}

void OpenGLRenderDevice::UpdateVertexBuffer(uint32_t handle, std::span<const std::byte> data) {
  auto it = buffers_.find(handle);
  if (it != buffers_.end()) {
    glBindBuffer(it->second.target, it->second.id);
    glBufferData(it->second.target, data.size(), data.data(), GL_STATIC_DRAW);
    glBindBuffer(it->second.target, 0);
  }
}

void OpenGLRenderDevice::UpdateIndexBuffer(uint32_t handle, std::span<const uint32_t> indices) {
  auto it = buffers_.find(handle);
  if (it != buffers_.end()) {
    glBindBuffer(it->second.target, it->second.id);
    glBufferData(it->second.target, indices.size() * sizeof(uint32_t), indices.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(it->second.target, 0);
  }
}

void OpenGLRenderDevice::BindVertexBuffer(uint32_t handle) {
  auto it = buffers_.find(handle);
  if (it != buffers_.end()) {
    glBindBuffer(it->second.target, it->second.id);
    currentVertexBuffer_ = handle;
  }
}

void OpenGLRenderDevice::SetVertexAttributes(uint32_t handle,
                                             const std::span<VertexAttribute>& attributes) {
  BindVertexBuffer(handle);

  unsigned int stride = 0;
  for (const VertexAttribute& attribute : attributes) {
    stride += attribute.size;
  }

  stride *= sizeof(float);

  for (const VertexAttribute& attribute : attributes) {
    std::cout << "Setting up vertex attribute: " << attribute.name
              << " location=" << attribute.location << ", size=" << attribute.size
              << ", offset=" << attribute.offset << ", stride=" << stride << std::endl;

    glEnableVertexAttribArray(attribute.location);
    glVertexAttribPointer(attribute.location,
                          attribute.size,  // 3 for Vec3
                          GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(attribute.offset));

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
      std::cout << "OpenGL error after setting up vertex attribute " << attribute.name << ": "
                << GetErrorString(error) << std::endl;
    }
  }
  std::cout << "SetupVertexAttributes completed" << std::endl;
}

void OpenGLRenderDevice::BindIndexBuffer(uint32_t handle) {
  auto it = buffers_.find(handle);
  if (it != buffers_.end()) {
    glBindBuffer(it->second.target, it->second.id);
    currentIndexBuffer_ = handle;
  }
}

void OpenGLRenderDevice::BindShader(uint32_t shaderHandle) {
  auto it = shaders_.find(shaderHandle);
  if (it != shaders_.end()) {
    glUseProgram(it->second.program);
    currentShader_ = shaderHandle;
  } else {
    std::cerr << "BindShader: shader handle " << shaderHandle << " not found" << std::endl;
  }
}

void OpenGLRenderDevice::SetUniform(const std::string& name, const Vec3& vec) {
  GLint location = GetUniformLocation(name);

  if (location != -1) {
    glUniform3f(location, vec.x, vec.y, vec.z);
  }
}

void OpenGLRenderDevice::SetUniform(const std::string& name, const float valueA,
                                    const float valueB) {
  GLint location = GetUniformLocation(name);

  if (location != -1) {
    glUniform2f(location, valueA, valueB);
  }
}

void OpenGLRenderDevice::SetUniform(const std::string& name, const Mat4& matrix) {
  GLint location = GetUniformLocation(name);

  if (location != -1) {
    // Assuming column-major order (OpenGL default)
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix.Data());
  }
}

void OpenGLRenderDevice::SetUniform(const std::string& name, int value) {
  GLint location = GetUniformLocation(name);

  if (location != -1) {
    glUniform1i(location, value);
  }
}

void OpenGLRenderDevice::SetUniform(const std::string& name, float value) {
  GLint location = GetUniformLocation(name);

  if (location != -1) {
    glUniform1f(location, value);
  }
}

void OpenGLRenderDevice::DrawIndexed(PrimitiveTopology topology, std::size_t indexCount) {
  GLenum mode = TopologyToGLenum(topology);

  // Check if we have all required state
  if (currentVertexBuffer_ == 0) {
    std::cerr << "Error: No vertex buffer bound" << std::endl;
    return;
  }
  if (currentIndexBuffer_ == 0) {
    std::cerr << "Error: No index buffer bound" << std::endl;
    return;
  }
  if (currentShader_ == 0) {
    std::cerr << "Error: No shader bound" << std::endl;
    return;
  }
  if (currentVAO_ == 0) {
    std::cerr << "Error: No VAO bound" << std::endl;
    return;
  }

  // Bind VAO for drawing
  glBindVertexArray(currentVAO_);

  glDrawElements(mode, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr);

  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    std::cerr << "OpenGL error after draw: " << error << " (" << GetErrorString(error) << ")"
              << std::endl;
  }
}

void OpenGLRenderDevice::BeginFrame() {
  // Set clear color to bright red for testing
  glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderDevice::EndFrame() {
#ifndef __EMSCRIPTEN__
  glfwSwapBuffers(window_);
#endif
}

bool OpenGLRenderDevice::ShouldClose() const {
#ifndef __EMSCRIPTEN__
  return glfwWindowShouldClose(window_);
#else
  // For Emscripten, we never "close" in the traditional sense
  // The application runs in the browser
  return false;
#endif
}

void OpenGLRenderDevice::PollEvents() {
#ifndef __EMSCRIPTEN__
  glfwPollEvents();
#endif
}

GLuint OpenGLRenderDevice::CompileShader(GLenum type, const std::string& source) {
  GLuint shader = glCreateShader(type);
  const char* sourcePtr = source.c_str();
  glShaderSource(shader, 1, &sourcePtr, nullptr);
  glCompileShader(shader);

  // Check compilation status
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLint logLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> infoLog(std::max(1, logLength));
    glGetShaderInfoLog(shader, logLength, nullptr, infoLog.data());
    std::cerr << "Shader compilation failed:\n" << infoLog.data() << std::endl;
    // Optionally print the shader source for easier debugging
    std::cerr << "--- Shader source begin ---" << std::endl;
    std::cerr << source << std::endl;
    std::cerr << "--- Shader source end ---" << std::endl;
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

GLuint OpenGLRenderDevice::CreateShaderProgram(const std::string& vertexSource,
                                               const std::string& fragmentSource,
                                               const std::string& geometrySource) {
  GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
  if (vertexShader == 0) return 0;

  GLuint geometryShader;
  if (!geometrySource.empty()) {
    geometryShader = CompileShader(GL_GEOMETRY_SHADER, geometrySource);
  }

  GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
  if (fragmentShader == 0) {
    glDeleteShader(vertexShader);
    return 0;
  }

  GLuint program = glCreateProgram();
  glAttachShader(program, vertexShader);
  if (geometryShader != 0) {
    glAttachShader(program, geometryShader);
  }
  glAttachShader(program, fragmentShader);
  // Explicitly bind known attribute locations so shaders don't need layout qualifiers
  // This makes attribute locations consistent across desktop GL and GLES/WebGL builds
  glBindAttribLocation(program, 0, "aPos");
  glLinkProgram(program);

  // Check linking status
  GLint success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    GLint logLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> infoLog(std::max(1, logLength));
    glGetProgramInfoLog(program, logLength, nullptr, infoLog.data());
    std::cerr << "Program linking failed:\n" << infoLog.data() << std::endl;

    // Try validating the program to get more info
    glValidateProgram(program);
    GLint validateStatus = GL_FALSE;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &validateStatus);
    if (validateStatus == GL_FALSE) {
      GLint validateLogLength = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &validateLogLength);
      std::vector<char> validateLog(std::max(1, validateLogLength));
      glGetProgramInfoLog(program, validateLogLength, nullptr, validateLog.data());
      std::cerr << "Program validation failed:\n" << validateLog.data() << std::endl;
    }

    // Dump active attributes
    GLint numAttribs = 0;
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numAttribs);
    std::cerr << "Active attributes: " << numAttribs << std::endl;
    for (GLint i = 0; i < numAttribs; ++i) {
      GLint nameLen = 0, size = 0;
      GLenum type = 0;
      glGetActiveAttrib(program, i, 256, &nameLen, &size, &type, infoLog.data());
      GLint loc = glGetAttribLocation(program, infoLog.data());
      std::cerr << "  attrib[" << i << "] " << infoLog.data() << " size=" << size
                << " type=" << type << " loc=" << loc << std::endl;
    }

    // Dump active uniforms
    GLint numUniforms = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);
    std::cerr << "Active uniforms: " << numUniforms << std::endl;
    for (GLint i = 0; i < numUniforms; ++i) {
      GLint nameLen = 0, size = 0;
      GLenum type = 0;
      glGetActiveUniform(program, i, 256, &nameLen, &size, &type, infoLog.data());
      GLint loc = glGetUniformLocation(program, infoLog.data());
      std::cerr << "  uniform[" << i << "] " << infoLog.data() << " size=" << size
                << " type=" << type << " loc=" << loc << std::endl;
    }

    glDeleteProgram(program);
    program = 0;
  }

  // Clean up shaders (they're linked into the program now)
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return program;
}

GLenum OpenGLRenderDevice::TopologyToGLenum(PrimitiveTopology topology) const {
  switch (topology) {
    case PrimitiveTopology::Lines:
      return GL_LINES;
    case PrimitiveTopology::Triangles:
      return GL_TRIANGLES;
    default:
      return GL_TRIANGLES;
  }
}

std::string OpenGLRenderDevice::GetErrorString(GLenum error) const {
  switch (error) {
    case GL_INVALID_ENUM:
      return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:
      return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:
      return "GL_INVALID_OPERATION";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return "GL_INVALID_FRAMEBUFFER_OPERATION";
    case GL_OUT_OF_MEMORY:
      return "GL_OUT_OF_MEMORY";
#ifndef __EMSCRIPTEN__
    case GL_STACK_UNDERFLOW:
      return "GL_STACK_UNDERFLOW";
    case GL_STACK_OVERFLOW:
      return "GL_STACK_OVERFLOW";
#endif
    default:
      return "Unknown error";
  }
}

GLint OpenGLRenderDevice::GetUniformLocation(const std::string& name) {
  GLint location = -1;

  if (currentShader_ == 0) return location;

  auto shaderIt = shaders_.find(currentShader_);
  if (shaderIt == shaders_.end()) return location;

  auto locIt = shaderIt->second.uniformLocations.find(name);
  if (locIt == shaderIt->second.uniformLocations.end()) {
    location = glGetUniformLocation(shaderIt->second.program, name.c_str());
    shaderIt->second.uniformLocations[name] = location;
  } else {
    location = locIt->second;
  }

  return location;
}