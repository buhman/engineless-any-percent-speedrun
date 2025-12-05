#version 120

varying vec2 fp_uv;

uniform float time;

vec3 palette(float d) {
  vec3 v = d + vec3(0.25, 0.40625, 0.5625); // 40 45 49
  v = v * 6.28318548202514648438;
  v = cos(v);
  return vec3(0.5, 0.5, 0.5) * v + vec3(0.5, 0.5, 0.5); // 48
}

void main()
{
  vec2 uv = fp_uv;
  float d = length(uv);
  float td = time * 0.5 + d;

  gl_FragColor = vec4(palette(td) * 0.2, 1);
}
