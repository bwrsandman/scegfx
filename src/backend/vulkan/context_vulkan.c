//
// Created by sandy on 21/03/19.
//

#include "context_vulkan.h"

#include <assert.h>

#include <SDL_vulkan.h>

const char* requested_instance_extension_names[] = {
#if SCEGFX_VALIDATION
  VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
};

const char* requested_instance_layer_names[] = {
#if SCEGFX_VALIDATION
  "VK_LAYER_LUNARG_standard_validation",
#endif
};

const char* requested_device_extension_names[] = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  VK_NV_RAY_TRACING_EXTENSION_NAME,
};

const char* requested_device_layer_names[] = {
  "VK_LAYER_LUNARG_standard_validation",
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
//#ifndef NDEBUG
VkBool32
scegfx_backend_vulkan_debug_callback(uint32_t severity,
                                     VkDebugReportObjectTypeEXT obj_type,
                                     uint64_t src_object,
                                     size_t location,
                                     int32_t msg_code,
                                     const char* layer_prefix,
                                     const char* msg,
                                     void* user_data)
{
#pragma clang diagnostic pop
  if (user_data == NULL || !msg || msg[0] == 0)
    return VK_FALSE;
  scegfx_context_vulkan_t* this = user_data;
  scegfx_debug_severity_t level = scegfx_debug_severity_info;
  if (severity & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
    level = scegfx_debug_severity_info;
  } else if (severity & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
    level = scegfx_debug_severity_warning;
  } else if (severity & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
    level = scegfx_debug_severity_warning;
  } else if (severity & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
    level = scegfx_debug_severity_error;
  } else if (severity & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
    level = scegfx_debug_severity_debug;
  }

  this->super.debug_callback(level, msg_code, layer_prefix, msg);

  return VK_FALSE;
}
//#endif

bool
register_extension(
  VkExtensionProperties* properties,
  uint32_t property_count,
  const char* name,
  uint32_t* extension_registry_count,
  char extension_registry[SCEGFX_VULKAN_MAX_EXTENSION_COUNT]
                         [SCEGFX_VULKAN_MAX_EXTENSION_STRING_SIZE])
{
  bool found = false;
  for (size_t j = 0; j < property_count; ++j) {
    if (strcmp(properties[j].extensionName, name) == 0) {
      found = true;
      assert(strlen(name) < SCEGFX_VULKAN_MAX_EXTENSION_STRING_SIZE);
      strncpy(extension_registry[(*extension_registry_count)++],
              name,
              SCEGFX_VULKAN_MAX_EXTENSION_STRING_SIZE);
      break;
    }
  }
  return found;
}

bool
register_layer(VkLayerProperties* properties,
               uint32_t properties_count,
               const char* name,
               uint32_t* layer_registry_count,
               char layer_registry[SCEGFX_VULKAN_MAX_LAYER_COUNT]
                                  [SCEGFX_VULKAN_MAX_LAYER_STRING_SIZE])
{
  bool found = false;
  for (size_t j = 0; j < properties_count; ++j) {
    if (strcmp(properties[j].layerName, name) == 0) {
      found = true;
      assert(strlen(name) < SCEGFX_VULKAN_MAX_LAYER_STRING_SIZE);
      strncpy(layer_registry[(*layer_registry_count)++],
              name,
              SCEGFX_VULKAN_MAX_LAYER_STRING_SIZE);
      break;
    }
  }
  return found;
}

bool
find_instance_required_extensions(scegfx_context_vulkan_t* this)
{
  // Zero out
  this->instance_extension_count = 0;
  memset(
    this->instance_extension_names, 0, sizeof(this->instance_extension_names));
  // Get available extensions
  uint32_t properties_count = SCEGFX_VULKAN_MAX_EXTENSION_COUNT;
  VkExtensionProperties properties[SCEGFX_VULKAN_MAX_EXTENSION_COUNT];
  VkResult result =
    vkEnumerateInstanceExtensionProperties(NULL, &properties_count, properties);
  if (result == VK_INCOMPLETE) {
    this->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "More instance extension properties than expected. "
      "Increase SCEGFX_VULKAN_MAX_EXTENSION_COUNT");
    return false;
  }
  // Select requested required extensions
  size_t required_extensions_count =
    sizeof(requested_instance_extension_names) /
    sizeof(requested_instance_extension_names[0]);
  for (size_t i = 0; i < required_extensions_count; ++i) {
    register_extension(properties,
                       properties_count,
                       requested_instance_extension_names[i],
                       &this->instance_extension_count,
                       this->instance_extension_names);
  }
  // Get required extensions from SDL
  uint32_t sdl_extension_properties_count = SCEGFX_VULKAN_MAX_EXTENSION_COUNT;
  const char* sdl_extension_names[SCEGFX_VULKAN_MAX_EXTENSION_COUNT];
  if (!SDL_Vulkan_GetInstanceExtensions(this->super.window_handle,
                                        &sdl_extension_properties_count,
                                        sdl_extension_names)) {
    this->super.debug_callback(
      scegfx_debug_severity_error, __LINE__, FILE_BASENAME, SDL_GetError());
    return false;
  }
  // Select requested required sdl extensions
  for (size_t i = 0; i < sdl_extension_properties_count; ++i) {
    register_extension(properties,
                       properties_count,
                       sdl_extension_names[i],
                       &this->instance_extension_count,
                       this->instance_extension_names);
  }
  return true;
}

bool
find_instance_required_layers(scegfx_context_vulkan_t* this)
{
  // Zero out
  this->instance_layer_count = 0;
  memset(this->instance_layer_names, 0, sizeof(this->instance_layer_names));
  // Get available layers
  uint32_t properties_count = SCEGFX_VULKAN_MAX_LAYER_COUNT;
  VkLayerProperties properties[SCEGFX_VULKAN_MAX_LAYER_COUNT];
  VkResult result =
    vkEnumerateInstanceLayerProperties(&properties_count, properties);
  if (result == VK_INCOMPLETE) {
    this->super.debug_callback(scegfx_debug_severity_error,
                               __LINE__,
                               FILE_BASENAME,
                               "More instance layer properties than expected. "
                               "Increase SCEGFX_VULKAN_MAX_LAYER_COUNT");
    return false;
  }
  // Select requested required layers
  size_t required_layer_count = sizeof(requested_instance_layer_names) /
                                sizeof(requested_instance_layer_names[0]);
  for (size_t i = 0; i < required_layer_count; ++i) {
    register_layer(properties,
                   properties_count,
                   requested_instance_layer_names[i],
                   &this->instance_layer_count,
                   this->instance_layer_names);
  }
  return true;
}

bool
find_device_required_extensions(scegfx_context_vulkan_t* this,
                                VkPhysicalDevice physical_device)
{
  // Zero out
  this->device_extension_count = 0;
  memset(this->device_extension_names, 0, sizeof(this->device_extension_names));
  // Get available extensions
  uint32_t properties_count = SCEGFX_VULKAN_MAX_EXTENSION_COUNT;
  VkExtensionProperties properties[SCEGFX_VULKAN_MAX_EXTENSION_COUNT];
  VkResult result = vkEnumerateDeviceExtensionProperties(
    physical_device, NULL, &properties_count, properties);
  if (result == VK_INCOMPLETE) {
    this->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "More device extension properties than expected. "
      "Increase SCEGFX_VULKAN_MAX_EXTENSION_COUNT");
    return false;
  }
  this->super.debug_callback(scegfx_debug_severity_info,
                             __LINE__,
                             FILE_BASENAME,
                             "Available extensions:");
  for (uint32_t i = 0; i < properties_count; ++i) {
    this->super.debug_callback(scegfx_debug_severity_info,
                               __LINE__,
                               FILE_BASENAME,
                               properties[i].extensionName);
  }
  // Select requested required extensions
  size_t required_extensions_count =
    sizeof(requested_device_extension_names) /
    sizeof(requested_device_extension_names[0]);
  for (size_t i = 0; i < required_extensions_count; ++i) {
    register_extension(properties,
                       properties_count,
                       requested_device_extension_names[i],
                       &this->device_extension_count,
                       this->device_extension_names);
  }
  return true;
}

bool
find_device_required_layers(scegfx_context_vulkan_t* this,
                            VkPhysicalDevice physical_device)
{
  // Zero out
  this->device_layer_count = 0;
  memset(this->device_layer_names, 0, sizeof(this->device_layer_names));
  // Get available layers
  uint32_t properties_count = SCEGFX_VULKAN_MAX_LAYER_COUNT;
  VkLayerProperties properties[SCEGFX_VULKAN_MAX_LAYER_COUNT];
  VkResult result = vkEnumerateDeviceLayerProperties(
    physical_device, &properties_count, properties);
  if (result == VK_INCOMPLETE) {
    this->super.debug_callback(scegfx_debug_severity_error,
                               __LINE__,
                               FILE_BASENAME,
                               "More device layer properties than expected. "
                               "Increase SCEGFX_VULKAN_MAX_LAYER_COUNT");
    return false;
  }
  this->super.debug_callback(
    scegfx_debug_severity_info, __LINE__, FILE_BASENAME, "Available layers:");
  for (uint32_t i = 0; i < properties_count; ++i) {
    this->super.debug_callback(scegfx_debug_severity_info,
                               __LINE__,
                               FILE_BASENAME,
                               properties[i].layerName);
  }
  // Select requested required layers
  size_t required_layer_count = sizeof(requested_device_layer_names) /
                                sizeof(requested_device_layer_names[0]);
  for (size_t i = 0; i < required_layer_count; ++i) {
    register_layer(properties,
                   properties_count,
                   requested_device_layer_names[i],
                   &this->device_layer_count,
                   this->device_layer_names);
  }
  return true;
}

bool
scegfx_context_vulkan_create_instance(scegfx_context_vulkan_t* this)
{
  if (!find_instance_required_extensions(this)) {
    return false;
  }
  if (!find_instance_required_layers(this)) {
    return false;
  }
  // Convert to char pointers
  const char* extension_names[SCEGFX_VULKAN_MAX_EXTENSION_COUNT];
  const char* layer_names[SCEGFX_VULKAN_MAX_LAYER_COUNT];
  for (size_t i = 0; i < this->instance_extension_count; ++i) {
    extension_names[i] = this->instance_extension_names[i];
  }
  for (size_t i = 0; i < this->instance_layer_count; ++i) {
    layer_names[i] = this->instance_layer_names[i];
  }
  const VkApplicationInfo app_info = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pEngineName = "SCarterEngine",
    .engineVersion = VK_MAKE_VERSION(0, 1, 0),
    .pApplicationName = "SCarterEngine",
    .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
    .apiVersion = VK_API_VERSION_1_1,
  };
  //#ifndef NDEBUG
  VkDebugReportCallbackCreateInfoEXT debug_info = {
    .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
    .flags = VK_DEBUG_REPORT_DEBUG_BIT_EXT |
             VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
             VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
             VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT,
    .pfnCallback = scegfx_backend_vulkan_debug_callback,
    .pUserData = (void*)this,
  };
  //#endif
  VkInstanceCreateInfo info = {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    //#ifndef NDEBUG
    .pNext = &debug_info,
    //#endif
    .pApplicationInfo = &app_info,
    .enabledExtensionCount = this->instance_extension_count,
    .ppEnabledExtensionNames = extension_names,
    .enabledLayerCount = this->instance_layer_count,
    .ppEnabledLayerNames = layer_names,
  };
  VkResult result = vkCreateInstance(&info, NULL, &this->instance);
  if (result != VK_SUCCESS) {
    this->super.debug_callback(scegfx_debug_severity_error,
                               __LINE__,
                               FILE_BASENAME,
                               "Failed to create vulkan instance");
    return false;
  }
  return true;
}

bool
get_instance_procs(scegfx_context_vulkan_t* this)
{
  typedef PFN_vkVoidFunction (*PFN_vkGetInstanceProcAddr)(VkInstance instance,
                                                          const char* pName);
  PFN_vkGetInstanceProcAddr GetInstanceProcAddr =
    (PFN_vkGetInstanceProcAddr)SDL_Vulkan_GetVkGetInstanceProcAddr();
  this->functions.GetPhysicalDeviceSurfaceFormats =
    (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)GetInstanceProcAddr(
      this->instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
  if (this->functions.GetPhysicalDeviceSurfaceFormats == NULL) {
    this->super.debug_callback(
      scegfx_debug_severity_error,
      __LINE__,
      FILE_BASENAME,
      "Could not get instance proc: vkGetPhysicalDeviceSurfaceFormatsKHR");
    return false;
  }
  return true;
}

void
destroy_instance(scegfx_context_vulkan_t* this)
{
  vkDestroyInstance(this->instance, NULL);
}

VkPhysicalDevice
choose_physical_device(scegfx_context_vulkan_t* this)
{
  VkPhysicalDevice devices[2];
  uint32_t device_count = sizeof(devices) / sizeof(devices[0]);
  VkResult res =
    vkEnumeratePhysicalDevices(this->instance, &device_count, devices);
  assert(res == VK_SUCCESS);
  if (res == VK_SUCCESS)
    return devices[0];
  else
    return NULL;
}

bool
create_surface(scegfx_context_vulkan_t* this, VkPhysicalDevice physical_device)
{
  VkResult res;
  SDL_Vulkan_CreateSurface(
    this->super.window_handle, this->instance, &this->surface);
  if (!this->surface) {
    this->super.debug_callback(
      scegfx_debug_severity_error, __LINE__, FILE_BASENAME, SDL_GetError());
    return false;
  }
  VkSurfaceFormatKHR surface_formats[32];
  uint32_t surface_formats_count =
    sizeof(surface_formats) / sizeof(surface_formats[0]);
  res = this->functions.GetPhysicalDeviceSurfaceFormats(
    physical_device, this->surface, &surface_formats_count, surface_formats);
  if (res != VK_SUCCESS || surface_formats_count == 0) {
    this->super.debug_callback(scegfx_debug_severity_error,
                               __LINE__,
                               FILE_BASENAME,
                               "Unable to get surface formats");
    return false;
  }
  this->surface_format = surface_formats[0];
  res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
    physical_device, this->surface, &this->surface_capabilities);
  if (res != VK_SUCCESS || surface_formats_count == 0) {
    this->super.debug_callback(scegfx_debug_severity_error,
                               __LINE__,
                               FILE_BASENAME,
                               "Unable to get surface capabilities");
    return false;
  }
  this->surface_present_mode_count =
    SCEGFX_VULKAN_MAX_SURFACE_PRESENT_MODES_COUNT;
  res =
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device,
                                              this->surface,
                                              &this->surface_present_mode_count,
                                              this->surface_present_modes);
  if (res != VK_SUCCESS || surface_formats_count == 0) {
    this->super.debug_callback(scegfx_debug_severity_error,
                               __LINE__,
                               FILE_BASENAME,
                               "Unable to get surface capabilities");
    return false;
  }
  return true;
}

void
destroy_surface(scegfx_context_vulkan_t* this)
{
  vkDestroySurfaceKHR(this->instance, this->surface, NULL);
}

bool
create_device(scegfx_context_vulkan_t* this, VkPhysicalDevice physical_device)
{
  find_device_required_extensions(this, physical_device);
  find_device_required_layers(this, physical_device);
  // Convert to char pointers
  const char* extensions_names[SCEGFX_VULKAN_MAX_EXTENSION_COUNT];
  const char* layer_names[SCEGFX_VULKAN_MAX_LAYER_COUNT];
  for (size_t i = 0; i < this->device_extension_count; ++i) {
    extensions_names[i] = this->device_extension_names[i];
  }
  for (size_t i = 0; i < this->device_layer_count; ++i) {
    layer_names[i] = this->device_layer_names[i];
  }

  VkFormat candidate_depth_formats[3] = {
    VK_FORMAT_D32_SFLOAT,
    VK_FORMAT_D32_SFLOAT_S8_UINT,
    VK_FORMAT_D24_UNORM_S8_UINT,
  };
  bool found_depth_format = false;
  for (uint32_t i = 0;
       i < sizeof(candidate_depth_formats) / sizeof(candidate_depth_formats[0]);
       ++i) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(
      physical_device, candidate_depth_formats[i], &props);
    if ((props.optimalTilingFeatures &
         VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0) {
      this->depth_format = candidate_depth_formats[i];
      found_depth_format = true;
      break;
    }
  }

  if (!found_depth_format) {
    this->super.debug_callback(scegfx_debug_severity_error,
                               __LINE__,
                               FILE_BASENAME,
                               "Unable to find supported depth format");
    return false;
  }

  VkQueueFamilyProperties queue_family_properties[64];
  VkBool32 supports_present[64];
  uint32_t queue_family_properties_count =
    sizeof(queue_family_properties) / sizeof(queue_family_properties[0]);
  vkGetPhysicalDeviceQueueFamilyProperties(
    physical_device, &queue_family_properties_count, queue_family_properties);
  for (uint32_t i = 0; i < queue_family_properties_count; ++i) {
    vkGetPhysicalDeviceSurfaceSupportKHR(
      physical_device, i, this->surface, &supports_present[i]);
  }
  // Search for a graphics and a present queue in the array of queue
  // families, try to find one that supports both
  uint32_t present_queue_family_index = UINT32_MAX;
  uint32_t graphics_queue_family_index = UINT32_MAX;
  uint32_t compute_queue_family_index = UINT32_MAX;
  uint32_t transfer_queue_family_index = UINT32_MAX;
  uint32_t sparse_binding_queue_family_index = UINT32_MAX;
  uint32_t protected_memory_queue_family_index = UINT32_MAX;
  for (uint32_t i = 0; i < queue_family_properties_count; i++) {
    if (present_queue_family_index == UINT32_MAX &&
        queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      if (graphics_queue_family_index == UINT32_MAX) {
        graphics_queue_family_index = i;
      }
      if (supports_present[i] == VK_TRUE) {
        graphics_queue_family_index = i;
        present_queue_family_index = i;
      }
    }
    if (compute_queue_family_index == UINT32_MAX &&
        queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
      compute_queue_family_index = i;
    }
    if (transfer_queue_family_index == UINT32_MAX &&
        queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
      transfer_queue_family_index = i;
    }
    if (sparse_binding_queue_family_index == UINT32_MAX &&
        queue_family_properties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
      sparse_binding_queue_family_index = i;
    }
    if (protected_memory_queue_family_index == UINT32_MAX &&
        queue_family_properties[i].queueFlags & VK_QUEUE_PROTECTED_BIT) {
      protected_memory_queue_family_index = i;
    }
  }
  assert(graphics_queue_family_index < queue_family_properties_count);
  assert(graphics_queue_family_index != UINT32_MAX &&
         present_queue_family_index != UINT32_MAX &&
         compute_queue_family_index != UINT32_MAX &&
         transfer_queue_family_index != UINT32_MAX);
  // Make sure we are using the same queue for graphics, present, transfer
  // and compute.
  assert(present_queue_family_index == graphics_queue_family_index &&
         present_queue_family_index == compute_queue_family_index &&
         present_queue_family_index == transfer_queue_family_index);

  // Create device queue info
  float queue_priorities[1] = {
    0.0f,
  };
  uint32_t queue_priorities_count =
    sizeof(queue_priorities) / sizeof(queue_priorities[0]);
  VkDeviceQueueCreateInfo queueInfo = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
    .queueFamilyIndex = graphics_queue_family_index,
    .queueCount = queue_priorities_count,
    .pQueuePriorities = queue_priorities,
  };
  VkDeviceCreateInfo info = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .enabledExtensionCount = this->device_extension_count,
    .ppEnabledExtensionNames = extensions_names,
    .enabledLayerCount = this->device_layer_count,
    .ppEnabledLayerNames = layer_names,
    .queueCreateInfoCount = 1,
    .pQueueCreateInfos = &queueInfo,
  };
  VkResult res = vkCreateDevice(physical_device, &info, NULL, &this->device);
  if (res != VK_SUCCESS) {
    this->super.debug_callback(scegfx_debug_severity_error,
                               __LINE__,
                               FILE_BASENAME,
                               "Unable to create logical device");
    return false;
  }
  this->present_graphics_compute_transfer_queue_index =
    graphics_queue_family_index;
  vkGetDeviceQueue(this->device,
                   this->present_graphics_compute_transfer_queue_index,
                   0,
                   &this->present_graphics_compute_transfer_queue);
  return true;
}

void
destroy_device(scegfx_context_vulkan_t* this)
{
  vkDestroyDevice(this->device, NULL);
}

bool
create_command_pool(scegfx_context_vulkan_t* this)
{
  VkResult res;
  VkCommandPoolCreateInfo info = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .queueFamilyIndex = this->present_graphics_compute_transfer_queue_index,
  };
  res = vkCreateCommandPool(this->device, &info, NULL, &this->command_pool);
  if (res != VK_SUCCESS) {
    this->super.debug_callback(scegfx_debug_severity_error,
                               __LINE__,
                               FILE_BASENAME,
                               "Unable to create command pool");
    return false;
  }

  return true;
}

void
destroy_command_pool(scegfx_context_vulkan_t* this)
{
  vkDestroyCommandPool(this->device, this->command_pool, NULL);
}

bool
scegfx_context_vulkan_initialize(scegfx_context_t* super)
{
  assert(!super->initialized);
  scegfx_context_vulkan_t* this = (scegfx_context_vulkan_t*)super;
  if (!scegfx_context_vulkan_create_instance(this)) {
    return false;
  }
  if (!get_instance_procs(this)) {
    return false;
  }
  VkPhysicalDevice physical_device = choose_physical_device(this);
  if (!create_surface(this, physical_device)) {
    return false;
  }
  if (!create_device(this, physical_device)) {
    return false;
  }
  if (!create_command_pool(this)) {
    return false;
  }
  super->initialized = true;
  return true;
}

void
scegfx_context_vulkan_terminate(scegfx_context_t* super)
{
  assert(super->initialized);
  scegfx_context_vulkan_t* this = (scegfx_context_vulkan_t*)super;
  destroy_command_pool(this);
  destroy_device(this);
  destroy_surface(this);
  destroy_instance(this);
  super->initialized = false;
}

bool
scegfx_context_vulkan_make_current(scegfx_context_t* super)
{
  assert(super->initialized);
  return true;
}
