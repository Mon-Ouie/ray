#version 110

uniform sampler2D in_Texture;
uniform bool in_TextureEnabled;

varying vec4 var_Color;

void main() {
  gl_FragColor = var_Color;
}
