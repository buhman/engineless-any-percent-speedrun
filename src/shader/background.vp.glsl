#version 120

attribute vec3 position;

uniform vec4 trans[4];
uniform vec2 resolution;

varying vec2 fp_uv;

vec4 transform4(vec4 v)
{
  return vec4(dot(trans[0], v),
              dot(trans[1], v),
              dot(trans[2], v),
              dot(trans[3], v));
}

void main()
{
  vec4 pos = transform4(vec4(position, 1));

  fp_uv = vec2(pos.x * resolution.x / resolution.y, pos.y);

  gl_Position = pos;
}
