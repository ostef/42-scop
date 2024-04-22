#include "Scop_Core.h"
#include "Scop_Graphics.h"
#include "Scop_Math.h"

WeldMeshResult WeldMesh (Vertex *vertices, u32 vertex_count)
{
    u32 *remap_table = (u32 *)malloc (sizeof (u32) * vertex_count);
    if (!remap_table)
        return {};

    for (u32 i = 0; i < vertex_count; i += 1)
        remap_table[i] = i;

    s64 unique_vertex_count = vertex_count;
    for (int i = 0; i < vertex_count; i += 1)
    {
        // If already remapped
        if (remap_table[i] != i)
            continue;

        for (int j = i + 1; j < vertex_count; j += 1)
        {
            if (vertices[i].position == vertices[j].position
            && vertices[i].normal == vertices[j].normal
            && vertices[i].tex_coords == vertices[j].tex_coords)
            {
                remap_table[j] = i;
                unique_vertex_count -= 1;
            }
        }
    }

    WeldMeshResult result = {};
    result.unique_vertices = (Vertex *)malloc (sizeof (Vertex) * unique_vertex_count);
    if (!result.unique_vertices)
    {
        free (remap_table);
        return {};
    }

    result.unique_vertex_count = unique_vertex_count;

    result.indices = remap_table;
    result.index_count = vertex_count;

    s64 vertex_index = 0;
    for (int i = 0; i < vertex_count; i += 1)
    {
        if (result.indices[i] == i)
        {
            result.unique_vertices[vertex_index] = vertices[i];
            result.indices[i] = vertex_index;

            vertex_index += 1;
        }
        else
        {
            result.indices[i] = result.indices[result.indices[i]];
        }
    }

    Assert (vertex_index == result.unique_vertex_count);

    return result;
}

void CalculateNormalsFlat (Vertex *vertices, s64 vertex_count)
{
    Assert (vertex_count % 3 == 0, "Vertices must form triangles");

    s64 tri_count = vertex_count / 3;
    for (int i = 0; i < tri_count; i += 1)
    {
        Vertex *v = vertices + i * 3;

        Vec3f delta12 = v[2].position - v[1].position;
        Vec3f delta10 = v[0].position - v[1].position;
        Vec3f normal = Cross (delta12, delta10);
        normal = Normalized (normal);

        v[0].normal = normal;
        v[1].normal = normal;
        v[2].normal = normal;
    }
}

void CalculateNormalsSmooth (Vertex *vertices, s64 vertex_count, u32 *indices, s64 index_count)
{
    for (int i = 0; i < index_count; i += 3)
    {
        Vertex &v0 = vertices[indices[i + 0]];
        Vertex &v1 = vertices[indices[i + 1]];
        Vertex &v2 = vertices[indices[i + 2]];

        Vec3f a = v0.position;
        Vec3f b = v1.position;
        Vec3f c = v2.position;

        Vec3f bc = c - b;
        Vec3f ba = a - b;
        Vec3f normal = Normalized (Cross (bc, ba));

        v0.normal += normal;
        v1.normal += normal;
        v2.normal += normal;
    }

    for (int i = 0; i < vertex_count; i += 1)
        vertices[i].normal = Normalized (vertices[i].normal);
}

void CalculateTangents (Vertex *vertices, s64 vertex_count, u32 *indices, s64 index_count)
{
    Vec3f *temp_tangents = (Vec3f *)malloc (sizeof (Vec3f) * vertex_count * 2);
    if (!temp_tangents)
        return;
    defer (free (temp_tangents));

    Vec3f *temp_bitangents = temp_tangents + vertex_count;

    memset (temp_tangents, 0, sizeof (Vec3f) * vertex_count * 2);

    for (int i = 0; i < index_count; i += 3)
    {
        u32 i0 = indices[i + 0];
        u32 i1 = indices[i + 1];
        u32 i2 = indices[i + 2];

        auto p0 = vertices[i0].position;
        auto p1 = vertices[i1].position;
        auto p2 = vertices[i2].position;
        auto t0 = vertices[i0].tex_coords;
        auto t1 = vertices[i1].tex_coords;
        auto t2 = vertices[i2].tex_coords;

        auto e1 = p1 - p0;
        auto e2 = p2 - p0;
        auto x1 = t1.x - t0.x;
        auto x2 = t2.x - t0.x;
        auto y1 = t1.y - t0.y;
        auto y2 = t2.y - t0.y;

        float inv_r = x1 * y2 - x2 * y1;
        if (inv_r == 0)
            continue;

        float r = 1 / inv_r;
        auto t = (e1 * y2 - e2 * y1) * r;
        auto b = (e2 * x1 - e1 * x2) * r;

        temp_tangents[i0] += t;
        temp_tangents[i1] += t;
        temp_tangents[i2] += t;
        temp_tangents[i0] += b;
        temp_tangents[i1] += b;
        temp_tangents[i2] += b;
    }

    for (int i = 0; i < vertex_count; i += 1)
    {
        auto t = temp_tangents[i];
        auto b = temp_bitangents[i];
        auto n = vertices[i].normal;

        Vec3f tangent = Normalized (Reject (t, n));
        float tangent_sign = (Dot (Cross (t, b), n) > 0) ? 1.0f : -1.0;

        vertices[i].tangent = {tangent.x, tangent.y, tangent.z, tangent_sign};
    }
}

void CalculateBoundingBox (Mesh *mesh)
{
    mesh->aabb_min = {FLT_MAX, FLT_MAX, FLT_MAX};
    mesh->aabb_max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};
    for (int i = 0; i < mesh->vertex_count; i += 1)
    {
        mesh->aabb_min.x = Min (mesh->aabb_min.x, mesh->vertices[i].position.x);
        mesh->aabb_min.y = Min (mesh->aabb_min.y, mesh->vertices[i].position.y);
        mesh->aabb_min.z = Min (mesh->aabb_min.z, mesh->vertices[i].position.z);

        mesh->aabb_max.x = Max (mesh->aabb_max.x, mesh->vertices[i].position.x);
        mesh->aabb_max.y = Max (mesh->aabb_max.y, mesh->vertices[i].position.y);
        mesh->aabb_max.z = Max (mesh->aabb_max.z, mesh->vertices[i].position.z);
    }
}

void CalculateBasicTexCoords (Mesh *mesh)
{
    for (int i = 0; i < mesh->vertex_count; i += 1)
    {
        mesh->vertices[i].tex_coords.x = InverseLerp (mesh->aabb_min.z, mesh->aabb_max.z, mesh->vertices[i].position.z);
        mesh->vertices[i].tex_coords.y = InverseLerp (mesh->aabb_min.y, mesh->aabb_max.y, mesh->vertices[i].position.y);
    }
}
