#include <math.h>
#include <stdio.h>
#include <assert.h>

#include "glad/glad.h"

#include "unparse.h"
#include "render.hpp"
#include "color.hpp"
#include "math/float_types.hpp"
#include "math/transform.hpp"

#define PI 3.14159274101257324219f

extern int vp_width;
extern int vp_height;

mat4x4 perspective(float low1, float high1,
                   float low2, float high2)
{
  float scale = (high2 - low2) / (high1 - low1);

  mat4x4 m1 = mat4x4(1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, 1, -low1,
                     0, 0, 0, 1
                     );

  mat4x4 m2 = mat4x4(1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, 0, 1,
                     0, 0, scale, low2
                     );

  mat4x4 m3 = m2 * m1;

  /*
  mat4x4 m3 = mat4x4(1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, 0, 1,
                     0, 0, 1, 0
                     );
  */

  if (0) {
    for (int i = 0; i < 4; i++) {
      printf("% 2.3f % 2.3f % 2.3f % 2.3f \n", m3[i][0], m3[i][1], m3[i][2], m3[i][3]);
    }

    vec4 v = vec4(5, 6, 0.5, 1);
    vec4 vv = m3 * v;
    printf("\n% 2.3f % 2.3f % 2.3f % 2.3f \n", vv[0], vv[1], vv[2], vv[3]);
    exit(1);
  }

  return m3;
}

static const float x_scale = 1.0f / 12.0f;
//static const float y_scale = 1.0f / 27.0f;
static const float y_scale = 1.0f / 12.0f;

static inline float px(float x)
{
  return (x * x_scale) * 2.0 - 1.0 + x_scale;
}

static inline float py(float y)
{
  return (y * y_scale) * -2.0 + 1.0 - y_scale;
}

/*
  levels are 13x28
 */

static inline mat4x4 aspect_mat()
{
  float aspect = (float)vp_height / (float)vp_width;
  mat4x4 p = perspective(-1, 1, 0, 1);
  mat4x4 tt = translate(vec3(-1.2f, 1.4f, 2.0f));
  mat4x4 a = scale(vec3(aspect, 1.0f, 1.0f)) * p * tt * scale(0.05f);
  return a;
}

static float light_pos_theta = 0;

static inline vec3 _light_pos()
{
  vec3 p = vec3(1, 1, 1);
  vec3 pos = normalize(rotate_z(light_pos_theta) * p);
  return pos;
}

void render(mesh block_mesh,
            mesh ball_mesh,
            uint attrib_position,
            uint attrib_texture,
            uint attrib_normal,
            uint uniform_trans,
            uint uniform_normal_trans,
            uint uniform_base_color,
            uint uniform_light_pos,
            struct game_state * state)
{
  light_pos_theta += 0.01;

  //////////////////////////////////////////////////////////////////////
  // render blocks
  //////////////////////////////////////////////////////////////////////

  glBindBuffer(GL_ARRAY_BUFFER, block_mesh.vtx);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, block_mesh.idx);

  glVertexAttribPointer(attrib_position,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        (sizeof (float)) * 8,
                        (void*)(0 * 4)
                        );
  glVertexAttribPointer(attrib_texture,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        (sizeof (float)) * 8,
                        (void*)(3 * 4)
                        );
  glVertexAttribPointer(attrib_normal,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        (sizeof (float)) * 8,
                        (void*)(5 * 4)
                        );
  glEnableVertexAttribArray(attrib_position);
  glEnableVertexAttribArray(attrib_texture);
  glEnableVertexAttribArray(attrib_normal);

  mat4x4 a = aspect_mat();
  vec3 light_pos = _light_pos();

  for (int y = 0; y < 28; y++) {
    for (int x = 0; x < 13; x++) {
      int block_ix = y * 13 + x;
      char tile = state->level[block_ix];
      if (tile == 0)
        continue;
      double destroyed_time = state->blocks[block_ix].destroyed_time;
      double dt = state->time - destroyed_time;
      if (destroyed_time != 0.0 && dt >= 2.0)
        continue;

      const float cs = 1.0f / 255.0f;

      vec4 base_color = vec4(((float)state->pal[tile * 3 + 0]) * cs,
                             ((float)state->pal[tile * 3 + 1]) * cs,
                             ((float)state->pal[tile * 3 + 2]) * cs,
                             1.0f);
      if (destroyed_time != 0.0) {
        base_color = vec4(1, 0, 0, (float)((2.0 - dt) * 0.5));
      }

      vec3 block_position = vec3(x * 4.0f, -y * 2.0f, 0.0f);

      mat4x4 rx = rotate_x(-PI / 2.0f);
      mat4x4 t = translate(block_position);

      mat4x4 trans = a * t * rx;

      //mat3x3 normal_trans = transpose(inverse(submatrix(trans, 0, 0)));
      mat3x3 normal_trans = submatrix(rx, 3, 3);

      glUniform4fv(uniform_trans, 4, &trans[0][0]);
      glUniform3fv(uniform_normal_trans, 3, &normal_trans[0][0]);
      glUniform4fv(uniform_base_color, 1, &base_color[0]);
      glUniform3fv(uniform_light_pos, 1, &light_pos[0]);

      glDrawElements(GL_TRIANGLES, block_mesh.length, GL_UNSIGNED_INT, 0);
    }
  }

  //////////////////////////////////////////////////////////////////////
  // render balls
  //////////////////////////////////////////////////////////////////////

  for (int i = 0; i < state->balls_launched; i++) {
    struct ball_state& ball = state->balls[i];

    mat4x4 rx = rotate_x((float)state->time);
    mat4x4 ry = rotate_y((float)state->time * 0.5f);
    vec3 ball_position = vec3(ball.ball_x * 4.0f, -ball.ball_y * 2.0f, 0.0);
    mat4x4 t = translate(ball_position);

    mat4x4 trans = a * t * ry * rx;
    mat3x3 normal_trans = submatrix(ry * rx, 3, 3);
    float hue = state->time - ball.launch_time;
    hue = hue - floorf(hue);
    vec3 c = hsv_to_rgb(hue, 1.0f, 1.0f);
    vec4 base_color = vec4(c.x, c.y, c.z, 1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, ball_mesh.vtx);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ball_mesh.idx);

    glVertexAttribPointer(attrib_position,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          (sizeof (float)) * 8,
                          (void*)(0 * 4)
                          );
    glVertexAttribPointer(attrib_texture,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          (sizeof (float)) * 8,
                          (void*)(3 * 4)
                          );
    glVertexAttribPointer(attrib_normal,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          (sizeof (float)) * 8,
                          (void*)(5 * 4)
                          );
    glEnableVertexAttribArray(attrib_position);
    glEnableVertexAttribArray(attrib_texture);
    glEnableVertexAttribArray(attrib_normal);

    glUniform4fv(uniform_trans, 4, &trans[0][0]);
    glUniform3fv(uniform_normal_trans, 3, &normal_trans[0][0]);
    glUniform4fv(uniform_base_color, 1, &base_color[0]);
    glUniform3fv(uniform_light_pos, 1, &light_pos[0]);

    glDrawElements(GL_TRIANGLES, ball_mesh.length, GL_UNSIGNED_INT, 0);
  }
}

const int grid_width = 16;
const int grid_height = 32;
const int grid_stride = 256 / grid_width;
const int first_character = 0x20;
const float grid_scale = 1.0f / 256.0f;

static inline mat4x4 char_tex_trans(char c)
{
  int cc = c - first_character;
  int x = cc % grid_stride;
  int y = cc / grid_stride;

  mat4x4 tex_t = translate(vec3((float)(x * grid_width) * grid_scale,
                                (float)(y * grid_height) * grid_scale,
                                0));
  mat4x4 tex_s = scale(vec3(grid_width * grid_scale, grid_height * grid_scale, 1.0f));
  mat4x4 texture_trans = tex_t * tex_s;

  return texture_trans;
}

static inline int max(int a, int b)
{
  if (a > b)
    return a;
  else
    return b;
}

void render_font(struct mesh plane_mesh,
                 uint attrib_position,
                 uint attrib_texture,
                 uint uniform_trans,
                 uint uniform_texture_trans,
                 uint uniform_texture0,
                 uint uniform_base_color,
                 uint uniform_time,
                 struct game_state * state)
{
  glBindBuffer(GL_ARRAY_BUFFER, plane_mesh.vtx);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane_mesh.idx);

  glVertexAttribPointer(attrib_position,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        (sizeof (float)) * 8,
                        (void*)(0 * 4)
                        );
  glVertexAttribPointer(attrib_texture,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        (sizeof (float)) * 8,
                        (void*)(3 * 4)
                        );
  glEnableVertexAttribArray(attrib_position);
  glEnableVertexAttribArray(attrib_texture);

  float aspect = (float)vp_height / (float)vp_width;

  mat4x4 a = scale(vec3(aspect, 1.0f, 1.0f));
  mat4x4 s = scale(vec3(1, 2, 1));
  mat4x4 r = rotate_y(PI / 1.0f) * rotate_z(PI / 1.0f) * rotate_x(PI / 2.0f);

  glUniform1i(uniform_texture0, 0);

  char dst[64];

  double remaining = state->remaining;
  if (remaining < 0.0)
    remaining = 0.0;
  int len = unparse_double(remaining, 4, 1, dst);

  vec3 base_color = vec3(1, 1, 1);
  if (remaining == 0) {
    base_color = vec3(abs(sin(state->time * 2)) * 0.6 + 0.4, 0.1, 0.1);
    //base_color = vec3(1, 0.1, 0.1);
  }

  glUniform1f(uniform_time, state->time);
  glUniform3fv(uniform_base_color, 1, &base_color[0]);

  int advance = 0;
  for (int i = 0; i < len; i++) {
    if (dst[i] != ' ') {
      mat4x4 texture_trans = char_tex_trans(dst[i]);

      mat4x4 char_t = translate(vec3(advance / 8.0f, -8.0f * aspect, 0));
      mat4x4 trans = a * scale(32.0f / vp_height) * s * char_t * r;

      glUniform4fv(uniform_trans, 4, &trans[0][0]);
      glUniform4fv(uniform_texture_trans, 4, &texture_trans[0][0]);
      glDrawElements(GL_TRIANGLES, plane_mesh.length, GL_UNSIGNED_INT, 0);
    }
    advance += grid_width;
  }
}

void render_background(struct mesh plane_mesh,
                       uint attrib_position,
                       uint uniform_resolution,
                       uint uniform_trans,
                       uint uniform_time,
                       struct game_state * state)
{
  glBindBuffer(GL_ARRAY_BUFFER, plane_mesh.vtx);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane_mesh.idx);

  glVertexAttribPointer(attrib_position,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        (sizeof (float)) * 8,
                        (void*)(0 * 4)
                        );
  glEnableVertexAttribArray(attrib_position);

  mat4x4 r = rotate_y(PI / 1.0f) * rotate_z(PI / 1.0f) * rotate_x(PI / 2.0f);
  mat4x4 trans = r;
  glUniform4fv(uniform_trans, 4, &trans[0][0]);
  glUniform1f(uniform_time, state->time);
  vec2 resolution = vec2(vp_width, vp_height);
  glUniform2fv(uniform_resolution, 1, &resolution[0]);

  glDrawElements(GL_TRIANGLES, plane_mesh.length, GL_UNSIGNED_INT, 0);
}



//////////////////////////////////////////////////////////////////////
// render paddle
//////////////////////////////////////////////////////////////////////

void render_paddle(mesh paddle_mesh,
                   uint attrib_position,
                   uint attrib_texture,
                   uint attrib_normal,
                   uint uniform_trans,
                   uint uniform_normal_trans,
                   uint uniform_light_pos,
                   uint uniform_time,
                   struct game_state * state)
{
  mat4x4 rx = rotate_y(PI / 2.0f);
  mat4x4 ry = rotate_x((float)state->time);

  mat4x4 t = translate(vec3(state->paddle_x * 4.0f, -state->paddle_y * 2.0f, 0.0));
  mat4x4 a = aspect_mat();

  mat4x4 trans = a * t * ry * rx;
  mat3x3 normal_trans = submatrix(ry * rx, 3, 3);
  vec3 light_pos = _light_pos();

  glBindBuffer(GL_ARRAY_BUFFER, paddle_mesh.vtx);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, paddle_mesh.idx);

  glVertexAttribPointer(attrib_position,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        (sizeof (float)) * 8,
                        (void*)(0 * 4)
                        );
  glVertexAttribPointer(attrib_texture,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        (sizeof (float)) * 8,
                        (void*)(3 * 4)
                        );
  glVertexAttribPointer(attrib_normal,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        (sizeof (float)) * 8,
                        (void*)(5 * 4)
                        );
  glEnableVertexAttribArray(attrib_position);
  glEnableVertexAttribArray(attrib_texture);
  glEnableVertexAttribArray(attrib_normal);

  glUniform4fv(uniform_trans, 4, &trans[0][0]);
  glUniform3fv(uniform_normal_trans, 3, &normal_trans[0][0]);
  glUniform3fv(uniform_light_pos, 1, &light_pos[0]);
  glUniform1f(uniform_time, state->time);

  glDrawElements(GL_TRIANGLES, paddle_mesh.length, GL_UNSIGNED_INT, 0);
}
