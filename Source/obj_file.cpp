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

    mesh->vertex_count = faces.count * 3;
    mesh->vertices = (Vertex *)malloc (sizeof (Vertex) * mesh->vertex_count);
    if (!mesh->vertices)
    {
        LogError ("Could not allocate mesh vertices");
        return false;
    }

    mesh->index_count = faces.count * 3;
    mesh->indices = (u32 *)malloc (sizeof (u32) * mesh->index_count);
    if (!mesh->indices)
    {
        LogError ("Could not allocate mesh indices");
        return false;
    }

    s64 vi = 0;
    s64 ii = 0;
    for (s64 fi = 0; fi < faces.count; fi += 1)
    {
        auto face = faces[fi];

        for (int i = 0; i < 3; i += 1)
        {
            mesh->indices[ii] = (u32)vi;
            ii += 1;

            Vertex *vertex = &mesh->vertices[vi];
            vi += 1;

            int position_index = face.indices[i].position;
            vertex->position = positions[position_index];

            if (face.indices[i].normal > 0)
            {
                int normal_index = face.indices[i].normal;
                vertex->normal = normals[normal_index];
            }

            if (face.indices[i].tex_coords > 0)
            {
                int tex_coords_index = face.indices[i].tex_coords;
                vertex->tex_coords = tex_coords[tex_coords_index];
            }
        }
    }

    GfxCreateMeshObjects (mesh);

    LogMessage ("Loaded mesh '%s', %lld vertices, %lld indices", filename, mesh->vertex_count, mesh->index_count);

    return true;
}
