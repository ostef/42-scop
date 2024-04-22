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

#include "stb_image.h"

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
    Vec4f tangent;
    Vec2f tex_coords;
};

struct Mesh
{
    Vertex *vertices;
    s64 vertex_count;
    u32 *indices;
    s64 index_count;
    Vec3f aabb_min;
    Vec3f aabb_max;
    GfxMeshObjects gfx_objects;
};

struct WeldMeshResult
{
    Vertex *unique_vertices;
    s64 unique_vertex_count;
    u32 *indices;
    s64 index_count;
};

WeldMeshResult WeldMesh (Vertex *vertices, u32 vertex_count);

void CalculateTangents (Vertex *vertices, s64 vertex_count, u32 *indices, s64 index_count);
void CalculateNormalsFlat (Vertex *vertices, s64 vertex_count);
void CalculateNormalsSmooth (Vertex *vertices, s64 vertex_count, u32 *indices, s64 index_count);
void CalculateBoundingBox (Mesh *mesh);
void CalculateBasicTexCoords (Mesh *mesh);

enum LoadMeshFlags
{
    LoadMesh_NoFlags = 0x00,
    LoadMesh_WeldMesh = 0x01,
    LoadMesh_CalculateNormalsSmooth = 0x02,
    LoadMesh_CalculateNormalsFlat = 0x04,
    LoadMesh_IgnoreSuppliedNormals = 0x08,
    LoadMesh_CalculateTangents = 0x10,
    LoadMesh_CalculateTexCoords = 0x20,

    LoadMesh_DefaultFlags = LoadMesh_WeldMesh
        | LoadMesh_CalculateNormalsSmooth
        | LoadMesh_CalculateTangents
        | LoadMesh_CalculateTexCoords,
};

bool LoadMeshFromObjFile (const char *filename, Mesh *mesh, LoadMeshFlags flags = LoadMesh_DefaultFlags);

bool LoadTextureFromFile (const char *filename, GfxTexture *texture, u32 *width, u32 *height);

void GLFWErrorCallback (int code, const char *description);

bool GfxInitBackend ();
void GfxTerminateBackend ();
void GfxCreateMeshObjects (Mesh *mesh);
GfxTexture GfxCreateTexture (void *data, u32 width, u32 height);
void GfxRenderFrame (Mesh *mesh, GfxTexture texture, const Vec3f &light_position);

#endif
