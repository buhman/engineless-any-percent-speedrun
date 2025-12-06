#version 120

attribute vec3 position;
attribute vec2 _texture;

varying vec2 fp_texture;
varying vec2 fp_position;

uniform vec4 trans[4];
uniform vec4 texture_trans[4];

vec4 transform4(vec4 v)
{
  return vec4(dot(trans[0], v),
              dot(trans[1], v),
              dot(trans[2], v),
              dot(trans[3], v));
}

vec4 transform4t(vec4 v)
{
  return vec4(dot(texture_trans[0], v),
              dot(texture_trans[1], v),
              dot(texture_trans[2], v),
              dot(texture_trans[3], v));
}

void main()
{
  vec4 pos = transform4(vec4(position, 1));
  vec4 tt = transform4t(vec4(_texture, 0, 1));

  fp_texture = tt.xy;
  fp_position = position.xz;

  gl_Position = pos;
}
