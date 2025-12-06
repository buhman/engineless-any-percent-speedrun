#version 120

uniform sampler2D texture1;

varying vec2 fp_uv;

uniform float time;

uniform vec3 palette;

uniform float aspect;

vec3 sample_palette(float d, vec3 p) {
  vec3 v = d + p;
  v = v * 6.28318548202514648438;
  v = cos(v);
  return vec3(0.5, 0.5, 0.5) * v + vec3(0.5, 0.5, 0.5); // 48
}

void main()
{
  vec2 tc = vec2(fp_uv.x * aspect, fp_uv.y) * 0.5 + 0.5;
  vec4 c = texture2D(texture1, tc);

  vec2 bc = fp_uv * 0.5 + 0.5;

  if (bc.x > 0.95 || bc.x < 0.05 ||
      bc.y > 1.0 || bc.y < 0.0) {
    vec2 uv = fp_uv;
    float d = length(uv);
    float st = sin(time * 0.05);
    float td = time * st * 0.5 + d;

    vec3 cc = mix(c.xyz, c.zyx, sin(time) * 0.5 + 0.5);
    gl_FragColor = vec4(sample_palette(td, palette) * cc, 1);
  } else {
    float noise_0 = mix(c.x, c.y, sin(time) * 0.5 + 0.5);
    float noise_1 = mix(c.z, c.w, cos(time) * 0.5 + 0.5);

    vec2 uv = vec2(sin(fp_uv.x + time + noise_0),
                   cos(fp_uv.y + time + noise_1));
    float d = length(uv);
    float td = time * 0.5 + d;

    gl_FragColor = vec4(sample_palette(td, palette) * 0.1, 1);
  }

  //gl_FragColor = vec4(noise * noise);
}
