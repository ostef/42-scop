#ifndef SCOP_GRAPHICS_H
#define SCOP_GRAPHICS_H

#include "Scop_Math.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct Vertex
{
    Vec3f position;
    Vec3f normal;
    Vec2f tex_coords;
};

struct Mesh
{
    Vertex *vertices;
    s64 vertex_count;
    u32 *indices;
    s64 index_count;
};

bool LoadMeshFromObjFile (const char *filename, Mesh *mesh);

bool GfxInitVulkan (GLFWwindow *window);

#endif
