#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <string>
#include <vector>
#include <optional>
#include <map>
#include <set>
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
	VkDebugUtilsMessengerEXT VulkanMessagesHook = nullptr;
	VkPhysicalDevice VulkanPhysicalDevice = VK_NULL_HANDLE;
	VkDevice VulkanDevice = nullptr;
	VkQueue GraphicsQueue = nullptr;
	VkQueue PresentQueue = nullptr;
	VkSurfaceKHR VulkanSurface = nullptr;

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
	static VKAPI_ATTR VkBool32 VKAPI_CALL MessagesVulkanHook(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		std::string Message = pCallbackData->pMessage;
		Print("[VULKAN]: " + Message);

		return VK_FALSE;
	}
	
	/* Создать обработчик ошибок Vulkan */
	VkResult CreateMessagesVulkanHook(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	/* Уничтожить обработчик ошибок Vulkan */
	void DestroyMessagesVulkanHook(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
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
		createInfo.pfnUserCallback = MessagesVulkanHook;
	}

	/* Установить обработчик ошибок Vulkan */
	void SetupMessagesVulkanHook() {
		if (!DeveloperVersion) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		PopulateDebugMessengerCreateInfo(createInfo);

		VkResult CreateMessagesVulkanHook_Result = CreateMessagesVulkanHook(Vulkan, &createInfo, nullptr, &VulkanMessagesHook);
		if (CreateMessagesVulkanHook_Result != VK_SUCCESS) {
			Print("failed to set up debug messenger! (" + std::to_string(CreateMessagesVulkanHook_Result) + ")");
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

	/* Проверить девайс, подходит ли он для Vulkan */
	bool IsDeviceSuitable(VkPhysicalDevice device) {
		QueueFamilyIndices indices = FindQueueFamilies(device);

		return indices.isComplete();
	}

	/* Оценить качество девайса */
	int RateDeviceSuitability(VkPhysicalDevice device) {
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		int score = 0;
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			score += 1000;
		}

		score += deviceProperties.limits.maxImageDimension2D;

		if (!deviceFeatures.geometryShader) {
			return 0;
		}

		return score;
	}

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, VulkanSurface, &presentSupport);

			if (presentSupport) {
				indices.presentFamily = i;
			}

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			if (indices.isComplete()) {
				break;
			}

			i++;
		}

		return indices;
	}

	/* Установить Vulkan какими девайсами пользоваться */
	void PickPhysicalDeviceForVulkan() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(Vulkan, &deviceCount, nullptr);
		if (deviceCount == 0) {
			Print("failed to find GPUs with Vulkan support!");
		}
		else {
			Print("Physical devices count: "+std::to_string(deviceCount));
		}
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(Vulkan, &deviceCount, devices.data());

		for (const VkPhysicalDevice& device : devices) {
			if (IsDeviceSuitable(device)) {
				VulkanPhysicalDevice = device;
				break;
			}
		}

		std::multimap<int, VkPhysicalDevice> candidates;

		for (const auto& device : devices) {
			int score = RateDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));
		}

		if (candidates.rbegin()->first > 0) {
			VulkanPhysicalDevice = candidates.rbegin()->second;
		}
		else {
			Print("failed to find a suitable GPU!");
		}
	}

	void CreateLogicalDeviceForVulkan() {
		QueueFamilyIndices indices = FindQueueFamilies(VulkanPhysicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = 0;

		if (DeveloperVersion) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(VulkanPhysicalDevice, &createInfo, nullptr, &VulkanDevice) != VK_SUCCESS) {
			Print("failed to create logical device!");
		}

		vkGetDeviceQueue(VulkanDevice, indices.graphicsFamily.value(), 0, &GraphicsQueue);
		vkGetDeviceQueue(VulkanDevice, indices.presentFamily.value(), 0, &PresentQueue);
	}

	/* Соеденить Vulkan с GLFW окном */
	void ConnectVulkanAndGLFW() {
		VkWin32SurfaceCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = glfwGetWin32Window(Window);
		createInfo.hinstance = GetModuleHandle(nullptr);

		if (vkCreateWin32SurfaceKHR(Vulkan, &createInfo, nullptr, &VulkanSurface) != VK_SUCCESS) {
			Print("failed to create window surface!");
		}
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

		ConnectVulkanAndGLFW();
		PickPhysicalDeviceForVulkan();
		CreateLogicalDeviceForVulkan();
	}
	
	/* Создание окна */
	void CreateGameWindow() {
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
		CreateGameWindow();
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
		vkDestroyDevice(VulkanDevice, nullptr);
		if (DeveloperVersion) {
			DestroyMessagesVulkanHook(Vulkan, VulkanMessagesHook,nullptr);
		}
		vkDestroySurfaceKHR(Vulkan, VulkanSurface, nullptr);
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