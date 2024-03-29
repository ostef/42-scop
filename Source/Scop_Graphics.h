#ifndef SCOP_GRAPHICS_H
#define SCOP_GRAPHICS_H

#include "Scop_Math.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct Vertex
{
    Vec3f position;
    Vec3f normal;
    Vec2f tex_coords;
};

struct Mesh
{
    Vertex *vertices;
    s64 vertex_count;
    u32 *indices;
    s64 index_count;
};

bool LoadMeshFromObjFile (const char *filename, Mesh *mesh);

extern VkInstance g_vk_instance;
extern VkPhysicalDevice g_vk_physical_device;
extern int g_vk_graphics_queue_index;
extern int g_vk_present_queue_index;
extern VkDevice g_vk_device;
extern VkQueue g_vk_graphics_queue;
extern VkQueue g_vk_present_queue;
extern VkSurfaceKHR g_vk_surface;
extern VkSwapchainKHR g_vk_swapchain;
extern VkFormat g_vk_swapchain_image_format;
extern Array<VkImage> g_vk_swapchain_images;
extern Array<VkImageView> g_vk_swapchain_image_views;

bool GfxInitVulkan (GLFWwindow *window);

#endif
