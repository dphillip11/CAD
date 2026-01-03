#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>

#include "RenderDevice.h"
#include "Rendering/Resources/UniformBuffer.h"
#include "Rendering/Resources/VertexAttribute.h"
#include "Utilities/Mat4.h"
#include "Utilities/Vec3.h"

// Constructor, destructor, and InitializePlatform are in platform-specific files

GpuHandle RenderDevice::CreatePipeline() {
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  return vao;
}

GpuHandle RenderDevice::CreateBuffer() {
  GLuint bufferId;
  glGenBuffers(1, &bufferId);
  return bufferId;
}

GpuHandle RenderDevice::CreateShader(const std::string& vertexSource,
                                     const std::string& fragmentSource,
                                     const std::string& geometrySource) {
  GLuint program = CreateShaderProgram(vertexSource, fragmentSource, geometrySource);
  if (program == 0) {
    std::cerr << "Failed to create shader program" << std::endl;
    return 0;
  }

  return program;
}

GpuHandle RenderDevice::CreateTexture2D(const float width, const float height,
                                        bool generateMipmaps) {
  GLuint textureId;
  glGenTextures(1, &textureId);

  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Allocate texture storage
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

  if (generateMipmaps) {
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  return textureId;
}

GpuHandle RenderDevice::CreateDepthTexture2D(const float width, const float height) {
  GLuint textureId;
  glGenTextures(1, &textureId);

  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Allocate depth texture storage
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT,
               GL_UNSIGNED_INT, nullptr);

  return textureId;
}

GpuHandle RenderDevice::CreateTexture1D(uint32_t width) {
  GLuint textureId;
  glGenTextures(1, &textureId);

  glBindTexture(GL_TEXTURE_2D, textureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Allocate 1D texture as a 2D texture with height=1 (WebGL compatible)
  // Using GL_R32UI for unsigned integer data
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, width, 1, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);

  return textureId;
}

GpuHandle RenderDevice::CreateFrameBuffer(GpuHandle colorHandle, GpuHandle depthHandle,
                                          GpuHandle stencilHandle) {
  GLuint fboId;
  glGenFramebuffers(1, &fboId);
  glBindFramebuffer(GL_FRAMEBUFFER, fboId);

  if (colorHandle != 0) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorHandle, 0);
  }

  // Attach depth attachment if provided
  if (depthHandle != 0) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthHandle, 0);
  }

  // Attach stencil attachment if provided
  if (stencilHandle != 0) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, stencilHandle, 0);
  }

  // Check framebuffer completeness
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "Framebuffer is not complete: " << status << std::endl;
    glDeleteFramebuffers(1, &fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return 0;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return fboId;
}

void RenderDevice::DestroyBuffer(GpuHandle handle) { glDeleteBuffers(1, &handle); }

void RenderDevice::DestroyShader(GpuHandle handle) { glDeleteProgram(handle); }

void RenderDevice::DestroyTexture(GpuHandle handle) {
  GLuint textureId = handle;
  glDeleteTextures(1, &textureId);
}

void RenderDevice::UpdateVertexBuffer(GpuHandle handle, const size_t bytes, const void* data) {
  glBindBuffer(GL_ARRAY_BUFFER, handle);
  glBufferData(GL_ARRAY_BUFFER, bytes, data, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderDevice::UpdateUniformBuffer(GpuHandle handle, const size_t bytes, const void* data,
                                       const uint32_t position) {
  glBindBuffer(GL_UNIFORM_BUFFER, handle);
  glBufferData(GL_UNIFORM_BUFFER, bytes, data, GL_DYNAMIC_DRAW);

  glBindBufferBase(GL_UNIFORM_BUFFER, position, handle);

  if (currentShader_ != 0) {
    GLuint blockIndex = glGetUniformBlockIndex(currentShader_, "GlobalUniforms");
    if (blockIndex != GL_INVALID_INDEX) {
      glUniformBlockBinding(currentShader_, blockIndex, position);
    }
  }
}

void RenderDevice::UpdateIndexBuffer(GpuHandle handle, std::span<const uint32_t> indices) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderDevice::UpdateTexture1D(GpuHandle textureHandle, std::span<const uint32_t> data) {
  glBindTexture(GL_TEXTURE_2D, textureHandle);
  // Update 1D texture stored as 2D with height=1
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, data.size(), 1, GL_RED_INTEGER, GL_UNSIGNED_INT,
                  data.data());
  // Don't unbind - texture should remain bound to its texture unit
}

void RenderDevice::BindPipeline(GpuHandle handle) { glBindVertexArray(handle); }

void RenderDevice::BindVertexBuffer(GpuHandle handle) { glBindBuffer(GL_ARRAY_BUFFER, handle); }

void RenderDevice::SetVertexAttributes(GpuHandle handle,
                                       const std::span<VertexAttribute>& attributes) {
  BindVertexBuffer(handle);

  unsigned int stride = 0;
  for (const VertexAttribute& attribute : attributes) {
    stride += attribute.size;
  }

  stride *= sizeof(float);

  for (const VertexAttribute& attribute : attributes) {
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
}

void RenderDevice::BindIndexBuffer(GpuHandle handle) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
}

void RenderDevice::BindShader(GpuHandle shaderHandle) {
  glUseProgram(shaderHandle);
  currentShader_ = shaderHandle;
}

void RenderDevice::BindTexture(GpuHandle handle, const uint32_t index) {
  glActiveTexture(GL_TEXTURE0 + index);
  glBindTexture(GL_TEXTURE_2D, handle);
}

void RenderDevice::BindFrameBuffer(GpuHandle handle) { glBindFramebuffer(GL_FRAMEBUFFER, handle); }

void RenderDevice::ReadPixel(uint32_t x, uint32_t y, uint8_t* rgba) {
  // Note: Coordinates are in framebuffer space with origin at bottom-left
  glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
}

void RenderDevice::SetUniform(const std::string& name, const Vec3& vec) {
  GLint location = GetUniformLocation(name);

  if (location != -1) {
    glUniform3f(location, vec.x, vec.y, vec.z);
  }
}

void RenderDevice::SetUniform(const std::string& name, const float valueA, const float valueB) {
  GLint location = GetUniformLocation(name);

  if (location != -1) {
    glUniform2f(location, valueA, valueB);
  }
}

void RenderDevice::SetUniform(const std::string& name, const Mat4& matrix) {
  GLint location = GetUniformLocation(name);

  if (location != -1) {
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix.Data());
  }
}

void RenderDevice::SetUniform(const std::string& name, int value) {
  GLint location = GetUniformLocation(name);

  if (location != -1) {
    glUniform1i(location, value);
  }
}

void RenderDevice::SetUniform(const std::string& name, float value) {
  GLint location = GetUniformLocation(name);

  if (location != -1) {
    glUniform1f(location, value);
  }
}

void RenderDevice::DrawIndexed(PrimitiveTopology topology, std::size_t indexCount) {
  // Debug: Check current OpenGL state
  GLint vao = 0, ebo = 0;
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vao);
  glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ebo);

  if (vao == 0) {
    std::cerr << "ERROR: No VAO bound!" << std::endl;
  }
  if (ebo == 0) {
    std::cerr << "ERROR: No index buffer bound!" << std::endl;
  }

  GLenum mode = TopologyToGLenum(topology);

#ifdef __EMSCRIPTEN__
  // WebGL doesn't support geometry shaders, so use glLineWidth for line thickness
  // Note: Most WebGL implementations only support line width of 1
  if (topology == PrimitiveTopology::Lines) {
    glLineWidth(2.0f);  // WebGL typically only supports lineWidth = 1
  }
#endif

  glDrawElements(mode, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr);

  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    std::cerr << "OpenGL error after draw: " << error << " (" << GetErrorString(error) << ")"
              << std::endl;
  }
}

void RenderDevice::Draw(PrimitiveTopology topology, std::size_t vertexCount) {
  // Debug: Check current OpenGL state
  GLint vao = 0;
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vao);

  if (vao == 0) {
    std::cerr << "ERROR: No VAO bound!" << std::endl;
  }

  GLenum mode = TopologyToGLenum(topology);

  glDrawArrays(mode, 0, static_cast<GLsizei>(vertexCount));

  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    std::cerr << "OpenGL error after draw: " << error << " (" << GetErrorString(error) << ")"
              << std::endl;
  }
}

void RenderDevice::SetViewport(int x, int y, int width, int height) {
  glViewport(x, y, width, height);
}

void RenderDevice::SetClearColor(float r, float g, float b, float a) { glClearColor(r, g, b, a); }

void RenderDevice::Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

void RenderDevice::BeginFrame() {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // White background
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLuint RenderDevice::CompileShader(GLenum type, const std::string& source) {
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

GLuint RenderDevice::CreateShaderProgram(const std::string& vertexSource,
                                         const std::string& fragmentSource,
                                         const std::string& geometrySource) {
  // Get shader preamble (version directive + uniform block definition)
  std::string preamble = ShaderCommon::GetShaderPreamble();

  // Prepend preamble to each shader source
  std::string fullVertexSource = preamble + vertexSource;
  std::string fullFragmentSource = preamble + fragmentSource;

  GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, fullVertexSource);
  if (vertexShader == 0) return 0;

  GLuint geometryShader = 0;
#ifndef __EMSCRIPTEN__
  // Geometry shaders are not supported in WebGL/OpenGL ES
  if (!geometrySource.empty()) {
    std::string fullGeometrySource = preamble + geometrySource;
    geometryShader = CompileShader(GL_GEOMETRY_SHADER, fullGeometrySource);
    if (geometryShader == 0) {
      glDeleteShader(vertexShader);
      return 0;
    }
  }
#endif

  GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fullFragmentSource);
  if (fragmentShader == 0) {
    glDeleteShader(vertexShader);
    if (geometryShader != 0) glDeleteShader(geometryShader);
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
  if (geometryShader != 0) {
    glDeleteShader(geometryShader);
  }

  return program;
}

GLenum RenderDevice::TopologyToGLenum(PrimitiveTopology topology) const {
  switch (topology) {
    case PrimitiveTopology::Lines:
      return GL_LINES;
    case PrimitiveTopology::Triangles:
      return GL_TRIANGLES;
    default:
      return GL_POINTS;
  }
}

std::string RenderDevice::GetErrorString(GLenum error) const {
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

GLint RenderDevice::GetUniformLocation(const std::string& name) {
  return glGetUniformLocation(currentShader_, name.c_str());
}
