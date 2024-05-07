#version 330 core

in vec3 Vertex_Position;
in vec3 Normal;
in vec2 Tex_Coords;

out vec4 Frag_Color;

uniform vec3 u_Light_Position;
uniform vec3 u_Light_Color;
uniform vec3 u_Model_Color;
uniform float u_Texture_Alpha;
uniform sampler2D u_Texture;

float Random (float seed)
{
    return fract (sin (seed * 91.3458) * 47453.5453);
}

void main ()
{
    vec3 normal = Normal * (float (gl_FrontFacing) * 2 - 1);
    normal = normalize (normal);

    vec3 vertex_to_light = normalize (u_Light_Position - Vertex_Position);
    float diffuse_factor = max (dot (vertex_to_light, normal), 0.1);

    vec3 texture_color = vec3 (0);
    if (u_Texture_Alpha != 0)
        texture_color = texture (u_Texture, Tex_Coords).rgb;

    vec3 random_color;
    random_color.r = Random (gl_PrimitiveID);
    random_color.g = Random (random_color.r);
    random_color.b = Random (random_color.g);

    vec3 diffuse = mix (random_color, texture_color, u_Texture_Alpha);

    Frag_Color = vec4 (diffuse * u_Light_Color * diffuse_factor, 1);
}
