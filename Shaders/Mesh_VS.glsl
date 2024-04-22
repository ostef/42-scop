#version 330 core

layout (location = 0) in vec3 v_Position;
layout (location = 1) in vec3 v_Normal;
layout (location = 2) in vec2 v_Tex_Coords;

uniform mat4 u_View_Projection_Matrix;
uniform mat4 u_Model_Matrix;

out vec3 Vertex_Position;
out vec3 Normal;
out vec2 Tex_Coords;

void main ()
{
    gl_Position = u_View_Projection_Matrix * u_Model_Matrix * vec4 (v_Position, 1);

    Vertex_Position = v_Position;
    Normal = v_Normal;
    Tex_Coords = v_Tex_Coords;
}
