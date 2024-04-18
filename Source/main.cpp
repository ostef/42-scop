#include "Scop_Core.h"
#include "Scop_Math.h"
#include "Scop_Graphics.h"

#include <stdio.h>

Result<String> ReadEntireFile (const char *filename)
{
    FILE *file = fopen (filename, "rb");
    if (!file)
        return Result<String>::Bad (false);

    defer (fclose (file));

    fseek (file, 0, SEEK_END);
    s64 size = ftell (file);
    rewind (file);

    char *data = (char *)malloc (size);
    if (!data)
        return Result<String>::Bad (false);

    s64 number_of_bytes_read = fread (data, 1, size, file);

    String str = String{number_of_bytes_read, data};

    return Result<String>::Good (str, true);
}

int main (int argc, char **args)
{
    glfwInit ();
    defer (glfwTerminate ());

    glfwWindowHint (GLFW_CLIENT_API, GLFW_NO_API);

    char window_title[100];
    snprintf (window_title, sizeof (window_title), "Scop (%s backend)", SCOP_BACKEND_NAME);
    GLFWwindow *window = glfwCreateWindow (1280, 720, window_title, null, null);
    defer (glfwDestroyWindow (window));

    bool graphics_init_ok = GfxInitBackend (window);
    if (!graphics_init_ok)
    {
        printf ("Could not initialize %s graphics backend\n", SCOP_BACKEND_NAME);
        return 1;
    }

    while (!glfwWindowShouldClose (window))
    {
        glfwPollEvents ();
    }

    return 0;
}
