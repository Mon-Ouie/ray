#version 110

attribute vec3 in_Position;
attribute vec3 in_Normal;
attribute vec4 in_Color;

uniform mat4 in_ModelView;
uniform mat4 in_Projection;

uniform vec3 in_Light;
uniform vec4 in_LightColor;

varying vec4 var_Color;

void main() {
  gl_Position = vec4(in_Position, 1) * in_ModelView * in_Projection;

  vec3 light  = normalize(in_Light - gl_Position.xyz);
  vec3 normal = normalize(in_Normal);

  float diff = dot(normal, light);

  var_Color = vec4(in_Color.rgb + in_LightColor.rgb * diff, 1.0);
}
