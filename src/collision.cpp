#include <math.h>

#include "collision.hpp"

#include "model/cube.h"

// cube_vertices
// cube_Cube_triangles

vec3 _plane_normal(vec3 a,
                   vec3 b,
                   vec3 c)
{
  return normalize(cross(b - a, c - a));
}

float _plane_d(vec3 normal,
               vec3 origin)
{
  return -dot(normal, origin);
}

float _signed_distance(vec3 plane_normal,
                       float plane_d,
                       vec3 point)
{
  return dot(point, plane_normal) + plane_d;
}

float clamp(float v,
            float min,
            float max)
{
  if (v > max)
    return max;
  if (v < min)
    return min;
  return v;
}

vec2 _t0_t1(float signed_distance,
            float r_n_dot_v)
{
  if (r_n_dot_v == 0.0)
    return vec2(0.0, 1.0);
  else {
    float t0 = (-1.0 - signed_distance) * r_n_dot_v;
    float t1 = ( 1.0 - signed_distance) * r_n_dot_v;
    if (t0 > t1) {
      float tmp = t0;
      t0 = t1;
      t1 = tmp;
    }
    t0 = clamp(t0, 0.0, 1.0);
    t1 = clamp(t0, 0.0, 1.0);
    return vec2(t0, t1);
  }
}

bool point_in_triangle(vec3 point,
                       vec3 ap,
                       vec3 bp,
                       vec3 cp)
{
  vec3 ba = bp - ap;
  vec3 ca = cp - ap;

  vec3 vp = point - ap;

  float a = dot(ba, ba);
  float b = dot(ba, ca);
  float c = dot(ca, ca);
  float d = dot(vp, ba);
  float e = dot(vp, ca);
  float x = (d * c) - (e * b);
  float y = (e * a) - (d * b);
  float z = x + y - ((a * c) - (b * b));

  return (z < 0) and not ((x < 0) or (y < 0));
}

vec2 collision_inner(vec3 plane_normal,
                     vec3 a, vec3 b, vec3 c,
                     vec3 point,
                     vec3 velocity)
{
  vec3 plane_origin = a;
  float plane_d = _plane_d(plane_normal, plane_origin);
  float signed_distance = _signed_distance(plane_normal, plane_d, point);

  float n_dot_v = dot(plane_normal, velocity);
  float r_n_dot_v = 1.0f / n_dot_v;

  //bool embedded = n_dot_v == 0.0f;
  if (n_dot_v == 0.0f) {
    if (fabsf(signed_distance) >= 1.0f)
      return false;
  }

  vec2 t0_t1 = _t0_t1(signed_distance, r_n_dot_v);
  float t0 = t0_t1.x;
  //float t1 = t0_t1.y;

  vec3 plane_intersection_point = point - plane_normal + t0 * velocity;
  bool inside = point_in_triangle(plane_intersection_point, a, b, c);
  float inside_f = inside ? 1.0f : 0.0f;

  return vec2(inside_f, t0);
}

vec4 collision(mat4x4 trans,
               vec3 point, vec3 velocity)
{
  bool collision = false;
  float smallest = 0.0f;
  vec3 smallest_normal = vec3(0, 0, 0);

  for (int i = 0; i < cube_Cube_triangles_length / 3; i++) {
    const float * a = &cube_vertices[cube_Cube_triangles[i * 3 + 0] * 8];
    const float * b = &cube_vertices[cube_Cube_triangles[i * 3 + 1] * 8];
    const float * c = &cube_vertices[cube_Cube_triangles[i * 3 + 2] * 8];

    vec3 ap = trans * vec3(a[0], a[1], a[2]);
    vec3 bp = trans * vec3(b[0], b[1], b[2]);
    vec3 cp = trans * vec3(c[0], c[1], c[2]);

    vec3 plane_normal = _plane_normal(ap, bp, cp);

    vec2 res = collision_inner(plane_normal, ap, bp, cp, point, velocity);
    bool inside = res.x == 1.0f;
    float t0 = res.y;
    if (!inside)
      continue;

    if (collision == false || t0 < smallest) {
      smallest = t0;
      smallest_normal = plane_normal;
    }
    collision = true;
  }

  float collision_f = collision ? 1.0 : 0.0;

  return vec4(smallest_normal.x,
              smallest_normal.y,
              smallest_normal.z,
              collision_f);
}

/*
bool collision(float ball_x, float ball_y,
               float ball_dx, float ball_dy)
{
  const uint8_t * level = (const uint8_t *)src_level_level1_data_start;
  const uint8_t * pal = (const uint8_t *)src_level_level1_data_pal_start;

  for (int y = 0; y < 28; y++) {
    for (int x = 0; x < 13; x++) {
      char tile = level[y * 13 + x];
      if (tile == 0)
        continue;

        //if (   (paddle_x + 1.0) >= ((float)x - 1.0)
          //&& (paddle_x - 1.0) <= ((float)x + 1.0)
          //&& (paddle_y + 1.0) >= ((float)y - 1.0)
          //&& (paddle_y - 1.0) <= ((float)y + 1.0)) {

      collision_inner(translate(vec3(x, y, 0)),
                      vec3(ball_x,  ball_y,  0),
                      vec3(ball_dx, ball_dy, 0));
    }
  }
}
*/
