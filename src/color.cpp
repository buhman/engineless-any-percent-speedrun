#include "color.hpp"

vec3 hsv_to_rgb(float h, float s, float v)
{
  if (s == 0.0)
    return vec3(v, v, v);
  int i = h * 6.0f;
  float f = (h * 6.0f) - i;
  float p = v * (1.0f - s);
  float q = v * (1.0f - s * f);
  float t = v * (1.0f - s * (1.0f - f));
  i = i % 6;
  if (i == 0)
    return vec3(v, t, p);
  if (i == 1)
    return vec3(q, v, p);
  if (i == 2)
    return vec3(p, v, t);
  if (i == 3)
    return vec3(p, q, v);
  if (i == 4)
    return vec3(t, p, v);
  if (i == 5)
    return vec3(v, p, q);
  return vec3(0, 0, 0);
}
