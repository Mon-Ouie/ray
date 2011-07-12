#version 110

uniform sampler2D in_Texture;
uniform bool in_TextureEnabled;

uniform vec3 in_Light;

varying vec4 var_Color;
varying vec3 var_Normal;

void main() {
  float intensity = dot(in_Light, normalize(var_Normal));

  float factor = 0.6;
  if      (intensity > 1.0)  factor = 1.0;
  else if (intensity > 0.7)  factor = 0.8;
  else if (intensity > 0.25) factor = 0.4;
  else if (intensity < 0.0)  factor = 0.1;

  vec4 light = vec4(factor, factor, factor, 1.0);
  gl_FragColor = var_Color * light;
}
