#version 330 core

in vec3 Vertex_Position;
in vec3 Normal;
in vec2 Tex_Coords;

out vec4 Frag_Color;

uniform vec3 u_Light_Position;
uniform vec3 u_Light_Color;
uniform bool u_Has_Texture;
uniform sampler2D u_Texture;

void main ()
{
    vec3 normal = Normal * (float (gl_FrontFacing) * 2 - 1);
    normal = normalize (normal);

    vec3 vertex_to_light = normalize (u_Light_Position - Vertex_Position);
    float diffuse_factor = max (dot (vertex_to_light, normal), 0.1);

    vec3 diffuse;
    if (u_Has_Texture)
        diffuse = texture (u_Texture, Tex_Coords).rgb;
    else
        diffuse = vec3 (1,1,1);

    Frag_Color = vec4 (diffuse * u_Light_Color * diffuse_factor, 1);
}
