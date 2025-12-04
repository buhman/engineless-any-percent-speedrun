#include <stdio.h>
#include <unistd.h>

#include "glad/glad.h"

#include <GLFW/glfw3.h>

#include "shader/vertex_color.fp.glsl.h"
#include "shader/vertex_color.vp.glsl.h"

#include "opengl.h"

static int vp_width = 800;
static int vp_height = 600;

typedef unsigned int uint;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
  vp_width = width;
  vp_height = height;
}

static const float triangle_vertex_buffer_data[] = {
  // position          // color
   0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
  -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
   0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top
};

int main()
{
  //////////////////////////////////////////////////////////////////////
  // window initialization
  //////////////////////////////////////////////////////////////////////

#ifdef __linux__
  glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

  GLFWwindow* window = glfwCreateWindow(vp_width, vp_height, "breakout", NULL, NULL);
  if (window == NULL) {
    const char* description;
    glfwGetError(&description);
    fprintf(stderr, "Failed to create GLFW window: %s\n", description);
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    fprintf(stderr, "gladLoadGLLoader error\n");
    return -1;
  }

  glViewport(0, 0, vp_width, vp_height);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  //////////////////////////////////////////////////////////////////////
  // buffer initialization
  //////////////////////////////////////////////////////////////////////

  uint triangle_vertex_buffer = make_buffer(GL_ARRAY_BUFFER,
                                            triangle_vertex_buffer_data,
                                            (sizeof (triangle_vertex_buffer_data)));

  uint vertex_color_program = compile_shader(src_shader_vertex_color_vp_glsl_start,
                                             src_shader_vertex_color_vp_glsl_size,
                                             src_shader_vertex_color_fp_glsl_start,
                                             src_shader_vertex_color_fp_glsl_size);
  glUseProgram(vertex_color_program);
  uint vertex_color_attrib_position = glGetAttribLocation(vertex_color_program, "position");
  uint vertex_color_attrib_color = glGetAttribLocation(vertex_color_program, "color");

  //////////////////////////////////////////////////////////////////////
  // main loop
  //////////////////////////////////////////////////////////////////////

  fwrite(src_shader_vertex_color_fp_glsl_start,
         src_shader_vertex_color_fp_glsl_size,
         1,
         stdout);

  const double frame_rate = 60.0;
  const double first_frame = glfwGetTime();
  double last_frame = first_frame;
  double frames = 0;
  while(!glfwWindowShouldClose(window)) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

    glBindBuffer(GL_ARRAY_BUFFER, triangle_vertex_buffer);
    glVertexAttribPointer(vertex_color_attrib_position,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          (sizeof (float)) * 6,
                          (void*)0
                          );
    glVertexAttribPointer(vertex_color_attrib_color,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          (sizeof (float)) * 6,
                          (void*)(3 * 4)
                          );
    glEnableVertexAttribArray(vertex_color_attrib_position);
    glEnableVertexAttribArray(vertex_color_attrib_color);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();

    double next_frame = last_frame + 1.0 / frame_rate;
    while (next_frame - glfwGetTime() > 0) {
      double delta = next_frame - glfwGetTime();
      glfwWaitEventsTimeout(delta);
    }
    //printf("fps %f\n", frames / (glfwGetTime() - first_frame));

    frames += 1;
  }

  glfwTerminate();

  return 0;
}
