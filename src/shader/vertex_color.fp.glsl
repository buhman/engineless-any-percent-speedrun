#version 120

uniform vec4 base_color;
uniform vec3 light_pos;

varying vec3 fp_position;
varying vec3 fp_normal;
varying vec2 fp_texture;

void main()
{
  //vec3 color_normal = fp_normal * 0.5 + 0.5;
  //vec3 light_pos = vec3(1, 1, 1);

  vec3 light_dir = normalize(light_pos - fp_position);
  float diffuse = max(dot(fp_normal, light_dir), 0.0);

  vec3 color = (diffuse + 0.5) * base_color.xyz;

  gl_FragColor = vec4(color, base_color.w);
  //gl_FragColor = vec4(fp_normal * 0.5 + 0.5, 1.0);
  //gl_FragColor = vec4(fp_texture, 0.0, 1.0);
}
// normal
// x (0.0 left 1.0 right)
// y (0.0 bot 1.0 top)
// z (0.0 far 1.0 near)
