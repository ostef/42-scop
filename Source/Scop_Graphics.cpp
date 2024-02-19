#include "Scop_Core.h"
#include "Scop_Graphics.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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
            while (!IsAtEnd (*parser) && parser->text[parser->offset] != '\n')
            {
                Advance (parser);
            }
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
    if (parser->size - parser->offset < len)
        return false;

    int res = strncmp (parser->text + parser->offset, str, parser->size - parser->offset);

    return res != 0;
}

static bool MatchString (Parser *parser, const char *str)
{
    if (EqualsString (parser, str))
    {
        Advance (parser, strlen (str));
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
        return false;

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
                return false;

            auto p1 = ParseFloat (&parser);
            if (!p1.ok)
                return false;

            auto p2 = ParseFloat (&parser);
            if (!p2.ok)
                return false;

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
                return false;

            auto t1 = ParseFloat (&parser);
            if (!t1.ok)
                return false;

            Vec2f *uv = ArrayPush (&tex_coords);
            uv->x = t0.value;
            uv->y = t1.value;
        }
        else if (MatchAlphaNumeric (&parser, "vn"))
        {
            SkipWhitespaceAndComments (&parser);

            auto n0 = ParseFloat (&parser);
            if (!n0.ok)
                return false;

            auto n1 = ParseFloat (&parser);
            if (!n1.ok)
                return false;

            auto n2 = ParseFloat (&parser);
            if (!n2.ok)
                return false;

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
                    return false;

                face->indices[i].position = p.value;

                Result<int> t = {};
                Result<int> n = {};

                if (MatchString (&parser, "/"))
                {
                    t = ParseInt (&parser);
                    if (!t.ok)
                        return false;

                    face->indices[i].tex_coords = t.value;

                    if (MatchString (&parser, "/"))
                    {
                        n = ParseInt (&parser);
                        if (!n.ok)
                            return false;

                        face->indices[i].normal = n.value;
                    }
                }
            }
        }
        else
        {
            Advance (&parser);
        }
    }

    mesh->vertex_count = faces.count * 3;
    mesh->vertices = (Vertex *)malloc (sizeof (Vertex) * mesh->vertex_count);

    mesh->index_count = faces.count * 3;
    mesh->indices = (u32 *)malloc (sizeof (u32) * mesh->index_count);

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

            vertex->position = positions[face.indices[i].position];

            if (face.indices[i].normal >= 0)
                vertex->normal = normals[face.indices[i].normal];

            if (face.indices[i].tex_coords >= 0)
                vertex->tex_coords = tex_coords[face.indices[i].tex_coords];
        }
    }

    return true;
}

static const char *Vulkan_Validation_Layers[] = {
    "VK_LAYER_KHRONOS_validation"
};

static bool CheckVulkanValidationLayerSupport ()
{
    u32 layer_count = 0;
    vkEnumerateInstanceLayerProperties (&layer_count, null);

    VkLayerProperties *available_layers = (VkLayerProperties *)malloc (sizeof (VkLayerProperties) * layer_count);
    vkEnumerateInstanceLayerProperties (&layer_count, available_layers);

    if (layer_count < StaticArraySize (Vulkan_Validation_Layers))
        return false;

    int number_of_layers_found = 0;
    for (int i = 0; i < StaticArraySize (Vulkan_Validation_Layers); i += 1)
    {
        bool found = false;
        for (int j = 0; j < layer_count; j += 1)
        {
            if (strcmp (available_layers[j].layerName, Vulkan_Validation_Layers[i]) == 0)
            {
                found = true;
                break;
            }
        }

        if (!found)
            return false;
    }

    return true;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback (
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
    void *user_data
)
{
    if (!callback_data->pMessage)
        return VK_FALSE;

    const char *severity_str = "";
    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        severity_str = "ERROR";
    else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        severity_str = "WARNING";
    else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        severity_str = "INFO";
    else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
        severity_str = "message";

    printf ("Vulkan %s: %s\n", severity_str, callback_data->pMessage);

    return VK_FALSE;
}

bool GfxInitVulkan (GLFWwindow *window)
{
    if (!CheckVulkanValidationLayerSupport ())
        return false;

    VkApplicationInfo app_info {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Scop Vk";
    app_info.applicationVersion = VK_MAKE_VERSION (1, 0, 0);
    app_info.pEngineName = "Scop Engine";
    app_info.engineVersion = VK_MAKE_VERSION (1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    u32 glfw_extension_count = 0;
    const char **glfw_extensions = glfwGetRequiredInstanceExtensions (&glfw_extension_count);

    Array<const char *> extensions {};

    for (int i = 0; i < glfw_extension_count; i += 1)
        ArrayPush (&extensions, glfw_extensions[i]);

    ArrayPush (&extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    create_info.enabledExtensionCount = extensions.count;
    create_info.ppEnabledExtensionNames = extensions.data;

    create_info.enabledLayerCount = StaticArraySize (Vulkan_Validation_Layers);
    create_info.ppEnabledLayerNames = Vulkan_Validation_Layers;

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info {};
    debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    debug_create_info.pfnUserCallback = VulkanDebugCallback;

    create_info.pNext = &debug_create_info;

    VkInstance instance {};

    if (vkCreateInstance (&create_info, null, &instance) != VK_SUCCESS)
        return false;

    VkDebugUtilsMessengerEXT messenger {};

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr (instance, "vkCreateDebugUtilsMessengerEXT");
    if (!func)
    {
        printf ("Could not load vkCreateDebugUtilsMessengerEXT\n");
        return false;
    }

    auto messenger_result = func (instance, &debug_create_info, null, &messenger);
    if (messenger_result != VK_SUCCESS)
    {
        printf ("vkCreateDebugUtilsMessengerEXT returned %h\n", messenger_result);
        return false;
    }

    printf ("Initialized Vulkan backend\n");

    return true;
}
