#version 110

uniform sampler2D in_Texture;
uniform bool in_TextureEnabled;

uniform vec3 in_Light;

varying vec4 var_Color;
varying vec3 var_Normal;

void main() {
  float intensity = dot(in_Light, normalize(var_Normal));

  float factor = 0.5;
  if (intensity > 0.1) factor = 1.0;
  else if (intensity < 0.0)  factor = 0.2;

  vec4 light = vec4(factor, factor, factor, 1.0);
  gl_FragColor = var_Color * light;
}
