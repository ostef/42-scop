#include "Scop_Core.h"
#include "Scop_Graphics.h"

static GLuint g_shader;

static bool CheckShader (GLuint shader, const char *desc)
{
    GLint status, log_length;
    glGetShaderiv (shader, GL_COMPILE_STATUS, &status);
    glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &log_length);

    if (!status)
    {
        if (log_length > 0)
        {
            char *msg = (char *)malloc (log_length + 1);
            defer (free (msg));

            glGetShaderInfoLog (shader, log_length, null, msg);
            LogError ("%s:\n\n%s", desc, msg);
        }
        else
        {
            LogError ("%s", desc);
        }

        return false;
    }

    return true;
}

static bool CheckShaderProgram (GLuint shader, const char *desc)
{
    GLint status, log_length;
    glGetProgramiv (shader, GL_LINK_STATUS, &status);
    glGetProgramiv (shader, GL_INFO_LOG_LENGTH, &log_length);

    if (!status)
    {
        if (log_length > 0)
        {
            char *msg = (char *)malloc (log_length + 1);
            defer (free (msg));

            glGetProgramInfoLog (shader, log_length, null, msg);
            LogError ("%s:\n\n%s", desc, msg);
        }
        else
        {
            LogError ("%s", desc);
        }

        return false;
    }

    return true;
}

static GLuint CreateShaderProgram (const char *vs_filename, const char *fs_filename)
{
    auto vs_read_res = ReadEntireFile (vs_filename);
    if (!vs_read_res.ok)
    {
        LogError ("Could not read file '%s'", vs_filename);
        return 0;
    }

    String vs_source = vs_read_res.value;
    defer (free (vs_source.data));

    auto fs_read_res = ReadEntireFile (fs_filename);
    if (!fs_read_res.ok)
    {
        LogError ("Could not read file '%s'", fs_filename);
        return 0;
    }

    String fs_source = fs_read_res.value;
    defer (free (fs_source.data));

    GLuint vs = glCreateShader (GL_VERTEX_SHADER);
    defer (glDeleteShader (vs));

    glShaderSource (vs, 1, &vs_source.data, null);
    glCompileShader (vs);

    if (!CheckShader (vs, "Could not compile vertex shader"))
        return 0;

    GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
    defer (glDeleteShader (fs));

    glShaderSource (fs, 1, &fs_source.data, null);
    glCompileShader (fs);

    if (!CheckShader (fs, "Could not compile fragment shader"))
        return 0;

    GLuint program = glCreateProgram ();

    glAttachShader (program, vs);
    glAttachShader (program, fs);
    glLinkProgram (program);

    if (!CheckShaderProgram (program, "Could not link shader program"))
    {
        glDeleteProgram (program);

        return 0;
    }

    return program;
}

bool GfxInitBackend ()
{
    glfwInit ();

    glfwSetErrorCallback (GLFWErrorCallback);

    glfwWindowHint (GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint (GLFW_SAMPLES, 4);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint (GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    char window_title[100];
    snprintf (window_title, sizeof (window_title), "Scop (%s backend)", SCOP_BACKEND_NAME);

    g_main_window = glfwCreateWindow (1280, 720, window_title, null, null);
    if (!g_main_window)
    {
        LogError ("Could not create GLFW window");
        return false;
    }

    glfwMakeContextCurrent (g_main_window);

    int ok = gladLoadGLLoader ((GLADloadproc) glfwGetProcAddress);
    if (!ok)
    {
        LogError ("Failed to initialize OpenGL context");
        return false;
    }

    LogMessage ("OpenGL verson: %d.%d", GLVersion.major, GLVersion.minor);

    g_shader = CreateShaderProgram ("Shaders/Mesh_VS.glsl", "Shaders/Mesh_FS.glsl");
    if (!g_shader)
        return false;

    glfwSwapInterval (1);

    return true;
}

void GfxTerminateBackend ()
{
    glfwDestroyWindow (g_main_window);
    g_main_window = null;

    glfwTerminate ();
}

void GfxCreateMeshObjects (Mesh *mesh)
{
    glGenVertexArrays (1, &mesh->gfx_objects.vao);
    glGenBuffers (2, mesh->gfx_objects.buffers);

    glBindVertexArray (mesh->gfx_objects.vao);

    glBindBuffer (GL_ARRAY_BUFFER, mesh->gfx_objects.vbo);
    glBufferData (GL_ARRAY_BUFFER, sizeof (Vertex) * mesh->vertex_count, mesh->vertices, GL_STATIC_DRAW);

    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, mesh->gfx_objects.ibo);
    glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (u32) * mesh->index_count, mesh->indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray (GL_Attrib_Position);
    glVertexAttribPointer (GL_Attrib_Position, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), (void *)offsetof (Vertex, position));

    glEnableVertexAttribArray (GL_Attrib_Normal);
    glVertexAttribPointer (GL_Attrib_Normal, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), (void *)offsetof (Vertex, normal));

    glEnableVertexAttribArray (GL_Attrib_Tex_Coords);
    glVertexAttribPointer (GL_Attrib_Tex_Coords, 2, GL_FLOAT, GL_FALSE, sizeof (Vertex), (void *)offsetof (Vertex, tex_coords));

    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindVertexArray (0);
}

void GfxRenderFrame (Mesh *mesh, GfxTexture texture, const Vec3f &light_position)
{
    int viewport_width, viewport_height;
    glfwGetFramebufferSize (g_main_window, &viewport_width, &viewport_height);

    glViewport (0, 0, viewport_width, viewport_height);
    glClearColor (0.1, 0.1, 0.1, 1);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LESS);

    glUseProgram (g_shader);
    glUniformMatrix4fv (
        glGetUniformLocation (g_shader, "u_View_Projection_Matrix"),
        1, GL_TRUE, &g_camera.view_projection_matrix.r0c0
    );

    Mat4f model_matrix = {};
    glUniformMatrix4fv (
        glGetUniformLocation (g_shader, "u_Model_Matrix"),
        1, GL_TRUE, &model_matrix.r0c0
    );

    glUniform3f (
        glGetUniformLocation (g_shader, "u_Light_Position"),
        light_position.x, light_position.y, light_position.z
    );

    glBindTexture (GL_TEXTURE_2D, texture);

    glBindVertexArray (mesh->gfx_objects.vao);
    glBindBuffer (GL_ARRAY_BUFFER, mesh->gfx_objects.vbo);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, mesh->gfx_objects.ibo);

    glDrawElements (GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, null);

    glBindTexture (GL_TEXTURE_2D, 0);

    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray (0);

    glfwSwapBuffers (g_main_window);
}

GfxTexture GfxCreateTexture (void *data, u32 width, u32 height)
{
    GLuint tex;
    glGenTextures (1, &tex);
    glBindTexture (GL_TEXTURE_2D, tex);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture (GL_TEXTURE_2D, 0);

    return tex;
}
