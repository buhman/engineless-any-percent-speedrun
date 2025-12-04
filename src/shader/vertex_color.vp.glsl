#version 120

attribute vec3 position;
attribute vec3 normal;

uniform vec4 trans[4];
uniform vec3 normal_trans[3];

varying vec3 fp_normal;
varying vec3 fp_position;

vec4 transform4(vec4 v)
{
  return vec4(dot(trans[0], v),
              dot(trans[1], v),
              dot(trans[2], v),
              dot(trans[3], v));
}

vec3 transform3(vec3 v)
{
  return vec3(dot(normal_trans[0], v),
              dot(normal_trans[1], v),
              dot(normal_trans[2], v));
}

void main()
{
  vec4 pos = transform4(vec4(position, 1));

  fp_normal = normalize(transform3(normal));
  fp_position = pos.xyz;

  gl_Position = pos;
}
