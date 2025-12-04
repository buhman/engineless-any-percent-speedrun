#include <stdio.h>
#include <unistd.h>

#include "glad/glad.h"

#include <GLFW/glfw3.h>

#include "shader/vertex_color.fp.glsl.h"
#include "shader/vertex_color.vp.glsl.h"

#include "opengl.h"
#include "render.hpp"

#include "model/brick.h"
#include "model/paddle.h"
#include "model/ball.h"

int vp_width = 2400;
int vp_height = 1200;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
  vp_width = width;
  vp_height = height;
}

/*
static const float triangle_vertex_buffer_data[] = {
  // position          // color
   0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
  -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
   0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top
};
*/

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

  /*
  uint triangle_vertex_buffer = make_buffer(GL_ARRAY_BUFFER,
                                            triangle_vertex_buffer_data,
                                            (sizeof (triangle_vertex_buffer_data)));
  */

  struct mesh paddle_mesh;
  struct mesh brick_mesh;
  struct mesh ball_mesh;
  brick_mesh.vtx = make_buffer(GL_ARRAY_BUFFER, brick_vertices, (sizeof (brick_vertices)));
  brick_mesh.idx = make_buffer(GL_ELEMENT_ARRAY_BUFFER, brick_Cube_triangles, (sizeof (brick_Cube_triangles)));
  brick_mesh.length = brick_Cube_triangles_length;

  paddle_mesh.vtx = make_buffer(GL_ARRAY_BUFFER, paddle_vertices, (sizeof (paddle_vertices)));
  paddle_mesh.idx = make_buffer(GL_ELEMENT_ARRAY_BUFFER, paddle_Cylinder_triangles, (sizeof (paddle_Cylinder_triangles)));
  paddle_mesh.length = paddle_Cylinder_triangles_length;

  ball_mesh.vtx = make_buffer(GL_ARRAY_BUFFER, ball_vertices, (sizeof (ball_vertices)));
  ball_mesh.idx = make_buffer(GL_ELEMENT_ARRAY_BUFFER, ball_Icosphere_triangles, (sizeof (ball_Icosphere_triangles)));
  ball_mesh.length = ball_Icosphere_triangles_length;

  uint program = compile_shader(src_shader_vertex_color_vp_glsl_start,
                                src_shader_vertex_color_vp_glsl_size,
                                src_shader_vertex_color_fp_glsl_start,
                                src_shader_vertex_color_fp_glsl_size);
  glUseProgram(program);
  uint attrib_position = glGetAttribLocation(program, "position");
  uint attrib_normal = glGetAttribLocation(program, "normal");
  uint uniform_trans = glGetUniformLocation(program, "trans");
  uint uniform_normal_trans = glGetUniformLocation(program, "normal_trans");
  uint uniform_base_color = glGetUniformLocation(program, "base_color");
  uint uniform_light_pos = glGetUniformLocation(program, "light_pos");

  //////////////////////////////////////////////////////////////////////
  // main loop
  //////////////////////////////////////////////////////////////////////

  const double frame_rate = 60.0;
  const double first_frame = glfwGetTime();
  double last_frame = first_frame;
  double frames = 0;
  const char * last_gamepad_name = NULL;

  float paddle_x = 0.0;

  while(!glfwWindowShouldClose(window)) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

    glEnable(GL_DEPTH_TEST);
    glClearDepth(-1000.0f);
    glDepthFunc(GL_GREATER);
    glClearColor(0.1, 0.2, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float paddle_dx = 0.0;
    for (int i = 0; i < 16; i++) {
      int present = glfwJoystickPresent(GLFW_JOYSTICK_1 + i);
      int is_gamepad = glfwJoystickIsGamepad(GLFW_JOYSTICK_1 + i);
      int count;
      const float * axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1 + i, &count);
      //printf("present, %d %d %s %d\n", i, count, name, is_gamepad);
      if (present && is_gamepad && count == 6) {
        const char * name = glfwGetJoystickName(GLFW_JOYSTICK_1 + i);
        if (name != last_gamepad_name) {
          printf("active gamepad: `%s`; axes: %d\n", name, count);
          last_gamepad_name = name;
        }
        float left = axes[2] * 0.5 + 0.5;
        float right = axes[5] * 0.5 + 0.5;
        paddle_dx = right - left;
        break;
      }
    }

    paddle_x += paddle_dx * 0.4;
    if (paddle_x < 0.5)
      paddle_x = 0.5;
    if (paddle_x > 11.5)
      paddle_x = 11.5;

    render(paddle_mesh,
           brick_mesh,
           ball_mesh,
           attrib_position,
           attrib_normal,
           uniform_trans,
           uniform_normal_trans,
           uniform_base_color,
           uniform_light_pos,
           paddle_x);

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
