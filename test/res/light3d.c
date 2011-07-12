#version 110

attribute vec3 in_Position;
attribute vec3 in_Normal;
attribute vec4 in_Color;

uniform mat4 in_ModelView;
uniform mat4 in_Projection;

varying vec4 var_Color;
varying vec3 var_Normal;

void main() {
  mat4 mvp = in_ModelView * in_Projection;
  gl_Position = vec4(in_Position, 1) * mvp;

  var_Color  = in_Color;
  /* Ignoring projection, willingly */
  var_Normal = (vec4(in_Normal, 1) * in_ModelView).xyz;
}
