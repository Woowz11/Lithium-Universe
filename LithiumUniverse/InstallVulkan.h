#pragma once
#include <vulkan/vulkan.h>

VkInstance GetVulkan();
VkDebugUtilsMessengerEXT GetDebugMessanger();
void CreateVulkan();
bool GetVulkanDebug();
void DestroyErrorVulkanHook(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);