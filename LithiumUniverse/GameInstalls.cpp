#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <stdexcept>
#include <algorithm>
#include <optional>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>
#include <chrono>
#include <array>
#include <set>

#include "BaseConstants.h";
#include "Console.h";

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

	void Run() {
		RunAll();
		if (Error==GameInstallError::SUCCESS) {
			Loop();
		}
		DestroyAll();
	}

private:
	GameInstallError Error = GameInstallError::SUCCESS;

	/* Создание окна */
	void CreateGameWindow() {
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		const uint32_t WIDTH = 800;
		const uint32_t HEIGHT = 600;

		std::string WindowTitle = "LithiumUniverse (" + GetGameVersion() + ")";
		Window = glfwCreateWindow(WIDTH, HEIGHT, WindowTitle.c_str(), NULL, NULL);
		glfwSetWindowUserPointer(Window, this);
		//glfwSetFramebufferSizeCallback(Window, FramebufferResizeCallback);
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

	/* Очистить GLFW */
	void DestroyGLFW() {
		glfwTerminate();
	}

	/* Очистить всё */
	void DestroyAll() {
		if (Error != GameInstallError::GLFW_NOT_CREATE_WINDOW) {
			
		}
		DestroyGLFW();
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