#version 120

attribute vec3 position;
attribute vec2 _texture;
attribute vec3 normal;

varying vec2 fp_texture;

uniform vec4 trans[4];

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

  fp_texture = _texture;

  gl_Position = pos;
}
