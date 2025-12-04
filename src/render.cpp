#include <math.h>
#include <stdio.h>
#include <assert.h>

#include "glad/glad.h"

#include "render.hpp"
#include "math/float_types.hpp"
#include "math/transform.hpp"

#include "level/level1.data.h"
#include "level/level1.data.pal.h"

#define PI 3.14159274101257324219f

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

/*
  levels are 13x28
 */

void render(mesh paddle_mesh,
            mesh brick_mesh,
            uint attrib_position,
            uint attrib_normal,
            uint uniform_trans,
            uint uniform_normal_trans,
            uint uniform_base_color,
            uint uniform_light_pos,
            float paddle_x)
{
  static float theta = 0;

  theta += 0.01;

  //////////////////////////////////////////////////////////////////////
  // render bricks
  //////////////////////////////////////////////////////////////////////

  glBindBuffer(GL_ARRAY_BUFFER, brick_mesh.vtx);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, brick_mesh.idx);

  glVertexAttribPointer(attrib_position,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        (sizeof (float)) * 8,
                        (void*)(0 * 4)
                        );
  /*
    glVertexAttribPointer(vertex_color_attrib_texture,
    2,
    GL_FLOAT,
    GL_FALSE,
    (sizeof (float)) * 8,
    (void*)(3 * 4)
    );
  */
  glVertexAttribPointer(attrib_normal,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        (sizeof (float)) * 8,
                        (void*)(5 * 4)
                        );
  glEnableVertexAttribArray(attrib_position);
  glEnableVertexAttribArray(attrib_normal);

  assert(src_level_level1_data_size == 13 * 28);
  const uint8_t * level = (const uint8_t *)src_level_level1_data_start;
  const uint8_t * pal = (const uint8_t *)src_level_level1_data_pal_start;

  vec3 light_pos = normalize(rotate_z(theta) * vec3(1, 1, 1));

  for (int y = 0; y < 28; y++) {
    for (int x = 0; x < 13; x++) {
      char tile = level[y * 13 + x];
      if (tile == 0)
        continue;

      const float cs = 1.0f / 255.0f;
      vec3 base_color = vec3(((float)pal[tile * 3 + 0]) * cs,
                             ((float)pal[tile * 3 + 1]) * cs,
                             ((float)pal[tile * 3 + 2]) * cs);

      mat4x4 rx = rotate_x(PI / 2.0f);
      //mat4x4 p = perspective(-1, 1, 0, 1);
      mat4x4 s = scale(vec3(1.0f / 12.0f,
                            1.0f / 27.0f,
                            1.0f / 27.0f));

      float px = ((float)x / 13.0) * 2.0 - 1.0 + 1.0f / 13.0f;
      float py = ((float)y / 28.0) * -2.0 + 1.0 - 1.0f / 28.0f;

      mat4x4 t = translate(vec3(px, py, 0.0));

      mat4x4 trans = t * rx * s;

      //mat3x3 normal_trans = transpose(inverse(submatrix(trans, 0, 0)));
      mat3x3 normal_trans = submatrix(rx, 3, 3);

      glUniform4fv(uniform_trans, 4, &trans[0][0]);
      glUniform3fv(uniform_normal_trans, 3, &normal_trans[0][0]);
      glUniform3fv(uniform_base_color, 1, &base_color[0]);
      glUniform3fv(uniform_light_pos, 1, &light_pos[0]);

      glDrawElements(GL_TRIANGLES, brick_mesh.length, GL_UNSIGNED_INT, 0);
    }
  }


  //////////////////////////////////////////////////////////////////////
  // render paddle
  //////////////////////////////////////////////////////////////////////

  {
    mat4x4 rx = rotate_y(PI / 2.0f);
    mat4x4 s = scale(vec3(1.0f,
                          1.3f,
                          1.5f));
    float px = ((float)paddle_x / 13.0) * 2.0 - 1.0 + 1.0f / 13.0f;
    float py = ((float)26 / 28.0) * -2.0 + 1.0 - 1.0f / 28.0f;
    mat4x4 t = translate(vec3(px, py, 0.0));

    mat4x4 trans = t * rx * s;
    mat3x3 normal_trans = submatrix(rx, 3, 3);
    //vec3 base_color = vec3(1, 1, 1);
    vec3 base_color = vec3(1, 1, 1) * 0.5f;
    //vec3 light_pos = vec3(-1, -1, 1);

    glBindBuffer(GL_ARRAY_BUFFER, paddle_mesh.vtx);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, paddle_mesh.idx);

    glVertexAttribPointer(attrib_position,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          (sizeof (float)) * 8,
                          (void*)(0 * 4)
                          );
    /*
      glVertexAttribPointer(vertex_color_attrib_texture,
      2,
      GL_FLOAT,
      GL_FALSE,
      (sizeof (float)) * 8,
      (void*)(3 * 4)
      );
    */
    glVertexAttribPointer(attrib_normal,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          (sizeof (float)) * 8,
                          (void*)(5 * 4)
                          );
    glEnableVertexAttribArray(attrib_position);
    glEnableVertexAttribArray(attrib_normal);

    glUniform4fv(uniform_trans, 4, &trans[0][0]);
    glUniform3fv(uniform_normal_trans, 3, &normal_trans[0][0]);
    glUniform3fv(uniform_base_color, 1, &base_color[0]);
    glUniform3fv(uniform_light_pos, 1, &light_pos[0]);

    glDrawElements(GL_TRIANGLES, paddle_mesh.length, GL_UNSIGNED_INT, 0);
  }
}
