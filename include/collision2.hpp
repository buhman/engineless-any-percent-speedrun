#pragma once

#include "math/float_types.hpp"

struct collision_data {
  vec3 escape_position;
  vec3 bounds_normal;
};

bool aabb_circle_collision(vec3 aabb_position, vec3 circle_position, vec3 bounds, struct collision_data * data);
