#include <math.h>
#include <stdio.h>
#include <assert.h>

#include "glad/glad.h"

#include "unparse.h"
#include "render.hpp"
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

void render_blocks(mesh block_mesh,
                   uint attrib_position,
                   uint attrib_texture,
                   uint attrib_normal,
                   uint uniform_trans,
                   uint uniform_normal_trans,
                   uint uniform_base_color,
                   uint uniform_light_pos,
                   uint uniform_time,
                   struct game_state * state)
{
  light_pos_theta += 0.01;

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

  //////////////////////////////////////////////////////////////////////
  // render blocks
  //////////////////////////////////////////////////////////////////////

  glUniform1f(uniform_time, state->time);

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

      vec3 block_position = vec3(x * 4.0f, -y * 2.0f, 0.0f);
      float rot = 0.0;

      if (destroyed_time != 0.0) {
        base_color = vec4(1, 0, 0, (float)((2.0 - dt) * 0.5));
        block_position.y -= dt * 10.0;
        block_position.z += 1;
        float dir = fabsf(sin(destroyed_time * 100)) / sin(destroyed_time * 100);
        rot = dt * dir;
      }

      mat4x4 rx = rotate_x(-PI / 2.0f);
      mat4x4 rz = rotate_z(rot);
      mat4x4 t = translate(block_position);

      mat4x4 trans = a * t * rz * rx;

      //mat3x3 normal_trans = transpose(inverse(submatrix(trans, 0, 0)));
      mat3x3 normal_trans = submatrix(rx, 3, 3);

      glUniform4fv(uniform_trans, 4, &trans[0][0]);
      glUniform3fv(uniform_normal_trans, 3, &normal_trans[0][0]);
      glUniform4fv(uniform_base_color, 1, &base_color[0]);
      glUniform3fv(uniform_light_pos, 1, &light_pos[0]);

      glDrawElements(GL_TRIANGLES, block_mesh.length, GL_UNSIGNED_INT, 0);
    }
  }
}

vec3 sample_palette(float d, vec3 p) {
  vec3 v = vec3(d, d, d) + p;
  v = v * vec3(6.28318548202514648438f) ;
  v = vec3(cos(v.x), cos(v.y), cos(v.z));
  return vec3(0.5, 0.5, 0.5) * v + vec3(0.5, 0.5, 0.5); // 48
}

void render_balls(mesh ball_mesh,
                  uint attrib_position,
                  uint attrib_texture,
                  uint attrib_normal,
                  uint uniform_trans,
                  uint uniform_normal_trans,
                  uint uniform_base_color,
                  uint uniform_light_pos,
                  struct game_state * state)
{
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

  //////////////////////////////////////////////////////////////////////
  // render balls
  //////////////////////////////////////////////////////////////////////

  mat4x4 a = aspect_mat();
  vec3 light_pos = _light_pos();

  for (int i = 0; i < state->balls_launched; i++) {
    struct ball_state& ball = state->balls[i];

    mat4x4 rx = rotate_x((float)state->time);
    mat4x4 ry = rotate_y((float)state->time * 0.5f);
    vec3 ball_position = vec3(ball.ball_x * 4.0f, -ball.ball_y * 2.0f, 0.0);
    mat4x4 t = translate(ball_position);

    mat4x4 trans = a * t * ry * rx;
    mat3x3 normal_trans = submatrix(ry * rx, 3, 3);

    vec4 base_color = vec4(0.5f, 0.5f, 0.5f, 1.0f);
    if (ball.super_ball) {
      float hue = state->time - ball.launch_time;
      base_color = sample_palette(hue, vec3(0, 0.33, 0.67));
    }

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

void render_text(struct mesh plane_mesh,
                 uint uniform_trans,
                 uint uniform_texture_trans,
                 const char ** text,
                 int text_length,
                 float aspect,
                 mat4x4 a,
                 mat4x4 s,
                 mat4x4 r,
                 float _scale,
                 int h_advance_init,
                 int v_advance)
{
  int h_advance = h_advance_init;
  for (int i = 0; i < text_length; i++) {
    const char * txt = text[i];
    while (*txt) {
      char c = *txt++;

      if (c != ' ') {
        mat4x4 char_t = translate(vec3(-2 * 16 + h_advance / 8.0f,
                                       -8.0f * aspect + -v_advance / 16.0f, 0));
        mat4x4 trans = a * scale(_scale / vp_height) * s * char_t * r;

        mat4x4 texture_trans = char_tex_trans(c);

        glUniform4fv(uniform_trans, 4, &trans[0][0]);
        glUniform4fv(uniform_texture_trans, 4, &texture_trans[0][0]);
        glDrawElements(GL_TRIANGLES, plane_mesh.length, GL_UNSIGNED_INT, 0);
      }

      h_advance += grid_width;
    }
    v_advance += grid_height;
    h_advance = h_advance_init;
  }
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
  glUniform1f(uniform_time, state->time);

  //////////////////////////////////////////////////////////////////////
  // instructions
  //////////////////////////////////////////////////////////////////////

  const char * intro[] = {
    "This game requires a gamepad.",
    "Keyboard is not supported.",
    "PlayStation DualShock 4 recommended.",
    "Press cross to launch ball(s).",
    "Analog triggers move the paddle.",
    "Finish level 1 before the timer elapses.",
  };
  const int intro_length = (sizeof (intro)) / (sizeof (intro[0]));

  const char * loss[] = {
    "You were not prepared.",
    "Press option to retry.",
  };
  const int loss_length = (sizeof (loss)) / (sizeof (loss[0]));

  const char * win[] = {
    "Your performance is satisfactory.",
  };
  const int win_length = (sizeof (win)) / (sizeof (win[0]));

  float text_scale = 12.0f;

  if (state->intro_shown == 0) {
    vec3 base_color = vec3(1, 1, 1);
    glUniform3fv(uniform_base_color, 1, &base_color[0]);
    render_text(plane_mesh,
                uniform_trans,
                uniform_texture_trans,
                intro,
                intro_length,
                aspect,
                a, s, r,
                text_scale,
                -grid_width * 3, 0);
  } else if (state->remaining <= 0) {
    if (state->level_ix == 0) {
      vec3 base_color = vec3(1, 0.1, 0.1);
      glUniform3fv(uniform_base_color, 1, &base_color[0]);
      render_text(plane_mesh,
                  uniform_trans,
                  uniform_texture_trans,
                  loss,
                  loss_length,
                  aspect,
                  a, s, r,
                  text_scale,
                  0, 0);
    } else {
      vec3 base_color = vec3(0.1, 1.0, 0.1);
      glUniform3fv(uniform_base_color, 1, &base_color[0]);
      render_text(plane_mesh,
                  uniform_trans,
                  uniform_texture_trans,
                  win,
                  win_length,
                  aspect,
                  a, s, r,
                  text_scale,
                  0, 0);
    }
  }

  char dst[64];
  const char * dst_l[] = { dst };

  //////////////////////////////////////////////////////////////////////
  // remaining
  //////////////////////////////////////////////////////////////////////

  glUniform1f(uniform_time, state->time + 0.5);

  double remaining = state->remaining;
  if (remaining < 0.0)
    remaining = 0.0;

  vec3 base_color = vec3(1, 1, 1);
  if (remaining == 0) {
    base_color = vec3(abs(sin(state->time * 2)) * 0.6 + 0.4, 0.1, 0.1);
    //base_color = vec3(1, 0.1, 0.1);
  }
  glUniform3fv(uniform_base_color, 1, &base_color[0]);

  int len = unparse_double(remaining, 2, 1, dst);
  dst[len] = 0;

  render_text(plane_mesh,
              uniform_trans,
              uniform_texture_trans,
              dst_l,
              1,
              aspect,
              a, s, r,
              28.0f, // scale
              13.75 * grid_width, 1.95 * grid_height);


  //////////////////////////////////////////////////////////////////////
  // ball count
  //////////////////////////////////////////////////////////////////////
  {
    dst[0] = 'b';
    dst[1] = 'l';
    int len = unparse_double(20 - state->balls_launched, 2, 1, &dst[2]);
    dst[2] = ':';
    for (int i = 0; i < len; i++) {
      if (dst[2 + i] == '.')
        dst[2 + i] = 0;
    }

    vec3 base_color = vec3(1, 1, 1);
    glUniform3fv(uniform_base_color, 1, &base_color[0]);

    render_text(plane_mesh,
                uniform_trans,
                uniform_texture_trans,
                dst_l,
                1,
                aspect,
                a, s, r,
                20.0f, // scale
                28 * grid_width, 3.75 * grid_height);
  }

  //////////////////////////////////////////////////////////////////////
  //
  //////////////////////////////////////////////////////////////////////
  {
    dst[0] = 'l';
    dst[1] = 'v';
    int len = unparse_double(state->level_ix + 1, 2, 1, &dst[2]);
    dst[2] = ':';
    for (int i = 0; i < len; i++) {
      if (dst[2 + i] == '.')
        dst[2 + i] = 0;
    }

    vec3 base_color = vec3(1, 1, 1);
    glUniform3fv(uniform_base_color, 1, &base_color[0]);

    render_text(plane_mesh,
                uniform_trans,
                uniform_texture_trans,
                dst_l,
                1,
                aspect,
                a, s, r,
                20.0f, // scale
                0 * grid_width, 3.75 * grid_height);
  }
}

void render_background(struct mesh plane_mesh,
                       uint attrib_position,
                       uint uniform_resolution,
                       uint uniform_trans,
                       uint uniform_texture1,
                       uint uniform_time,
                       uint uniform_palette,
                       uint uniform_aspect,
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

  glUniform1i(uniform_texture1, 1);

  glUniform1f(uniform_time, state->time_bg);
  vec2 resolution = vec2(vp_width, vp_height);
  glUniform2fv(uniform_resolution, 1, &resolution[0]);

  const vec3 palettes[5] = {
    vec3(0.440f, 0.021f, 0.512f),
    vec3(0.131f, 0.958f, 0.678f),
    vec3(0.507f, 0.407f, 0.103f),
    vec3(0.709f, 0.240f, 0.475f),
    vec3(0.658f, 0.944f, 0.719f),
  };
  glUniform3fv(uniform_palette, 1, &palettes[state->level_ix % 5][0]);

  float aspect = (float)vp_height / (float)vp_width;
  glUniform1f(uniform_aspect, aspect);

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
