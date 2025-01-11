#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <string>
#include <vector>;
#include "Console.h";
#include "BaseConstants.h";

enum GameInstallError {
	SUCCESS = 0,
	GLFW_NOT_CREATE_WINDOW = 1
};

class GameInstalls {
public:
#ifdef NDEBUG
	const bool DeveloperVersion = false;
#else
	const bool DeveloperVersion = true;
#endif

	GLFWwindow* Window = NULL;
	VkInstance Vulkan = nullptr;
	VkDebugUtilsMessengerEXT VulkanErrorHook;

	void Run() {
		RunAll();
		if (Error==GameInstallError::SUCCESS) {
			Loop();
		}
		DestroyAll();
	}

private:
	GameInstallError Error = GameInstallError::SUCCESS;

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	/* Отправка сообщения об ошибки от Vulkan */
	static VKAPI_ATTR VkBool32 VKAPI_CALL ErrorVulkanHook(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		std::string Error = pCallbackData->pMessage;
		Print("[VULKAN]: " + Error);

		return VK_FALSE;
	}
	
	/* Создать обработчик ошибок Vulkan */
	VkResult CreateErrorVulkanHook(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	/* Уничтожить обработчик ошибок Vulkan */
	void DestroyErrorVulkanHook(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}

	/* ? */
	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = ErrorVulkanHook;
	}

	/* Установить обработчик ошибок Vulkan */
	void SetupErrorVulkanHook() {
		if (!DeveloperVersion) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		PopulateDebugMessengerCreateInfo(createInfo);

		VkResult CreateErrorVulkanHook_Result = CreateErrorVulkanHook(Vulkan, &createInfo, nullptr, &VulkanErrorHook);
		if (CreateErrorVulkanHook_Result != VK_SUCCESS) {
			Print("failed to set up debug messenger! (" + std::to_string(CreateErrorVulkanHook_Result) + ")");
		}
	}

	/* Проверка слоя Vulkan */
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

	/* Загрузка Vulkan */
	void RunVulkan() {
		uint32_t vk_ec = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &vk_ec, nullptr);
		Print("Vulkan Extensions Supported: " + std::to_string(vk_ec));

		VkApplicationInfo Info{};
		Info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		Info.pApplicationName = "LithiumUniverse";
		Info.applicationVersion = VK_MAKE_VERSION(GameVersionMajor, GameVersionMinor, GameVersionPatch);
		Info.pEngineName = "No Engine";
		Info.engineVersion = VK_MAKE_VERSION(GameVersionMajor, GameVersionMinor, GameVersionPatch);
		Info.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &Info;

		std::vector<const char*> createInfoExtensions = {};

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		for (uint32_t i = 0; i < glfwExtensionCount; i++) {
			createInfoExtensions.push_back(glfwExtensions[i]);
		}

		if (DeveloperVersion) {
			createInfoExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		createInfo.enabledExtensionCount = createInfoExtensions.size();
		createInfo.ppEnabledExtensionNames = createInfoExtensions.data();

		createInfo.enabledLayerCount = 0;

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (DeveloperVersion) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			PopulateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;

			createInfo.pNext = nullptr;
		}

		VkResult result = vkCreateInstance(&createInfo, nullptr, &Vulkan);
		if (vkCreateInstance(&createInfo, nullptr, &Vulkan) != VK_SUCCESS) {
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

		if (DeveloperVersion && !CheckValidationLayerSupport()) {
			Print("validation layers requested, but not available!");
		}
		if (DeveloperVersion) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionNames.size());
		createInfo.ppEnabledExtensionNames = extensionNames.data();
	}
	
	/* Создание окна */
	void CreateWindow() {
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		const uint32_t WIDTH = 800;
		const uint32_t HEIGHT = 600;

		std::string WindowTitle = "LithiumUniverse (" + GetGameVersion() + ")";
		Window = glfwCreateWindow(WIDTH, HEIGHT, WindowTitle.c_str(), NULL, NULL);
		if (!Window) {
			Error = GameInstallError::GLFW_NOT_CREATE_WINDOW;
			Window = NULL;
		}
		else {
			Print("Window created!");
			glfwMakeContextCurrent(Window);
		}
	}

	/* Загрузка GLFW */
	void RunGLFW() {
		glfwInit();
		CreateWindow();
	}

	/* Загрузка всего */
	void RunAll() {
		RunGLFW();
		RunVulkan();
	}

	/* Цикл GLFW */
	void LoopGLFW() {
		glfwSwapBuffers(Window);
		glfwPollEvents();
	}

	/* Цикл всего */
	void Loop() {
		while (!glfwWindowShouldClose(Window)) {
			LoopGLFW();
		}
	}

	/* Очистить Vulkan */
	void DestroyVulkan() {
		if (DeveloperVersion) {
			DestroyErrorVulkanHook(Vulkan, VulkanErrorHook,nullptr);
		}
		vkDestroyInstance(Vulkan,nullptr);
	}

	/* Очистить GLFW */
	void DestroyGLFW() {
		glfwTerminate();
	}

	/* Очистить всё */
	void DestroyAll() {
		DestroyGLFW();
		if (Error != GameInstallError::GLFW_NOT_CREATE_WINDOW) {
			DestroyVulkan();
		}
	}
};

int Run() {
	GameInstalls game;
	Print("Run LithiumUniverse (" + GetGameVersion() + (game.DeveloperVersion ? " DEV" : "") + ")!");

	try {
		game.Run();
	}
	catch (const std::exception& e) {
		std::string Error = e.what();
		Print("[TERMINATED ERROR]: " + Error);
		Print("The game was terminated with an error!");
		return EXIT_FAILURE;
	}
	Print("Game has been exit successfully!");
	return EXIT_SUCCESS;
}