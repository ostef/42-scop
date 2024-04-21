#ifndef SCOP_GRAPHICS_H
#define SCOP_GRAPHICS_H

#include "Scop_Math.h"

#ifdef SCOP_BACKEND_OPENGL
    #include "Scop_OpenGL.h"
#elif defined (SCOP_BACKEND_VULKAN)
    #include "Scop_Vulkan.h"

    #define GLFW_INCLUDE_VULKAN
#else
    #error No graphics backend is defined. #define either SCOP_BACKEND_OPENGL or SCOP_BACKEND_VULKAN

    #define SCOP_BACKEND_NAME "None"
#endif

#include <GLFW/glfw3.h>

struct Camera
{
    Vec3f position;
    Vec3f target;
    Vec2f yaw_pitch;

    Vec2f offset;
    float distance_from_target;

    Mat4f view_matrix;
    Mat4f projection_matrix;
    Mat4f view_projection_matrix;
};

extern GLFWwindow *g_main_window;
extern Camera g_camera;

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
    GfxMeshObjects gfx_objects;
};

void GLFWErrorCallback (int code, const char *description);

bool LoadMeshFromObjFile (const char *filename, Mesh *mesh);

bool GfxInitBackend ();
void GfxTerminateBackend ();
void GfxCreateMeshObjects (Mesh *mesh);
void GfxRenderFrame (Mesh *mesh);

#endif
