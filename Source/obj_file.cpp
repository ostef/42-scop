#include "Scop_Core.h"
#include "Scop_Graphics.h"

struct Parser
{
    char *text = null;
    s64 offset = 0;
    s64 size = 0;
};

static void ParserInit (Parser *parser, String str)
{
    parser->text = str.data;
    parser->size = str.length;
}

static bool IsAtEnd (const Parser &parser)
{
    return parser.offset >= parser.size;
}

static void Advance (Parser *parser, int count = 1)
{
    int i = 0;
    while (!IsAtEnd (*parser) && i < count)
    {
        parser->offset += 1;
        i += 1;
    }
}

static void AdvanceToNextLine (Parser *parser)
{
    while (!IsAtEnd (*parser) && parser->text[parser->offset] != '\n')
    {
        Advance (parser);
    }

    Advance (parser);
}

static void SkipWhitespaceAndComments (Parser *parser)
{
    while (!IsAtEnd (*parser))
    {
        if (isspace (parser->text[parser->offset]))
        {
            Advance (parser);
        }
        else if (parser->text[parser->offset] == '#')
        {
            AdvanceToNextLine (parser);
        }
        else
        {
            break;
        }
    }
}

static Result<float> ParseFloat (Parser *parser)
{
    char *start = parser->text + parser->offset;
    char *end = parser->text + parser->size;

    float value = strtof (start, &end);
    if (end == start)
        return Result<float>::Bad (false);

    Advance (parser, (int)(end - start));

    return Result<float>::Good (value, true);
}

static Result<int> ParseInt (Parser *parser)
{
    char *start = parser->text + parser->offset;
    char *end = parser->text + parser->size;

    long value = strtol (start, &end, 10);
    if (end == start)
        return Result<int>::Bad (false);

    Advance (parser, (int)(end - start));

    return Result<int>::Good ((int)value, true);
}

static bool EqualsString (Parser *parser, const char *str)
{
    int len = strlen (str);
    if (parser->offset + len > parser->size)
        return false;

    int res = strncmp (parser->text + parser->offset, str, len);

    return res == 0;
}

static bool MatchString (Parser *parser, const char *str)
{
    if (EqualsString (parser, str))
    {
        Advance (parser, strlen (str));

        return true;
    }

    return false;
}

static bool MatchAlphaNumeric (Parser *parser, const char *str)
{
    if (!EqualsString (parser, str))
        return false;

    int len = strlen (str);
    if (parser->offset + len >= parser->size)
    {
        Advance (parser, len);
        return true;
    }

    if (!isalnum (parser->text[parser->offset + len]))
    {
        Advance (parser, len);
        return true;
    }

    return false;
}

struct OBJIndex
{
    s64 position;
    s64 normal;
    s64 tex_coords;
};

struct OBJTriangleFace
{
    OBJIndex indices[3];
};

struct OBJQuadFace
{
    OBJIndex indices[4];
};

bool LoadMeshFromObjFile (const char *filename, Mesh *mesh, LoadMeshFlags flags)
{
    auto read_result = ReadEntireFile (filename);
    if (!read_result.ok)
    {
        return false;
    }

    String file_contents = read_result.value;
    defer (free (file_contents.data));

    Parser parser {};
    ParserInit (&parser, file_contents);

    Array<Vec3f> positions = {};
    Array<Vec3f> normals = {};
    Array<Vec2f> tex_coords = {};
    Array<OBJTriangleFace> faces = {};

    defer (ArrayFree (&positions));
    defer (ArrayFree (&normals));
    defer (ArrayFree (&tex_coords));
    defer (ArrayFree (&faces));

    while (!IsAtEnd (parser))
    {
        SkipWhitespaceAndComments (&parser);

        if (MatchAlphaNumeric (&parser, "v"))
        {
            SkipWhitespaceAndComments (&parser);

            auto p0 = ParseFloat (&parser);
            if (!p0.ok)
            {
                return false;
            }

            auto p1 = ParseFloat (&parser);
            if (!p1.ok)
            {
                return false;
            }

            auto p2 = ParseFloat (&parser);
            if (!p2.ok)
            {
                return false;
            }

            Vec3f *vertex = ArrayPush (&positions);
            vertex->x = p0.value;
            vertex->y = p1.value;
            vertex->z = p2.value;
        }
        else if (MatchAlphaNumeric (&parser, "vt"))
        {
            SkipWhitespaceAndComments (&parser);

            auto t0 = ParseFloat (&parser);
            if (!t0.ok)
            {
                return false;
            }

            auto t1 = ParseFloat (&parser);
            if (!t1.ok)
            {
                return false;
            }

            Vec2f *uv = ArrayPush (&tex_coords);
            uv->x = t0.value;
            uv->y = t1.value;
        }
        else if (MatchAlphaNumeric (&parser, "vn"))
        {
            SkipWhitespaceAndComments (&parser);

            auto n0 = ParseFloat (&parser);
            if (!n0.ok)
            {
                return false;
            }

            auto n1 = ParseFloat (&parser);
            if (!n1.ok)
            {
                return false;
            }

            auto n2 = ParseFloat (&parser);
            if (!n2.ok)
            {
                return false;
            }

            if (!(flags & LoadMesh_IgnoreSuppliedNormals))
            {
                Vec3f *normal = ArrayPush (&normals);
                normal->x = n0.value;
                normal->y = n1.value;
                normal->z = n2.value;
            }
        }
        else if (MatchAlphaNumeric (&parser, "f"))
        {
            SkipWhitespaceAndComments (&parser);

            OBJQuadFace quad = {};
            int i = 0;

            // Parse as potential quads, triangulate afterwards
            for (i = 0; i < 4; i += 1)
            {
                if (i == 3)
                {
                    bool found_newline = false;
                    while (!IsAtEnd (parser))
                    {
                        if (parser.text[parser.offset] == '\n')
                        {
                            found_newline = true;
                            break;
                        }
                        else if (isspace (parser.text[parser.offset]))
                        {
                            Advance (&parser);
                        }
                        else
                        {
                            break;
                        }
                    }

                    if (found_newline)
                        break;
                }

                auto p = ParseInt (&parser);
                if (!p.ok)
                {
                    return false;
                }

                quad.indices[i].position = p.value;

                Result<int> t = {};
                Result<int> n = {};

                if (MatchString (&parser, "/"))
                {
                    t = ParseInt (&parser);
                    if (!t.ok)
                    {
                        return false;
                    }

                    quad.indices[i].tex_coords = t.value;

                    if (MatchString (&parser, "/"))
                    {
                        n = ParseInt (&parser);
                        if (!n.ok)
                        {
                            return false;
                        }

                        if (!(flags & LoadMesh_IgnoreSuppliedNormals))
                            quad.indices[i].normal = n.value;
                    }
                }
            }

            if (i != 3 && i != 4)
            {
                LogError ("Mesh has faces that are neither triangles or quads");
                return false;
            }

            if (i == 3)
            {
                OBJTriangleFace *face = ArrayPush (&faces);
                face->indices[0] = quad.indices[0];
                face->indices[1] = quad.indices[1];
                face->indices[2] = quad.indices[2];
            }
            else
            {
                OBJTriangleFace *face = ArrayPush (&faces);
                face->indices[0] = quad.indices[0];
                face->indices[1] = quad.indices[1];
                face->indices[2] = quad.indices[2];

                face = ArrayPush (&faces);
                face->indices[0] = quad.indices[0];
                face->indices[1] = quad.indices[2];
                face->indices[2] = quad.indices[3];
            }
        }
        else
        {
            AdvanceToNextLine (&parser);
        }
    }

    s64 vertex_count = faces.count * 3;
    Vertex *vertices = (Vertex *)malloc (sizeof (Vertex) * vertex_count);
    if (!vertices)
    {
        LogError ("Could not allocate vertices");
        return false;
    }

    // Populate array of vertices
    for (int f = 0; f < faces.count; f += 1)
    {
        for (int i = 0; i < 3; i += 1)
        {
            Vertex *v = &vertices[f * 3 + i];

            int index = faces[f].indices[i].position - 1;
            if (index < 0 || index >= positions.count)
                return false;

            v->position = positions[index];

            index = faces[f].indices[i].normal - 1;
            if (index >= normals.count)
                return false;

            if (index >= 0)
                v->normal = normals[index];
            else
                v->normal = Vec3f{};

            index = faces[f].indices[i].tex_coords - 1;
            if (index >= tex_coords.count)
                return false;

            if (index >= 0)
                v->tex_coords = tex_coords[index];
            else
                v->tex_coords = Vec2f{};
        }
    }

    bool has_normals = normals.count != 0;
    if (normals.count == 0 && (flags & LoadMesh_CalculateNormalsFlat))
    {
        CalculateNormalsFlat (vertices, vertex_count);
        has_normals = true;
    }

    if (flags & LoadMesh_WeldMesh)
    {
        auto welded_mesh = WeldMesh (vertices, vertex_count);

        free (vertices);
        vertices = null;

        mesh->vertex_count = welded_mesh.unique_vertex_count;
        mesh->vertices = welded_mesh.unique_vertices;
        if (!mesh->vertices)
        {
            LogError ("Could not allocate mesh vertices");
            return false;
        }

        mesh->index_count = welded_mesh.index_count;
        mesh->indices = welded_mesh.indices;
        if (!mesh->indices)
        {
            LogError ("Could not allocate mesh indices");
            return false;
        }
    }
    else
    {
        mesh->vertices = vertices;
        mesh->vertex_count = vertex_count;

        mesh->indices = (u32 *)malloc (sizeof (u32) * vertex_count);
        mesh->index_count = vertex_count;
        if (!mesh->indices)
        {
            LogError ("Could not allocate mesh indices");
            return false;
        }

        for (int i = 0; i < vertex_count; i += 1)
        {
            mesh->indices[i] = i;
        }
    }

    if (normals.count == 0 && (flags & LoadMesh_CalculateNormalsSmooth))
    {
        CalculateNormalsSmooth (mesh->vertices, mesh->vertex_count, mesh->indices, mesh->index_count);
        has_normals = true;
    }

    if (tex_coords.count > 0 && has_normals && (flags & LoadMesh_CalculateTangents))
    {
        CalculateTangents (mesh->vertices, mesh->vertex_count, mesh->indices, mesh->index_count);
    }

    CalculateBoundingBox (mesh);

    if (tex_coords.count == 0 && flags & LoadMesh_CalculateTexCoords)
        CalculateBasicTexCoords (mesh);

    GfxCreateMeshObjects (mesh);

    LogMessage ("Loaded mesh '%s', %ld vertices, %ld indices", filename, mesh->vertex_count, mesh->index_count);

    return true;
}
