#version 120

uniform sampler2D texture0;

uniform vec3 base_color;
uniform float time;

varying vec2 fp_texture;
varying vec2 fp_position;

void main()
{
  vec4 c = texture2D(texture0, fp_texture);

  if (c.x == 0)
    discard;

  float i = c.x == 0 ? 0.0 : 1.0;

  vec2 uv = fp_position;
  /*
  float d = sin(uv.x * 0.1 + time * 0.1) / 0.1;
  d = abs(d);
  d = 0.01 / d;
  */

  float red_alpha = (1.0 - length(uv) + sin(time * 3) * abs(sin(time + uv.x * 2 + uv.y * 2)));

  float alpha = (base_color.x == 1.0 && base_color.y == 1.0 && base_color.z == 1.0) ? 1.0 : red_alpha;
  gl_FragColor = vec4(base_color * (i + alpha), 1.0);
  //gl_FragColor = vec4(d, d, d, 1);
  //gl_FragColor = vec4(fp_position.xy, 0, 1.0);
}
