#ifndef SCOP_VULKAN_H
#define SCOP_VULKAN_H

#define SCOP_BACKEND_NAME "OpenGL 3.3"

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

#endif
