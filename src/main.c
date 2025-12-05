#include <stdio.h>
#include <unistd.h>
#include <math.h>

#include "glad/glad.h"

#include <GLFW/glfw3.h>

#include "shader/vertex_color.fp.glsl.h"
#include "shader/vertex_color.vp.glsl.h"

#include "shader/font.fp.glsl.h"
#include "shader/font.vp.glsl.h"

#include "font/ter_u32n.data.h"

#include "opengl.h"
#include "render.hpp"
#include "update.hpp"

#include "model/block.h"
#include "model/paddle.h"
#include "model/ball.h"
//#include "model/cube.h"
#include "model/plane.h"

int vp_width = 1600;
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

float deadzone(float n)
{
  if (fabsf(n) < 0.02)
    return 0.0f;
  else
    return n;
}

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
  struct mesh block_mesh;
  struct mesh ball_mesh;
  block_mesh.vtx = make_buffer(GL_ARRAY_BUFFER, block_vertices, (sizeof (block_vertices)));
  block_mesh.idx = make_buffer(GL_ELEMENT_ARRAY_BUFFER, block_Cube_triangles, (sizeof (block_Cube_triangles)));
  block_mesh.length = block_Cube_triangles_length;
  //block_mesh.vtx = make_buffer(GL_ARRAY_BUFFER, cube_vertices, (sizeof (cube_vertices)));
  //block_mesh.idx = make_buffer(GL_ELEMENT_ARRAY_BUFFER, cube_Cube_triangles, (sizeof (cube_Cube_triangles)));
  //block_mesh.length = cube_Cube_triangles_length;

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
  uint attrib_texture = glGetAttribLocation(program, "_texture");
  uint attrib_normal = glGetAttribLocation(program, "normal");
  uint uniform_trans = glGetUniformLocation(program, "trans");
  uint uniform_normal_trans = glGetUniformLocation(program, "normal_trans");
  uint uniform_base_color = glGetUniformLocation(program, "base_color");
  uint uniform_light_pos = glGetUniformLocation(program, "light_pos");

  //
  struct mesh plane_mesh;

  plane_mesh.vtx = make_buffer(GL_ARRAY_BUFFER, plane_vertices, (sizeof (plane_vertices)));
  plane_mesh.idx = make_buffer(GL_ELEMENT_ARRAY_BUFFER, plane_Plane_triangles, (sizeof (plane_Plane_triangles)));
  plane_mesh.length = plane_Plane_triangles_length;

  uint font_program = compile_shader(src_shader_font_vp_glsl_start,
                                     src_shader_font_vp_glsl_size,
                                     src_shader_font_fp_glsl_start,
                                     src_shader_font_fp_glsl_size);
  glUseProgram(font_program);
  uint font__attrib_position = glGetAttribLocation(font_program, "position");
  uint font__attrib_texture = glGetAttribLocation(font_program, "_texture");
  uint font__attrib_normal = glGetAttribLocation(font_program, "normal");
  uint font__uniform_trans = glGetUniformLocation(font_program, "trans");
  uint font__uniform_texture_trans = glGetUniformLocation(font_program, "texture_trans");
  uint font__uniform_texture0 = glGetUniformLocation(font_program, "texture0");

  //////////////////////////////////////////////////////////////////////
  // textures
  //////////////////////////////////////////////////////////////////////

  uint terminus_font = make_texture(src_font_ter_u32n_data_start,
                                    GL_RED,
                                    256,
                                    256,
                                    GL_RED);
  (void)terminus_font;

  //////////////////////////////////////////////////////////////////////
  // main loop
  //////////////////////////////////////////////////////////////////////

  const double frame_rate = 60.0;
  const double first_frame = glfwGetTime();
  double last_frame = first_frame;
  double frames = 1;
  const char * last_gamepad_name = NULL;

  struct game_state state;
  reset_level(&state);
  state.start_time = glfwGetTime();

  while(!glfwWindowShouldClose(window)) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

    glClearDepth(-1000.0f);
    glClearColor(0.1, 0.2, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float paddle_dx = 0.0;
    //float paddle_dy = 0.0;

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
        float sensitivity = 0.4f;
        paddle_dx = (right - left) * sensitivity;

        state.ball_dx = deadzone(axes[0]);
        state.ball_dy = deadzone(axes[1]);
        break;
      }
    }

    float extent = 0.25;
    state.paddle_x += paddle_dx;
    if (state.paddle_x < extent)
      state.paddle_x = extent;
    if (state.paddle_x > 12 - extent)
      state.paddle_x = 12 - extent;

    update(&state);
    double time = glfwGetTime();
    state.remaining = 20.0 - (time - state.start_time);

    if ((state.ball_x + state.ball_dx * 0.4) > 12.25f) {
      state.ball_x = 12.25f;
      state.ball_dx = -state.ball_dx;
    } else if ((state.ball_x + state.ball_dx * 0.4) < -0.25f) {
      state.ball_x = -0.25f;
      state.ball_dx = -state.ball_dx;
    }

    if ((state.ball_y + state.ball_dy * 0.4) > 27.0f) {
      state.ball_y = 27.0f;
      state.ball_dy = -state.ball_dy;
    } else if ((state.ball_y + state.ball_dy * 0.4) < 0.0f) {
      state.ball_y = 0.0f;
      state.ball_dy = -state.ball_dy;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);
    glUseProgram(program);

    render(paddle_mesh,
           block_mesh,
           ball_mesh,
           attrib_position,
           attrib_texture,
           attrib_normal,
           uniform_trans,
           uniform_normal_trans,
           uniform_base_color,
           uniform_light_pos,
           &state);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
    glUseProgram(font_program);

    render_font(plane_mesh,
                font__attrib_position,
                font__attrib_texture,
                font__attrib_normal,
                font__uniform_trans,
                font__uniform_texture_trans,
                font__uniform_texture0,
                &state);

    glfwSwapBuffers(window);
    glfwPollEvents();

    double next_frame = last_frame + 1.0 / frame_rate;
    last_frame = glfwGetTime();
    while (next_frame - glfwGetTime() > 0) {
      double delta = next_frame - glfwGetTime();
      if (delta < 0)
        break;
      glfwWaitEventsTimeout(delta);
    }
    //printf("fps %f\n", frames / (glfwGetTime() - first_frame));

    frames += 1;
  }

  glfwTerminate();

  return 0;
}
