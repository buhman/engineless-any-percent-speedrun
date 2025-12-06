#version 120

uniform vec4 base_color;
uniform vec3 light_pos;

uniform float time;

varying vec3 fp_position;
varying vec2 fp_texture;
varying vec3 fp_normal;

void main()
{
  vec3 light_dir = normalize(light_pos - fp_position);
  float diffuse = max(dot(fp_normal, light_dir), 0.0);

  float g = fp_position.x + fp_position.y;
  float gt = g * 8.0 + time * 10;
  float d = sin(gt) / 8.0;
  d = abs(d);
  d = 0.05 / d;

  float sgt = sin(gt / 10);
  if (sgt < 0.9)
    d = 0;
  else
    d = (sgt - 0.9) * 10;

  //d = d * sin(time);
  d = pow(d, 32);

  vec3 color = (diffuse + 0.5) * (base_color.xyz + max(d * 0.5, 0));
  gl_FragColor = vec4(color, base_color.w);

  //gl_FragColor = vec4(d, 0, 0, base_color.w);
}
