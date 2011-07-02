#version 110

attribute vec3 in_Position;
attribute vec4 in_Color;

uniform mat4 in_ModelView;
uniform mat4 in_Projection;

varying vec4 var_Color;

void main() {
  gl_Position = vec4(in_Position, 1) * (in_ModelView * in_Projection);
  var_Color   = in_Color;
}
