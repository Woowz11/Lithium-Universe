#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Console.h";
#include "LibControl.h";
#include "BaseConstants.h";
#include "GlobalRender.h";
#include <vector>;

VkInstance vulkan;
VkDebugUtilsMessengerEXT debugMessenger;

#ifdef NDEBUG
const bool VulkanDebug = false;
#else
const bool VulkanDebug = true;
#endif

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

VkInstance GetVulkan() {
	return vulkan;
}

VkDebugUtilsMessengerEXT GetDebugMessanger() {
	return debugMessenger;
}

bool GetVulkanDebug() {
	return VulkanDebug;
}

std::vector<const char*> GetRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (VulkanDebug) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

bool CheckValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

void CheckAllValidationLayers() {
	if (VulkanDebug && !CheckValidationLayerSupport()) {
		Print("validation layers requested, but not available!");
	}
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

static VKAPI_ATTR VkBool32 VKAPI_CALL ErrorVulkanHook(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	std::string Error = pCallbackData->pMessage;
	Print("[VULKAN]: " + Error);

	return VK_FALSE;
}

VkResult CreateErrorVulkanHook(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyErrorVulkanHook(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = ErrorVulkanHook;
}

void SetupErrorVulkanHook() {
	if (!VulkanDebug) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	PopulateDebugMessengerCreateInfo(createInfo);

	VkResult CreateErrorVulkanHook_Result = CreateErrorVulkanHook(vulkan, &createInfo, nullptr, &debugMessenger);
	if (CreateErrorVulkanHook_Result != VK_SUCCESS) {
		Print("failed to set up debug messenger! (" + std::to_string(CreateErrorVulkanHook_Result) + ")");
	}
}

void CreateVulkan() {
	Print("Vulkan Extensions Supported: " + std::to_string(Initializate_Vulkan()));
	VkApplicationInfo Info = CreateVulkanInfo();
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &Info;

	std::vector<const char*> createInfoExtensions = {};

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	for (uint32_t i = 0; i < glfwExtensionCount; i++) {
		createInfoExtensions.push_back(glfwExtensions[i]);
	}

	if (VulkanDebug) {
		createInfoExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	createInfo.enabledExtensionCount = createInfoExtensions.size();
	createInfo.ppEnabledExtensionNames = createInfoExtensions.data();

	createInfo.enabledLayerCount = 0;

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (VulkanDebug) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	VkResult result = vkCreateInstance(&createInfo, nullptr, &vulkan);
	if (vkCreateInstance(&createInfo, nullptr, &vulkan) != VK_SUCCESS) {
		Print("failed to create vk instance!");
	}

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	Print("Available vulkan extensions:");
	std::vector<const char*> extensionNames;
	for (const auto& extension : extensions) {
		std::string exten = extension.extensionName;
		extensionNames.push_back(exten.c_str());
		Print("-> " + exten);
	}

	CheckAllValidationLayers();
	if (VulkanDebug) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionNames.size());
	createInfo.ppEnabledExtensionNames = extensionNames.data();
}