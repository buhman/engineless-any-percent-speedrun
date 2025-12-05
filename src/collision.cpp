#include <math.h>
#include <stdio.h>

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
    t0 = clamp(t0, -1.0, 1.0);
    t1 = clamp(t0, -1.0, 1.0);
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

  return (z < 0) && !((x < 0) || (y < 0));
}

struct ct {
  bool collision;
  float t;
};

#define NONE -99999999.0f

ct lowest_root(float a,
               float b,
               float c,
               float max_r)
{
  float determinant = b * b - 4.0f * a * c;
  if (determinant < 0.0f)
    return ct(false, max_r);

  float sqrt_det = sqrtf(determinant);
  float r_2a = 1.0f / (2.0f * a);
  float r1 = (-b - sqrt_det) * r_2a;
  float r2 = (-b + sqrt_det) * r_2a;
  if (r1 > r2) {
    float tmp = r1;
    r1 = r2;
    r2 = tmp;
  }
  if (r1 > 0 && (max_r == NONE || r1 < max_r))
    return ct(true, r1);
  if (r2 > 0 && (max_r == NONE || r2 < max_r))
    return ct(true, r2);
  return ct(false, max_r);
}

ct triangle_point_collide(vec3 tri_point,
                          vec3 point,
                          vec3 velocity,
                          float t)
{
  float a = dot(velocity, velocity);
  float b = 2.0f * dot(velocity, point - tri_point);
  vec3 c0 = (tri_point - point);
  float c = dot(c0, c0) - 1.0f;
  return lowest_root(a, b, c, t);
}

struct ctf0 {
  bool collision;
  float t;
  float f0;
};

ctf0 triangle_edge_collide(vec3 tri_point1,
                           vec3 tri_point2,
                           vec3 point,
                           vec3 velocity,
                           float t)
{
  vec3 edge = tri_point2 - tri_point1;
  vec3 base = tri_point1 - point;

  float edge_dot_edge = dot(edge, edge);
  float edge_dot_velocity = dot(edge, velocity);
  float edge_dot_base = dot(edge, base);
  float velocity_dot_velocity = dot(velocity, velocity);

  float a = edge_dot_edge * -velocity_dot_velocity + (edge_dot_velocity * edge_dot_velocity);
  float b = edge_dot_edge * 2.0f * (dot(velocity, base)) - 2.0f * edge_dot_velocity * edge_dot_base;
  float c = edge_dot_edge * (1.0f - dot(base, base)) + (edge_dot_base * edge_dot_base);

  ct res = lowest_root(a, b, c, t);
  float f0 = NONE;
  if (res.collision) {
    f0 = (edge_dot_velocity * res.t - edge_dot_base) * (1.0f / edge_dot_edge);
  }
  return ctf0(res.collision, t, f0);
}

struct t0t1pip {
  float t0;
  float t1;
  vec3 pip;
};

t0t1pip collision_inner(vec3 a, vec3 b, vec3 c,
                        vec3 point,
                        vec3 velocity)
{
  vec3 plane_normal = _plane_normal(a, b, c);
  vec3 plane_origin = a;
  float plane_d = _plane_d(plane_normal, plane_origin);
  float signed_distance = _signed_distance(plane_normal, plane_d, point);

  float n_dot_v = dot(plane_normal, velocity);
  float r_n_dot_v = 1.0f / n_dot_v;

  bool embedded = n_dot_v == 0.0f;
  if (n_dot_v == 0.0f) {
    if (fabsf(signed_distance) >= 1.0)
      return t0t1pip(NONE, NONE, vec3(0, 0, 0));
  }
  vec2 t0t1 = _t0_t1(signed_distance, r_n_dot_v);
  float t0 = t0t1.x;
  float t1 = t0t1.y;

  vec3 plane_intersection_point = point - plane_normal + t0 * velocity;

  bool inside = point_in_triangle(plane_intersection_point, a, b, c);

  bool collision = false;
  vec3 collision_point;
  float t = NONE;
  vec3 abc[] = {a, b, c};
  for (int i = 0; i < 3; i++) {
    vec3 tri_point = abc[i];
    ct res = triangle_point_collide(tri_point, point, velocity, t);
    t = res.t;
    if (res.collision) {
      collision_point = tri_point;
      collision = true;
    }
  }

  for (int i = 0; i < 3; i++) {
    vec3 p1 = abc[i];
    vec3 p2 = abc[(i + 1) % 3];
    ctf0 res = triangle_edge_collide(p1, p2, point, velocity, t);
    if (res.collision && res.f0 >= 0.0 && res.f0 <= 1.0) {
      t = res.t;
      collision_point = p1 + res.f0 * (p2 - p1);
      collision = true;
    }
  }

  if (inside && collision) {
    printf("r_n_dot_v %f %f %f %f\n", n_dot_v, r_n_dot_v, t0, t1);

    return t0t1pip(t0, t1, plane_intersection_point);
  }
  else
    return t0t1pip(NONE, NONE, vec3(0, 0, 0));
}

vec4 collision(mat4x4 trans,
               vec3 point, vec3 velocity)
{
  bool collision = false;
  float smallest = -9999.0f;
  vec3 smallest_normal = vec3(0, 0, 0);
  //printf("velocity %f %f %f\n", velocity.x, velocity.y, velocity.z);

  for (int i = 0; i < cube_Cube_triangles_length / 3; i++) {
    const float * a = &cube_vertices[cube_Cube_triangles[i * 3 + 0] * 8];
    const float * b = &cube_vertices[cube_Cube_triangles[i * 3 + 1] * 8];
    const float * c = &cube_vertices[cube_Cube_triangles[i * 3 + 2] * 8];

    vec3 ap = trans * vec3(a[0], a[1], a[2]);
    vec3 bp = trans * vec3(b[0], b[1], b[2]);
    vec3 cp = trans * vec3(c[0], c[1], c[2]);

    t0t1pip res = collision_inner(ap, bp, cp, point, velocity);
    bool inside = res.t0 != NONE;
    if (!inside)
      continue;

    if (collision == false || res.t0 < smallest) {
      printf("new smallest %f %f\n", smallest, res.t0);
      smallest = res.t0;

      vec3 plane_normal = _plane_normal(ap, bp, cp);
      smallest_normal = plane_normal;
    }
    collision = true;
  }

  //float collision_f = collision ? 1.0 : 0.0;

  return vec4(smallest_normal.x,
              smallest_normal.y,
              smallest_normal.z,
              collision ? smallest : -9999.0f);
}
