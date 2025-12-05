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

bool aabb_circle_collision(vec3 aabb_position, vec3 circle_position)
{
  const vec3 bounds = vec3(2, 1, 0);

  vec3 distance = circle_position - aabb_position;
  vec3 closest_point_pd = clamp(distance, -bounds, bounds);
  vec3 closest_point_p = aabb_position + closest_point_pd;

  bool collided = fabsf(magnitude(closest_point_p - circle_position)) < 1;
  if (!collided)
    return collided;

  return collided;
}
