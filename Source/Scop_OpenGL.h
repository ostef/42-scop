#pragma once

#define SCOP_BACKEND_NAME "OpenGL 3.3"

#include <glad/glad.h>

struct GfxMeshObjects
{
    GLuint vao;
    union
    {
        struct
        {
            GLuint vbo, ibo;
        };
        GLuint buffers[2];
    };
};

enum GLVertexAttribIndex
{
    GL_Attrib_Position,
    GL_Attrib_Normal,
    GL_Attrib_Tex_Coords,
};

typedef GLuint GfxTexture;
