#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>

#include "glad/glad.h"

#include <GLFW/glfw3.h>

#include "shader/vertex_color.fp.glsl.h"
#include "shader/vertex_color.vp.glsl.h"
#include "shader/font.fp.glsl.h"
#include "shader/font.vp.glsl.h"
#include "shader/background.fp.glsl.h"
#include "shader/background.vp.glsl.h"
#include "shader/paddle.fp.glsl.h"
#include "shader/block.fp.glsl.h"

#include "font/ter_u32n.data.h"
#include "texture/noise.data.h"

#include "opengl.h"
#include "render.hpp"
#include "update.hpp"

#include "model/block.h"
#include "model/paddle.h"
#include "model/ball.h"
//#include "model/cube.h"
#include "model/plane.h"

int vp_width = 800;
int vp_height = 600;

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

#ifdef __linux__
// Linux DS4
#define X_BUTTON 0 // cross
#define O_BUTTON 1 // circle
#define T_BUTTON 2 // triangle
#define S_BUTTON 3 // square
#define OPTIONS_BUTTON 9
#define LEFT_AXIS 2
#define RIGHT_AXIS 5
#else
// Windows DS4
#define X_BUTTON 1
#define OPTIONS_BUTTON 9
#define LEFT_AXIS 3
#define RIGHT_AXIS 4
#endif

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
  // mesh initialization
  //////////////////////////////////////////////////////////////////////

  struct mesh paddle_mesh;
  struct mesh block_mesh;
  struct mesh ball_mesh;
  struct mesh plane_mesh;
  block_mesh.vtx = make_buffer(GL_ARRAY_BUFFER, block_vertices, (sizeof (block_vertices)));
  block_mesh.idx = make_buffer(GL_ELEMENT_ARRAY_BUFFER, block_Cube_triangles, (sizeof (block_Cube_triangles)));
  block_mesh.length = block_Cube_triangles_length;

  //cube_mesh.vtx = make_buffer(GL_ARRAY_BUFFER, cube_vertices, (sizeof (cube_vertices)));
  //cube_mesh.idx = make_buffer(GL_ELEMENT_ARRAY_BUFFER, cube_Cube_triangles, (sizeof (cube_Cube_triangles)));
  //cube_mesh.length = cube_Cube_triangles_length;

  paddle_mesh.vtx = make_buffer(GL_ARRAY_BUFFER, paddle_vertices, (sizeof (paddle_vertices)));
  paddle_mesh.idx = make_buffer(GL_ELEMENT_ARRAY_BUFFER, paddle_Cylinder_triangles, (sizeof (paddle_Cylinder_triangles)));
  paddle_mesh.length = paddle_Cylinder_triangles_length;

  ball_mesh.vtx = make_buffer(GL_ARRAY_BUFFER, ball_vertices, (sizeof (ball_vertices)));
  ball_mesh.idx = make_buffer(GL_ELEMENT_ARRAY_BUFFER, ball_Icosphere_triangles, (sizeof (ball_Icosphere_triangles)));
  ball_mesh.length = ball_Icosphere_triangles_length;

  plane_mesh.vtx = make_buffer(GL_ARRAY_BUFFER, plane_vertices, (sizeof (plane_vertices)));
  plane_mesh.idx = make_buffer(GL_ELEMENT_ARRAY_BUFFER, plane_Plane_triangles, (sizeof (plane_Plane_triangles)));
  plane_mesh.length = plane_Plane_triangles_length;

  //////////////////////////////////////////////////////////////////////
  // shaders
  //////////////////////////////////////////////////////////////////////

  //

  uint ball_program = compile_shader(src_shader_vertex_color_vp_glsl_start,
                                     src_shader_vertex_color_vp_glsl_size,
                                     src_shader_vertex_color_fp_glsl_start,
                                     src_shader_vertex_color_fp_glsl_size);
  uint ball__attrib_position = glGetAttribLocation(ball_program, "position");
  uint ball__attrib_texture = glGetAttribLocation(ball_program, "_texture");
  uint ball__attrib_normal = glGetAttribLocation(ball_program, "normal");
  uint ball__uniform_trans = glGetUniformLocation(ball_program, "trans");
  uint ball__uniform_normal_trans = glGetUniformLocation(ball_program, "normal_trans");
  uint ball__uniform_base_color = glGetUniformLocation(ball_program, "base_color");
  uint ball__uniform_light_pos = glGetUniformLocation(ball_program, "light_pos");

  // block

  uint block_program = compile_shader(src_shader_vertex_color_vp_glsl_start,
                                      src_shader_vertex_color_vp_glsl_size,
                                      src_shader_block_fp_glsl_start,
                                      src_shader_block_fp_glsl_size);
  uint block__attrib_position = glGetAttribLocation(block_program, "position");
  uint block__attrib_texture = glGetAttribLocation(block_program, "_texture");
  uint block__attrib_normal = glGetAttribLocation(block_program, "normal");
  uint block__uniform_trans = glGetUniformLocation(block_program, "trans");
  uint block__uniform_normal_trans = glGetUniformLocation(block_program, "normal_trans");
  uint block__uniform_base_color = glGetUniformLocation(block_program, "base_color");
  uint block__uniform_light_pos = glGetUniformLocation(block_program, "light_pos");
  uint block__uniform_time = glGetUniformLocation(block_program, "time");

  // font

  uint font_program = compile_shader(src_shader_font_vp_glsl_start,
                                     src_shader_font_vp_glsl_size,
                                     src_shader_font_fp_glsl_start,
                                     src_shader_font_fp_glsl_size);
  uint font__attrib_position = glGetAttribLocation(font_program, "position");
  uint font__attrib_texture = glGetAttribLocation(font_program, "_texture");
  uint font__uniform_trans = glGetUniformLocation(font_program, "trans");
  uint font__uniform_texture_trans = glGetUniformLocation(font_program, "texture_trans");
  uint font__uniform_texture0 = glGetUniformLocation(font_program, "texture0");
  uint font__uniform_base_color = glGetUniformLocation(font_program, "base_color");
  uint font__uniform_time = glGetUniformLocation(font_program, "time");

  // background

  uint bg_program = compile_shader(src_shader_background_vp_glsl_start,
                                   src_shader_background_vp_glsl_size,
                                   src_shader_background_fp_glsl_start,
                                   src_shader_background_fp_glsl_size);
  uint bg__attrib_position = glGetAttribLocation(bg_program, "position");
  uint bg__uniform_resolution = glGetUniformLocation(bg_program, "resolution");
  uint bg__uniform_trans = glGetUniformLocation(bg_program, "trans");
  uint bg__uniform_texture1 = glGetUniformLocation(bg_program, "texture1");
  uint bg__uniform_time = glGetUniformLocation(bg_program, "time");
  uint bg__uniform_palette = glGetUniformLocation(bg_program, "palette");
  uint bg__uniform_aspect = glGetUniformLocation(bg_program, "aspect");

  // paddle

  uint paddle_program = compile_shader(src_shader_vertex_color_vp_glsl_start,
                                       src_shader_vertex_color_vp_glsl_size,
                                       src_shader_paddle_fp_glsl_start,
                                       src_shader_paddle_fp_glsl_size);
  uint paddle__attrib_position = glGetAttribLocation(paddle_program, "position");
  uint paddle__attrib_texture = glGetAttribLocation(paddle_program, "_texture");
  uint paddle__attrib_normal = glGetAttribLocation(paddle_program, "normal");
  uint paddle__uniform_trans = glGetUniformLocation(paddle_program, "trans");
  uint paddle__uniform_normal_trans = glGetUniformLocation(paddle_program, "normal_trans");
  uint paddle__uniform_light_pos = glGetUniformLocation(paddle_program, "light_pos");
  uint paddle__uniform_time = glGetUniformLocation(paddle_program, "time");

  //////////////////////////////////////////////////////////////////////
  // textures
  //////////////////////////////////////////////////////////////////////

  glActiveTexture(GL_TEXTURE0);
  uint terminus_font = make_texture(src_font_ter_u32n_data_start,
                                    GL_RGB,
                                    256,
                                    256,
                                    GL_RED);
  (void)terminus_font;

  glActiveTexture(GL_TEXTURE1);
  uint noise_texture = make_texture(src_texture_noise_data_start,
                                    GL_RGBA,
                                    256,
                                    256,
                                    GL_RGBA);
  (void)noise_texture;

  //////////////////////////////////////////////////////////////////////
  // main loop
  //////////////////////////////////////////////////////////////////////

  const double frame_rate = 60.0;
  const double first_frame = glfwGetTime();
  double last_frame = first_frame;
  //double frames = 1;
  const char * last_gamepad_name = NULL;

  struct game_state state = {0};
  reset_game(&state);
  state.start_time = glfwGetTime();

  glEnable(GL_DEPTH_TEST);

  while(!glfwWindowShouldClose(window)) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

    glClearDepth(-1000.0f);
    glClearColor(0.1, 0.2, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float paddle_dx = 0.0;
    //float paddle_dy = 0.0;
    static bool last_x_press = false;
    static bool last_option_press = false;
    bool x_press = false;
    bool option_press = false;

    for (int i = 0; i < 16; i++) {
      int present = glfwJoystickPresent(GLFW_JOYSTICK_1 + i);
      int is_gamepad = glfwJoystickIsGamepad(GLFW_JOYSTICK_1 + i);
      int count;
      const float * axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1 + i, &count);

      //printf("present, %d %d %s %d\n", i, count, name, is_gamepad);
      if (present && is_gamepad && count == 6) {
        const unsigned char * buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1 + i, &count);

        #ifdef DEBUG_BUTTONS
        //printf("buttons count %d\n", count);
        for (int i = 0; i < count; i++) {
          printf("[% 2d % 2d] ", i, buttons[i]);
        }
        printf("\n");
        #endif

        #ifdef DEBUG_AXES
        for (int i = 0; i < 6; i++) {
          printf("[% 2d % 2.03f] ", i, axes[i]);
        }
        printf("\n");
        #endif

        x_press = buttons[X_BUTTON] != 0;
        option_press = buttons[OPTIONS_BUTTON] != 0;

        const char * name = glfwGetJoystickName(GLFW_JOYSTICK_1 + i);
        if (name != last_gamepad_name) {
          printf("active gamepad: `%s`; axes: 6 ; buttons: %d\n", name, count);
          last_gamepad_name = name;
        }

        float left = axes[LEFT_AXIS] * 0.5 + 0.5;
        float right = axes[RIGHT_AXIS] * 0.5 + 0.5;
        float sensitivity = 0.4f;
        paddle_dx = (right - left) * sensitivity;

        //state.ball_dx = deadzone(axes[0]);
        //state.ball_dy = deadzone(axes[1]);
        break;
      }
    }

    double time = glfwGetTime();

    if (!last_x_press && x_press) {
      launch_ball(&state, time);
    }
    last_x_press = x_press;

    if (!last_option_press && option_press) {
      reset_game(&state);
    }
    last_option_press = option_press;

    float extent = 0.25;
    state.paddle_x += paddle_dx;
    if (state.paddle_x < extent)
      state.paddle_x = extent;
    if (state.paddle_x > 12 - extent)
      state.paddle_x = 12 - extent;

    update(&state, time);

    glDisable(GL_BLEND);
    glDepthFunc(GL_ALWAYS);
    glUseProgram(bg_program);
    render_background(plane_mesh,
                      bg__attrib_position,
                      bg__uniform_resolution,
                      bg__uniform_trans,
                      bg__uniform_texture1,
                      bg__uniform_time,
                      bg__uniform_palette,
                      bg__uniform_aspect,
                      &state);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_GREATER);
    glUseProgram(block_program);
    render_blocks(block_mesh,
                  block__attrib_position,
                  block__attrib_texture,
                  block__attrib_normal,
                  block__uniform_trans,
                  block__uniform_normal_trans,
                  block__uniform_base_color,
                  block__uniform_light_pos,
                  block__uniform_time,
                  &state);

    glDisable(GL_BLEND);
    glDepthFunc(GL_GREATER);
    glUseProgram(ball_program);
    render_balls(ball_mesh,
                 ball__attrib_position,
                 ball__attrib_texture,
                 ball__attrib_normal,
                 ball__uniform_trans,
                 ball__uniform_normal_trans,
                 ball__uniform_base_color,
                 ball__uniform_light_pos,
                 &state);

    glDisable(GL_BLEND);
    glDepthFunc(GL_GREATER);
    glUseProgram(paddle_program);
    render_paddle(paddle_mesh,
                  paddle__attrib_position,
                  paddle__attrib_texture,
                  paddle__attrib_normal,
                  paddle__uniform_trans,
                  paddle__uniform_normal_trans,
                  paddle__uniform_light_pos,
                  paddle__uniform_time,
                  &state);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_ALWAYS);
    glUseProgram(font_program);
    render_font(plane_mesh,
                font__attrib_position,
                font__attrib_texture,
                font__uniform_trans,
                font__uniform_texture_trans,
                font__uniform_texture0,
                font__uniform_base_color,
                font__uniform_time,
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
    //frames += 1;
  }

  glfwTerminate();

  return 0;
}
