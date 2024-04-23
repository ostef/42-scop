#include "Scop_Core.h"
#include "Scop_Math.h"
#include "Scop_Graphics.h"

GLFWwindow *g_main_window = null;
Camera g_camera;

static float g_model_rotation;
static Vec3f g_model_position;

#define Camera_Rotate_Speed 0.1
#define Camera_Movement_Speed 0.1
#define Model_Rotate_Speed 0.1
#define Model_Move_Speed 0.1

static Vec2f g_mouse_delta;
static Vec2f g_mouse_wheel;

struct ProgramArguments
{
    const char *mesh_filename = null;
    const char *texture_filename = null;
    Vec3f light_position = {0,0,0};
    Vec3f light_color = {1,0,1};
};

static void UpdateInput ()
{
    g_mouse_wheel = {};

    double x, y;
    glfwGetCursorPos (g_main_window, &x, &y);

    glfwPollEvents ();

    double new_x, new_y;
    glfwGetCursorPos (g_main_window, &new_x, &new_y);

    g_mouse_delta.x = (float)(new_x - x);
    g_mouse_delta.y = (float)(new_y - y);
}

static void UpdateCamera ()
{
    Vec2f mouse_input = {};

    if (glfwGetMouseButton (g_main_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        glfwSetInputMode (g_main_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        mouse_input = g_mouse_delta;
    }

    g_camera.distance_from_target = Clamp (
        g_camera.distance_from_target - g_mouse_wheel.y * 0.5,
        1, 10
    );

    g_camera.yaw_pitch.x += mouse_input.x * Camera_Rotate_Speed;
    g_camera.yaw_pitch.y += mouse_input.y * Camera_Rotate_Speed;
    g_camera.yaw_pitch.y = Clamp (g_camera.yaw_pitch.y, -90, 90);

    Mat4f rotation_matrix = Mat4fRotate ({0,1,0}, ToRads (g_camera.yaw_pitch.x))
        * Mat4fRotate ({1,0,0}, ToRads (g_camera.yaw_pitch.y));

    g_camera.position =
        g_camera.target
        + ForwardVector (rotation_matrix) * g_camera.distance_from_target
        + Vec3f{g_camera.offset.x, g_camera.offset.y, 0};

    Mat4f transform = Mat4fTranslate (g_camera.position) * rotation_matrix;

    int width, height;
    glfwGetFramebufferSize (g_main_window, &width, &height);

    g_camera.view_matrix = Inverted (transform);
    g_camera.projection_matrix = Mat4fPerspectiveProjection (70, width / (float)height, 0.1, 100.0);
    g_camera.view_projection_matrix = g_camera.projection_matrix * g_camera.view_matrix;
}

static void UpdateModelTransform ()
{
    Vec2f mouse_input = {};
    if (glfwGetMouseButton (g_main_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        glfwSetInputMode (g_main_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        mouse_input = g_mouse_delta;
    }

    g_model_rotation += mouse_input.x * Model_Rotate_Speed;

    Vec3f move_input = {};
    move_input.x = (glfwGetKey (g_main_window, GLFW_KEY_D) == GLFW_PRESS)
        - (glfwGetKey (g_main_window, GLFW_KEY_A) == GLFW_PRESS);
    move_input.y = (glfwGetKey (g_main_window, GLFW_KEY_E) == GLFW_PRESS)
        - (glfwGetKey (g_main_window, GLFW_KEY_Q) == GLFW_PRESS);
    move_input.z = (glfwGetKey (g_main_window, GLFW_KEY_W) == GLFW_PRESS)
        - (glfwGetKey (g_main_window, GLFW_KEY_S) == GLFW_PRESS);

    move_input = Normalized (move_input);

    g_model_position += move_input * Model_Move_Speed;
}

static void GLFWScrollCallback (GLFWwindow *window, double x, double y)
{
    g_mouse_wheel.x += (float)x;
    g_mouse_wheel.y += (float)y;
}

static bool ParseFloat (const char *str, float *result)
{
    *result = 0.0f;

    char *end;
    float f = strtof (str, &end);
    if (end != str + strlen (str))
        return false;

    *result = f;

    return true;
}

static bool ParseProgramArguments (int argc, char **argv, ProgramArguments *result)
{
    const char *Usage = "Usage: ScopGL mesh_filename [texture_filename] [light_x light_y light_z] [light_r light_g light_b]";

    if (argc <= 1)
    {
        LogError ("Missing mesh argument");
        LogMessage (Usage);
        return false;
    }

    argc -= 1;
    argv += 1;

    result->mesh_filename = *argv;
    argc -= 1;
    argv += 1;

    if (argc == 0)
        return true;

    result->texture_filename = *argv;
    argc -= 1;
    argv += 1;

    if (argc == 0)
        return true;

    if (argc < 3)
    {
        if (argc == 2)
            LogError ("Missing light position Z component");
        else if (argc == 1)
            LogError ("Missing light position Y and Z components");

        return false;
    }

    if (!ParseFloat (argv[0], &result->light_position.x))
    {
        LogError ("Invalid argument for light X position");
        return false;
    }

    if (!ParseFloat (argv[1], &result->light_position.y))
    {
        LogError ("Invalid argument for light Y position");
        return false;
    }

    if (!ParseFloat (argv[2], &result->light_position.z))
    {
        LogError ("Invalid argument for light Z position");
        return false;
    }

    argc -= 3;
    argv += 3;

    if (argc == 0)
        return true;

    if (argc < 3)
    {
        if (argc == 2)
            LogError ("Missing light color B component");
        else if (argc == 1)
            LogError ("Missing light color G and B components");

        return false;
    }

    if (!ParseFloat (argv[0], &result->light_color.x))
    {
        LogError ("Invalid argument for light R color component");
        return false;
    }

    if (!ParseFloat (argv[1], &result->light_color.y))
    {
        LogError ("Invalid argument for light G color component");
        return false;
    }

    if (!ParseFloat (argv[2], &result->light_color.z))
    {
        LogError ("Invalid argument for light B color component");
        return false;
    }

    argc -= 3;
    argv += 3;

    if (argc != 0)
    {
        LogError ("Too many arguments");
        LogMessage (Usage);
        return false;
    }

    return true;
}

int main (int argc, char **argv)
{
    bool gfx_ok = GfxInitBackend ();
    if (!gfx_ok)
    {
        LogError ("Could not initialize %s graphics backend", SCOP_BACKEND_NAME);
        return 1;
    }

    defer (GfxTerminateBackend ());

    if (glfwRawMouseMotionSupported ())
        glfwSetInputMode (g_main_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    glfwSetScrollCallback (g_main_window, GLFWScrollCallback);

    ProgramArguments args = {};

    if (!ParseProgramArguments (argc, argv, &args))
        return 1;

    GfxTexture texture = {};
    if (args.texture_filename)
    {
        if (!LoadTextureFromFile (args.texture_filename, &texture, null, null))
        {
            LogError ("Could not load texture '%s'", args.texture_filename);
            return 1;
        }
    }

    defer (GfxDestroyTexture (&texture));

    Mesh mesh = {};
    if (!LoadMeshFromObjFile (args.mesh_filename, &mesh))
    {
        LogError ("Could not load mesh '%s'", args.mesh_filename);
        return 1;
    }

    defer (DestroyMesh (&mesh));

    g_camera.target = (mesh.aabb_min + mesh.aabb_max) * 0.5;
    g_camera.distance_from_target = 3;

    while (!glfwWindowShouldClose (g_main_window))
    {
        UpdateInput ();

        if (glfwGetMouseButton (g_main_window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_PRESS
        && glfwGetMouseButton (g_main_window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS)
            glfwSetInputMode (g_main_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        UpdateModelTransform ();
        UpdateCamera ();

        Mat4f model_matrix = Mat4fTranslate (g_model_position)
            * Mat4fRotate ({0,1,0}, ToRads (g_model_rotation));

        GfxRenderFrame (&mesh, texture, model_matrix, args.light_position, args.light_color);
    }

    return 0;
}

void GLFWErrorCallback (int code, const char *description)
{
    if (description)
        LogError ("GLFW: %s", description);
}

void LogMessage (const char *str, ...)
{
    va_list args;
    va_start (args, str);

    vprintf (str, args);

    va_end (args);

    printf ("\n");
}

void LogWarning (const char *str, ...)
{
    printf ("\x1b[1;33mWarning: ");

    va_list args;
    va_start (args, str);

    vprintf (str, args);

    va_end (args);

    printf ("\x1b[0m\n");
}

void LogError (const char *str, ...)
{
    printf ("\x1b[1;31mError: ");

    va_list args;
    va_start (args, str);

    vprintf (str, args);

    va_end (args);

    printf ("\x1b[0m\n");
}

Result<String> ReadEntireFile (const char *filename)
{
    FILE *file = fopen (filename, "rb");
    if (!file)
        return Result<String>::Bad (false);

    defer (fclose (file));

    fseek (file, 0, SEEK_END);
    s64 size = ftell (file);
    rewind (file);

    char *data = (char *)malloc (size + 1);
    if (!data)
        return Result<String>::Bad (false);

    s64 number_of_bytes_read = fread (data, 1, size, file);
    data[number_of_bytes_read] = 0;

    String str = String{number_of_bytes_read, data};

    return Result<String>::Good (str, true);
}

bool LoadTextureFromFile (const char *filename, GfxTexture *texture, u32 *width, u32 *height)
{
    *texture = 0;
    if (width)
        *width = 0;
    if (height)
        *height = 0;

    int x, y;
    u8 *pixels = stbi_load (filename, &x, &y, null, 4);
    if (!pixels)
    {
        return false;
    }

    defer (stbi_image_free (pixels));

    GfxTexture result = GfxCreateTexture (pixels, x, y);
    if (!result)
        return false;

    *texture = result;
    if (width)
        *width = x;
    if (height)
        *height = y;

    LogMessage ("Loaded texture %s, %u by %u px", filename, x, y);

    return true;
}

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
