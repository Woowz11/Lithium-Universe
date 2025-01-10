#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Console.h";
#include "LibControl.h";
#include "BaseConstants.h";
#include "GlobalRender.h";

VkInstance vulkan;

VkInstance GetVulkan() {
	return vulkan;
}

VkApplicationInfo CreateVulkanInfo() {
	VkApplicationInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	Info.pApplicationName = "LithiumUniverse";
	Info.applicationVersion = VK_MAKE_VERSION(GameVersionMajor, GameVersionMinor, GameVersionPatch);
	Info.pEngineName = "No Engine";
	Info.engineVersion = VK_MAKE_VERSION(GameVersionMajor, GameVersionMinor, GameVersionPatch);
	Info.apiVersion = VK_API_VERSION_1_0;
	return Info;
}

void CreateVulkan() {
	Print("Vulkan Extensions Supported: " + std::to_string(Initializate_Vulkan()));
	VkApplicationInfo Info = CreateVulkanInfo();
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &Info;

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	createInfo.enabledLayerCount = 0;

	VkResult result = vkCreateInstance(&createInfo, nullptr, &vulkan);
	if (vkCreateInstance(&createInfo, nullptr, &vulkan) != VK_SUCCESS) {
		Print("failed to create instance!");
	}
}

void Render() {
	
}