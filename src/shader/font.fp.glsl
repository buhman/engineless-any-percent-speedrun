#version 120

uniform sampler2D texture0;

varying vec2 fp_texture;

void main()
{
  vec4 c = texture2D(texture0, fp_texture);

  float i = c.x == 0 ? 0.0 : 1.0;

  gl_FragColor = vec4(i, i, i, 1.0);
  //gl_FragColor = vec4(fp_texture.xy, 1, 1.0);
}
