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

struct WeldMeshResult
{
    Vertex *unique_vertices;
    s64 unique_vertex_count;
    u32 *indices;
    s64 index_count;
};

static WeldMeshResult WeldMesh (Vertex *vertices, u32 vertex_count)
{
    u32 *remap_table = (u32 *)malloc (sizeof (u32) * vertex_count);

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

bool LoadMeshFromObjFile (const char *filename, Mesh *mesh)
{
    auto read_result = ReadEntireFile (filename);
    if (!read_result.ok)
    {
        LogError ("Could not read file '%s'", filename);
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

            Vec3f *normal = ArrayPush (&normals);
            normal->x = n0.value;
            normal->y = n1.value;
            normal->z = n2.value;
        }
        else if (MatchAlphaNumeric (&parser, "f"))
        {
            SkipWhitespaceAndComments (&parser);

            OBJTriangleFace *face = ArrayPush (&faces);

            for (int i = 0; i < 3; i += 1)
            {
                auto p = ParseInt (&parser);
                if (!p.ok)
                {
                    return false;
                }

                face->indices[i].position = p.value - 1;

                Result<int> t = {};
                Result<int> n = {};

                if (MatchString (&parser, "/"))
                {
                    t = ParseInt (&parser);
                    if (!t.ok)
                    {
                        return false;
                    }

                    face->indices[i].tex_coords = t.value - 1;

                    if (MatchString (&parser, "/"))
                    {
                        n = ParseInt (&parser);
                        if (!n.ok)
                        {
                            return false;
                        }

                        face->indices[i].normal = n.value - 1;
                    }
                }
            }
        }
        else
        {
            AdvanceToNextLine (&parser);
        }
    }

    s64 vertex_count = faces.count * 3;
    Vertex *vertices = (Vertex *)malloc (sizeof (Vertex) * vertex_count);

    for (int f = 0; f < faces.count; f += 1)
    {
        for (int i = 0; i < 3; i += 1)
        {
            Vertex *v = &vertices[f * 3 + i];

            int index = faces[f].indices[i].position;
            v->position = positions[index];

            index = faces[f].indices[i].normal;
            if (index > 0)
                v->normal = normals[index];
            else
                v->normal = {};

            index = faces[f].indices[i].tex_coords;
            if (index > 0)
                v->tex_coords = tex_coords[index];
            else
                v->tex_coords = {};
        }
    }

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

    GfxCreateMeshObjects (mesh);

    LogMessage ("Loaded mesh '%s', %lld vertices, %lld indices", filename, mesh->vertex_count, mesh->index_count);

    return true;
}
