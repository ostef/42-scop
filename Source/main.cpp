#include "Scop_Core.h"
#include "Scop_Math.h"
#include "Scop_Graphics.h"

GLFWwindow *g_main_window = null;
Camera g_camera;

#define Camera_Rotate_Speed 10

static Vec2f g_mouse_delta;

static void UpdateInput ()
{
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
    else
    {
        glfwSetInputMode (g_main_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    g_camera.yaw_pitch.x += mouse_input.x * Camera_Rotate_Speed;
    g_camera.yaw_pitch.y += mouse_input.y * Camera_Rotate_Speed;
}

int main (int argc, char **args)
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

    Mesh mannequin_mesh = {};
    bool ok = LoadMeshFromObjFile ("Data/Male_Prototype.obj", &mannequin_mesh);
    if (!ok)
        LogError ("Could not load mesh 'Data/Male_Prototype.obj'");

    while (!glfwWindowShouldClose (g_main_window))
    {
        UpdateInput ();

        UpdateCamera ();

        GfxRenderFrame ();
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
