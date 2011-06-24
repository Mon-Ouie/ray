#version 110

uniform sampler2D in_Texture;
uniform bool in_TextureEnabled;

varying vec4 var_Color;
varying vec2 var_TexCoord;

void main() {
  if (in_TextureEnabled)
    gl_FragColor = texture2D(in_Texture, var_TexCoord) * var_Color;
  else
    gl_FragColor = var_Color;
}
