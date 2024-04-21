#version 330 core

in vec3 Vertex_Position;
in vec3 Normal;

out vec4 Frag_Color;

void main ()
{
    vec3 normal = Normal * float (gl_FrontFacing) * 2 - 1;

    vec3 light_position = vec3(-100, 100, -100);
    vec3 vertex_to_light = normalize (light_position - Vertex_Position);
    float diffuse_factor = max (dot (vertex_to_light, normal), 0.1);

    vec3 diffuse_color = vec3(1,1,1) * diffuse_factor;

    Frag_Color = vec4 (diffuse_color,1);
}
