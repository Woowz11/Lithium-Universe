#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "GlobalRender.h";

int Initializate_GLFW() {
	return glfwInit();
}

void Terminate_GLFW() {
	glfwTerminate();
}

uint32_t Initializate_Vulkan() {
	uint32_t vk_ec = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &vk_ec, nullptr);
	return vk_ec;
}

void Terminate_Vulkan() {
	vkDestroyInstance(GetVulkan(), nullptr);
}