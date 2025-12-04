unsigned int compile_shader(const char * vp_path, const char * fp_path)
{
  void * vertexShaderSource = read_file(vp_path);
  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, (const char **)&vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  {
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
      glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
      printf("vertex shader compile failed:\n%s\n", infoLog);
    }
  }
  free(vertexShaderSource);

  void * fragmentShaderSource = read_file(fp_path);
  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, (const char **)&fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  {
    int  success;
    char infoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
      glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
      printf("fragment shader compile failed:\n%s\n", infoLog);
    }
  }
  free(fragmentShaderSource);

  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();

  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  {
    int  success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
      glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
      printf("program link failed:\n%s\n", infoLog);
    }
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
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
