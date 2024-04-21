#version 330 core

layout (location = 0) in vec3 v_Position;
layout (location = 1) in vec3 v_Normal;
layout (location = 2) in vec2 v_Tex_Coords;

uniform mat4 u_View_Projection_Matrix;

void main ()
{
    gl_Position = u_View_Projection_Matrix * vec4 (v_Position, 1);
}
