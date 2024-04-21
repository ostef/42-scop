#include "Scop_Core.h"
#include "Scop_Math.h"
#include "Scop_Graphics.h"

GLFWwindow *g_main_window = null;
Camera g_camera;

#define Camera_Rotate_Speed 0.1
#define Camera_Movement_Speed 0.1

static Vec2f g_mouse_delta;
static Vec2f g_mouse_wheel;

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
    else
    {
        glfwSetInputMode (g_main_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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

static void GLFWScrollCallback (GLFWwindow *window, double x, double y)
{
    g_mouse_wheel.x += (float)x;
    g_mouse_wheel.y += (float)y;
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

    glfwSetScrollCallback (g_main_window, GLFWScrollCallback);

    g_camera.offset.y = 1;
    g_camera.distance_from_target = 3;

    Mesh mannequin_mesh = {};
    const char *mesh_filename = "Data/42.obj";
    bool ok = LoadMeshFromObjFile (mesh_filename, &mannequin_mesh);
    if (!ok)
        LogError ("Could not load mesh '%s'", mesh_filename);

    while (!glfwWindowShouldClose (g_main_window))
    {
        UpdateInput ();

        UpdateCamera ();

        GfxRenderFrame (&mannequin_mesh);
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
