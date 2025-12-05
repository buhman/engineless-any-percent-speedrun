#include <math.h>

#include "collision2.hpp"

static inline float clamp(float n, float min, float max)
{
  if (n < min)
    return min;
  if (n > max)
    return max;
  return n;
}

static inline vec3 clamp(vec3 v, vec3 min_v, vec3 max_v)
{
  return vec3(clamp(v.x, min_v.x, max_v.x),
              clamp(v.y, min_v.y, max_v.y),
              clamp(v.z, min_v.z, max_v.z));
}

static inline float rcp(float f)
{
  if (f == 0.0f)
    return 0.0f;
  else
    return 1.0f / f;
}

static inline vec3 rcp(vec3 v)
{
  return vec3(rcp(v.x),
              rcp(v.y),
              rcp(v.z));
}

static inline vec3 bounds_normal(vec3 bounds, vec3 point, vec3 cdistance0)
{
  vec3 normal_space = rcp(bounds);

  vec3 n = normal_space * point;

  if (fabsf(n.x) > fabsf(n.y))
    return vec3(1 * fabsf(n.x) / n.x, 0, 0);
  else if (fabsf(n.y) > fabsf(n.x))
    return vec3(0, 1 * fabsf(n.y) / n.y, 0);
  else {
    if (fabsf(cdistance0.x) > fabsf(cdistance0.y))
      return vec3(1 * -fabsf(cdistance0.x) / cdistance0.x, 0, 0);
    else if (fabsf(cdistance0.y) > fabsf(cdistance0.x))
      return vec3(0, 1 * -fabsf(cdistance0.y) / cdistance0.y, 0);
    else
      return normalize(vec3(1, 1, 0));
  }
}

bool aabb_circle_collision(vec3 aabb_position, vec3 circle_position, vec3 bounds, struct collision_data * data)
{
  vec3 distance = circle_position - aabb_position;
  vec3 closest_point_pd = clamp(distance, -bounds, bounds);
  vec3 closest_point_p = aabb_position + closest_point_pd;

  bool collided = fabsf(magnitude(closest_point_p - circle_position)) < 1;
  if (!collided)
    return collided;

  vec3 cdistance0 = closest_point_p - circle_position;
  vec3 cdistance = normalize(cdistance0);
  vec3 closest_point_c = circle_position + cdistance;
  vec3 penetration_dir = closest_point_p - closest_point_c;
  vec3 penetration_vec = normalize(penetration_dir);

  vec3 escape_position = circle_position + penetration_vec * magnitude(penetration_dir);

  data->escape_position = escape_position;
  data->bounds_normal = bounds_normal(bounds, closest_point_pd, cdistance0);

  return collided;
}
