#include "Scop_Core.h"
#include "Scop_Graphics.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

VkInstance g_vk_instance {};
VkPhysicalDevice g_vk_physical_device = VK_NULL_HANDLE;
int g_vk_graphics_queue_index = -1;
VkDevice g_vk_device = VK_NULL_HANDLE;
VkQueue g_vk_graphics_queue = VK_NULL_HANDLE;

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

static bool InitVulkanInstance (GLFWwindow *window)
{
    if (!CheckVulkanValidationLayerSupport ())
        return false;

    VkApplicationInfo app_info {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Scop Vk";
    app_info.applicationVersion = VK_MAKE_API_VERSION (0, 1, 0, 0);
    app_info.pEngineName = "Scop Engine";
    app_info.engineVersion = VK_MAKE_API_VERSION (0, 1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    u32 glfw_extension_count = 0;
    const char **glfw_extensions = glfwGetRequiredInstanceExtensions (&glfw_extension_count);

    Array<const char *> extensions {};
    ArrayReserve (&extensions, glfw_extension_count + 2);

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

    if (vkCreateInstance (&create_info, null, &g_vk_instance) != VK_SUCCESS)
        return false;

    VkDebugUtilsMessengerEXT messenger {};

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr (g_vk_instance, "vkCreateDebugUtilsMessengerEXT");
    if (!func)
    {
        printf ("Could not load vkCreateDebugUtilsMessengerEXT\n");
        return false;
    }

    auto messenger_result = func (g_vk_instance, &debug_create_info, null, &messenger);
    if (messenger_result != VK_SUCCESS)
    {
        printf ("vkCreateDebugUtilsMessengerEXT returned %h\n", messenger_result);
        return false;
    }

    printf ("Initialized Vulkan instance\n");

    return true;
}

static void PrintVulkanDeviceProperties (
    VkPhysicalDeviceProperties props,
    VkPhysicalDeviceFeatures feats
)
{
    printf ("Device %s, general information:\n", props.deviceName);
    printf ("  apiVersion: %u.%u.%u.%u\n",
        VK_API_VERSION_MAJOR (props.apiVersion),
        VK_API_VERSION_MINOR (props.apiVersion),
        VK_API_VERSION_PATCH (props.apiVersion),
        VK_API_VERSION_VARIANT (props.apiVersion)
    );
    printf ("  driverVersion: %u\n", props.driverVersion);
    printf ("  vendorID: %u\n", props.vendorID);
    printf ("  deviceID: %u\n", props.deviceID);

    switch (props.deviceType)
    {
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        printf ("  deviceType: OTHER\n"); break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        printf ("  deviceType: INTEGRATED_GPU\n"); break;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        printf ("  deviceType: DISCRETE_GPU\n"); break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        printf ("  deviceType: VIRTUAL_GPU\n"); break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        printf ("  deviceType: CPU\n"); break;
    }

    printf ("  limits.maxImageDimension2D: %u\n", props.limits.maxImageDimension2D);
    printf ("  limits.maxImageDimension3D: %u\n", props.limits.maxImageDimension3D);
    printf ("  limits.maxImageDimensionCube: %u\n", props.limits.maxImageDimensionCube);
    printf ("  limits.maxImageArrayLayers: %u\n", props.limits.maxImageArrayLayers);
    printf ("  limits.maxUniformBufferRange: %u\n", props.limits.maxUniformBufferRange);
    printf ("  limits.maxStorageBufferRange: %u\n", props.limits.maxStorageBufferRange);
    printf ("  Supported features:\n");
    printf ("    geometryShader: %s\n", feats.geometryShader ? "true" : "false");
    printf ("    shaderFloat64: %s\n", feats.shaderFloat64 ? "true" : "false");
}

static int GetVulkanDeviceSuitabilityScore (VkPhysicalDevice device, int *graphics_queue_index)
{
    int score = 1;

    VkPhysicalDeviceProperties device_properties {};
    vkGetPhysicalDeviceProperties (device, &device_properties);

    VkPhysicalDeviceFeatures device_features {};
    vkGetPhysicalDeviceFeatures (device, &device_features);

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties (device, &queue_family_count, null);

    VkQueueFamilyProperties *queue_families = (VkQueueFamilyProperties *)malloc (sizeof (VkQueueFamilyProperties) * queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties (device, &queue_family_count, queue_families);

    *graphics_queue_index = -1;
    for (int i = 0; i < queue_family_count; i += 1)
    {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            *graphics_queue_index = i;
            break;
        }
    }

    if (*graphics_queue_index == -1)
    {
        PrintVulkanDeviceProperties (device_properties, device_features);
        printf ("  Device is unsuitable\n\n", score);

        return 0;
    }

    if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 1000;

    PrintVulkanDeviceProperties (device_properties, device_features);
    printf ("  Suitability score: %d\n\n", score);

    return score;
}

static bool ChooseVulkanPhysicalDevice ()
{
    VkPhysicalDevice device = VK_NULL_HANDLE;

    u32 device_count = 0;
    vkEnumeratePhysicalDevices (g_vk_instance, &device_count, null);

    if (device_count == 0)
    {
        printf ("Could not find a device that supports Vulkan\n");
        return false;
    }

    VkPhysicalDevice *available_devices = (VkPhysicalDevice *)malloc (sizeof (VkPhysicalDevice) * device_count);
    vkEnumeratePhysicalDevices (g_vk_instance, &device_count, available_devices);

    printf ("Found %u devices that support Vulkan\n", device_count);

    int suitability_score = 0;
    for (int i = 0; i < device_count; i += 1)
    {
        int graphics_queue_index;
        int score = GetVulkanDeviceSuitabilityScore (available_devices[i], &graphics_queue_index);
        if (score > suitability_score)
        {
            device = available_devices[i];
            g_vk_graphics_queue_index = graphics_queue_index;
            suitability_score = score;
        }
    }

    if (device == VK_NULL_HANDLE)
    {
        printf ("Could not find a suitable Vulkan device\n");
        return false;
    }

    VkPhysicalDeviceProperties device_properties {};
    vkGetPhysicalDeviceProperties (device, &device_properties);

    printf ("Picked device %s\n", device_properties.deviceName);

    g_vk_physical_device = device;

    return true;
}

static bool InitVulkanDeviceAndQueues ()
{
    VkDeviceQueueCreateInfo queue_create_info {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = g_vk_graphics_queue_index;
    queue_create_info.queueCount = 1;
    float queue_priority = 1;
    queue_create_info.pQueuePriorities = &queue_priority;

    VkPhysicalDeviceFeatures required_features {};

    VkDeviceCreateInfo create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = &queue_create_info;
    create_info.queueCreateInfoCount = 1;

    create_info.pEnabledFeatures = &required_features;

    create_info.enabledExtensionCount = 0;
    create_info.enabledLayerCount = StaticArraySize (Vulkan_Validation_Layers);
    create_info.ppEnabledLayerNames = Vulkan_Validation_Layers;

    if (vkCreateDevice (g_vk_physical_device, &create_info, null, &g_vk_device) != VK_SUCCESS)
    {
        printf ("Could not create Vulkan logical device\n");
        return false;
    }

    vkGetDeviceQueue (g_vk_device, g_vk_graphics_queue_index, 0, &g_vk_graphics_queue);

    printf ("Created Vulkan logicial device and retrieved graphics queue handle\n");

    return true;
}

bool GfxInitVulkan (GLFWwindow *window)
{
    if (!InitVulkanInstance (window))
        return false;

    if (!ChooseVulkanPhysicalDevice ())
        return false;

    if (!InitVulkanDeviceAndQueues ())
        return false;

    return true;
}
