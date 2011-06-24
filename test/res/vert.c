#version 110

attribute vec2 in_Vertex;
attribute vec4 in_Color;
attribute vec2 in_TexCoord;

uniform mat4 in_ModelView;
uniform mat4 in_Projection;

varying vec4 var_Color;
varying vec2 var_TexCoord;

void main() {
  gl_Position  = vec4(in_Vertex, 0, 1) * (in_ModelView * in_Projection);
  var_Color    = in_Color;
  var_TexCoord = in_TexCoord;
}
