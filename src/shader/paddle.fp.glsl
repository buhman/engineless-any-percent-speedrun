#version 120

uniform vec3 light_pos;
uniform float time;

varying vec3 fp_position;
varying vec2 fp_texture;
varying vec3 fp_normal;

vec3 palette(float d) {
  vec3 v = d + vec3(0, 0.33, 0.67);
  v = v * 6.28318548202514648438;
  v = cos(v);
  return vec3(0.5, 0.5, 0.5) * v + vec3(0.5, 0.5, 0.5);
}

void main()
{
  vec3 light_dir = normalize(light_pos - fp_position);
  float diffuse = max(dot(fp_normal, light_dir), 0.0);

  vec2 uv = fp_texture;
  float td = uv.x + -time;

  float d = sin(dot(uv, uv) * 8.0 + time * 6) / 8.0;
  d = abs(d);
  d = 0.05 / d;

  float dy = 0;
  /*
  float dy = sin(uv.y * 8.0 + time * 6) / 8.0;
  dy = abs(dy);
  dy = 0.05 / dy;
  */

  vec3 colorA = palette(td) * (d + dy);
  vec3 colorB = palette(td);

  gl_FragColor = vec4(mix(colorA, colorB, 0.5) * (diffuse), 1);
  //gl_FragColor = vec4(abs(fp_texture.xy), 0, 1);
}
