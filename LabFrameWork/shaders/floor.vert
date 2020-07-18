#version 400

layout (location = 0) in vec4 VertexPosition;
layout (location = 1) in vec3 VertexColor;

uniform mat4 MVP;
out vec3 f_color;

void main(void)
{

  gl_Position = MVP * VertexPosition;
  f_color = VertexColor;

}
