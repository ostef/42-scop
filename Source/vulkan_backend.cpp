#include "Scop_Core.h"
#include "Scop_Graphics.h"

VkInstance g_vk_instance {};
VkPhysicalDevice g_vk_physical_device = VK_NULL_HANDLE;
int g_vk_graphics_queue_index = -1;
int g_vk_present_queue_index = -1;
VkDevice g_vk_device = VK_NULL_HANDLE;
VkQueue g_vk_graphics_queue = VK_NULL_HANDLE;
VkQueue g_vk_present_queue = VK_NULL_HANDLE;
VkSurfaceKHR g_vk_surface = VK_NULL_HANDLE;
VkSwapchainKHR g_vk_swapchain = VK_NULL_HANDLE;
VkFormat g_vk_swapchain_image_format {};
Array<VkImage> g_vk_swapchain_images {};
Array<VkImageView> g_vk_swapchain_image_views {};

static const char *Vulkan_Validation_Layers[] = {
    "VK_LAYER_KHRONOS_validation"
};

static const char *Vulkan_Instance_Extensions[] = {
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
};

static const char *Vulkan_Device_Extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
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
    ArrayReserve (&extensions, glfw_extension_count + StaticArraySize (Vulkan_Instance_Extensions));

    for (int i = 0; i < glfw_extension_count; i += 1)
        ArrayPush (&extensions, glfw_extensions[i]);

    for (int i = 0; i < StaticArraySize (Vulkan_Instance_Extensions); i += 1)
        ArrayPush (&extensions, Vulkan_Instance_Extensions[i]);

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

    printf ("Creating Vulkan instance with validation layers:\n");
    for (int i = 0; i < StaticArraySize (Vulkan_Validation_Layers); i += 1)
        printf ("  %s\n", Vulkan_Validation_Layers[i]);

    printf ("Extensions:\n");
    for (int i = 0; i < extensions.count; i += 1)
        printf ("  %s\n", extensions[i]);

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

static bool VulkanPhysicalDeviceSupportsExtensions (VkPhysicalDevice device)
{
    u32 extension_count = 0;
    vkEnumerateDeviceExtensionProperties (device, null, &extension_count, null);

    VkExtensionProperties *extensions = (VkExtensionProperties *)malloc (sizeof (VkExtensionProperties) * extension_count);
    vkEnumerateDeviceExtensionProperties (device, null, &extension_count, extensions);

    for (int i = 0; i < StaticArraySize (Vulkan_Device_Extensions); i += 1)
    {
        bool found = false;
        for (int j = 0; j < extension_count; j += 1)
        {
            if (strcmp (Vulkan_Device_Extensions[i], extensions[j].extensionName) == 0)
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

struct VulkanSwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR caps;
    Array<VkSurfaceFormatKHR> formats;
    Array<VkPresentModeKHR> present_modes;
};

VulkanSwapChainSupportDetails QueryVulkanSwapChainSupportDetails (VkPhysicalDevice device)
{
    VulkanSwapChainSupportDetails details {};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR (device, g_vk_surface, &details.caps);

    u32 format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR (device, g_vk_surface, &format_count, null);

    ArrayReserve (&details.formats, format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR (device, g_vk_surface, &format_count, details.formats.data);
    details.formats.count = format_count;

    u32 present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR (device, g_vk_surface, &present_mode_count, null);

    ArrayReserve (&details.present_modes, present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR (device, g_vk_surface, &present_mode_count, details.present_modes.data);
    details.present_modes.count = present_mode_count;

    return details;
}

static int GetVulkanDeviceSuitabilityScore (VkPhysicalDevice device, int *graphics_queue_index, int *present_queue_index)
{
    Assert (graphics_queue_index != null && present_queue_index != null, "Null parameters");

    int score = 1;

    VkPhysicalDeviceProperties device_properties {};
    vkGetPhysicalDeviceProperties (device, &device_properties);

    VkPhysicalDeviceFeatures device_features {};
    vkGetPhysicalDeviceFeatures (device, &device_features);

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties (device, &queue_family_count, null);

    VkQueueFamilyProperties *queue_families = (VkQueueFamilyProperties *)malloc (sizeof (VkQueueFamilyProperties) * queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties (device, &queue_family_count, queue_families);

    if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score += 1000;

    *graphics_queue_index = -1;
    *present_queue_index = -1;
    for (int i = 0; i < queue_family_count; i += 1)
    {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            *graphics_queue_index = i;
        }

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR (device, i, g_vk_surface, &present_support);

        if (present_support)
            *present_queue_index = i;
    }

    if (*graphics_queue_index == -1 || *present_queue_index == -1)
        score = 0;

    if (!VulkanPhysicalDeviceSupportsExtensions (device))
        score = 0;

    auto swap_chain_support = QueryVulkanSwapChainSupportDetails (device);
    if (swap_chain_support.formats.count == 0 || swap_chain_support.present_modes.count == 0)
        score = 0;

    PrintVulkanDeviceProperties (device_properties, device_features);

    if (score == 0)
        printf ("  Device is unsuitable\n\n");
    else
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
        int graphics_queue_index, present_queue_index;
        int score = GetVulkanDeviceSuitabilityScore (available_devices[i], &graphics_queue_index, &present_queue_index);
        if (score > suitability_score)
        {
            device = available_devices[i];
            g_vk_graphics_queue_index = graphics_queue_index;
            g_vk_present_queue_index = present_queue_index;
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
    VkDeviceQueueCreateInfo queues_create_info[2] = {};
    // Graphics queue
    {
        VkDeviceQueueCreateInfo *info = &queues_create_info[0];
        info->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        info->queueFamilyIndex = g_vk_graphics_queue_index;
        info->queueCount = 1;
        float queue_priority = 1;
        info->pQueuePriorities = &queue_priority;
    }
    // Present queue
    {
        VkDeviceQueueCreateInfo *info = &queues_create_info[1];
        info->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        info->queueFamilyIndex = g_vk_present_queue_index;
        info->queueCount = 1;
        float queue_priority = 1;
        info->pQueuePriorities = &queue_priority;
    }

    VkPhysicalDeviceFeatures required_features {};

    VkDeviceCreateInfo create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = queues_create_info;
    create_info.queueCreateInfoCount = StaticArraySize (queues_create_info);

    create_info.pEnabledFeatures = &required_features;

    create_info.enabledExtensionCount = StaticArraySize (Vulkan_Device_Extensions);
    create_info.ppEnabledExtensionNames = Vulkan_Device_Extensions;
    create_info.enabledLayerCount = StaticArraySize (Vulkan_Validation_Layers);
    create_info.ppEnabledLayerNames = Vulkan_Validation_Layers;

    if (vkCreateDevice (g_vk_physical_device, &create_info, null, &g_vk_device) != VK_SUCCESS)
    {
        printf ("Could not create Vulkan logical device\n");
        return false;
    }

    vkGetDeviceQueue (g_vk_device, g_vk_graphics_queue_index, 0, &g_vk_graphics_queue);
    if (g_vk_graphics_queue == VK_NULL_HANDLE)
    {
        printf ("Could not get graphics queue from logical device\n");
        return false;
    }

    vkGetDeviceQueue (g_vk_device, g_vk_present_queue_index, 0, &g_vk_present_queue);
    if (g_vk_present_queue == VK_NULL_HANDLE)
    {
        printf ("Could not get present queue from logical device\n");
        return false;
    }

    printf ("Created Vulkan logicial device and retrieved queue handles\n");

    return true;
}

static VkSurfaceFormatKHR ChooseSwapSurfaceFormat (const Array<VkSurfaceFormatKHR> &available_formats)
{
    int highest_score = 0;
    int highest_score_index = 0;

    for (int i = 0; i < available_formats.count; i += 1)
    {
        auto format = available_formats[i];

        int score = 0;

        if (format.format == VK_FORMAT_B8G8R8A8_SRGB)
            score += 3;

        if (format.format == VK_FORMAT_B8G8R8_SRGB)
            score += 2;

        if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            score += 1;

        if (score > highest_score)
        {
            highest_score = score;
            highest_score_index = i;
        }
    }

    return available_formats[highest_score_index];
}

static VkPresentModeKHR ChooseSwapPresentMode (const Array<VkPresentModeKHR > &available_modes)
{
    for (int i = 0; i < available_modes.count; i += 1)
    {
        auto mode = available_modes[i];

        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return VK_PRESENT_MODE_MAILBOX_KHR;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D ChooseSwapExtent (GLFWwindow *window, const VkSurfaceCapabilitiesKHR &caps)
{
    if (caps.currentExtent.width != 0xffffffff)
        return caps.currentExtent;

    int width, height;
    glfwGetFramebufferSize (window, &width, &height);

    VkExtent2D actualExtent = {
        (u32)width,
        (u32)height
    };

    actualExtent.width = Clamp (actualExtent.width, caps.minImageExtent.width, caps.maxImageExtent.width);
    actualExtent.height = Clamp (actualExtent.height, caps.minImageExtent.height, caps.maxImageExtent.height);

    return actualExtent;
}

static bool CreateVulkanSwapChain (GLFWwindow *window)
{
    auto support = QueryVulkanSwapChainSupportDetails (g_vk_physical_device);

    auto surface_format = ChooseSwapSurfaceFormat (support.formats);
    auto present_mode = ChooseSwapPresentMode (support.present_modes);
    auto extent = ChooseSwapExtent (window, support.caps);

    g_vk_swapchain_image_format = surface_format.format;

    u32 image_count = Min (support.caps.minImageCount + 1, support.caps.maxImageCount);

    VkSwapchainCreateInfoKHR create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = g_vk_surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    u32 queue_family_indices[] = {(u32)g_vk_graphics_queue_index, (u32)g_vk_present_queue_index};
    if (g_vk_graphics_queue_index != g_vk_present_queue_index)
    {
        // We don't want to handle ownership of images ourselves for simplicity
        // at the moment so set the sharing mode to concurrent
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else
    {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0; // Optional
        create_info.pQueueFamilyIndices = null; // Optional
    }

    // Setting this to currentTransform means no transform is applied
    create_info.preTransform = support.caps.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;

    create_info.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR (g_vk_device, &create_info, null, &g_vk_swapchain) != VK_SUCCESS)
    {
        printf ("Could not create swap chain\n");
        return false;
    }

    {
        u32 image_count = 0;
        vkGetSwapchainImagesKHR (g_vk_device, g_vk_swapchain, &image_count, null);

        ArrayReserve (&g_vk_swapchain_images, image_count);
        vkGetSwapchainImagesKHR (g_vk_device, g_vk_swapchain, &image_count, g_vk_swapchain_images.data);
        g_vk_swapchain_images.count = image_count;
    }

    printf ("Created Vulkan swap chain\n");

    return true;
}

static bool CreateVulkanImageViews ()
{
    ArrayReserve (&g_vk_swapchain_image_views, g_vk_swapchain_images.count);

    for (int i = 0; i < g_vk_swapchain_images.count; i += 1)
    {
        VkImageViewCreateInfo create_info {};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = g_vk_swapchain_images[i];
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = g_vk_swapchain_image_format;

        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;

        auto view = ArrayPush (&g_vk_swapchain_image_views);
        if (vkCreateImageView (g_vk_device, &create_info, null, view) != VK_SUCCESS)
        {
            printf ("Could not create swap chain image view for iage %d\n", i);
            return false;
        }
    }

    printf ("Created swap chain image views\n");

    return true;
}

bool GfxInitVulkan (GLFWwindow *window)
{
    if (!InitVulkanInstance (window))
        return false;

    if (glfwCreateWindowSurface (g_vk_instance, window, null, &g_vk_surface) != VK_SUCCESS)
    {
        printf ("Could not create Vulkan window surface\n");
        return false;
    }

    printf ("Created Vulkan window surface\n");

    if (!ChooseVulkanPhysicalDevice ())
        return false;

    if (!InitVulkanDeviceAndQueues ())
        return false;

    if (!CreateVulkanSwapChain (window))
        return false;

    if (!CreateVulkanImageViews ())
        return false;

    printf ("\n=== Fully initialized Vulkan backend. Yipee! ===\n");

    return true;
}
