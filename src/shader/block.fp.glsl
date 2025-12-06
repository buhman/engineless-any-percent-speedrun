#version 120

uniform vec4 base_color;
uniform vec3 light_pos;

varying vec3 fp_position;
varying vec2 fp_texture;
varying vec3 fp_normal;

void main()
{
  vec3 light_dir = normalize(light_pos - fp_position);
  float diffuse = max(dot(fp_normal, light_dir), 0.0);

  vec3 color = (diffuse + 0.5) * base_color.xyz;

  gl_FragColor = vec4(color, base_color.w);
}
