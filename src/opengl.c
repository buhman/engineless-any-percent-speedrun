#include <stddef.h>
#include <stdio.h>

#include "glad/glad.h"

unsigned int compile_shader(const void * vp,
                            const int vp_length,
                            const void * fp,
                            const int fp_length)
{
  unsigned int vertex_shader;
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, (const char **)&vp, &vp_length);
  glCompileShader(vertex_shader);
  {
    int  success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
      glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
      fprintf(stderr, "vertex shader compile failed:\n%s\n", info_log);
    }
  }

  unsigned int fragment_shader;
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, (const char **)&fp, &fp_length);
  glCompileShader(fragment_shader);
  {
    int  success;
    char info_log[512];
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
      glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
      fprintf(stderr, "fragment shader compile failed:\n%s\n", info_log);
    }
  }

  unsigned int shader_program;
  shader_program = glCreateProgram();

  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  {
    int  success;
    char info_log[512];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success) {
      glGetProgramInfoLog(shader_program, 512, NULL, info_log);
      fprintf(stderr, "program link failed:\n%s\n", info_log);
    }
  }

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return shader_program;
}

int make_buffer(unsigned int target,
                const void * data,
                size_t size)
{
  unsigned int buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(target, buffer);
  glBufferData(target, size, data, GL_STATIC_DRAW);
  return buffer;
}
