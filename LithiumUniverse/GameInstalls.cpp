#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
#include "GlobalRender.h";
#include "Console.h";

enum GameInstallError {
	SUCCESS = 0,
	GLFW_NOT_CREATE_WINDOW = 1,
	GLAD_NOT_LOADED_GL = 2,
	GLFW_NOT_LOADED = 3
};

class GameInstalls {
public:
#ifdef NDEBUG
	const bool DeveloperVersion = false;
#else
	const bool DeveloperVersion = true;
#endif

	const uint32_t START_WINDOW_WIDTH = 800;
	const uint32_t START_WINDOW_HEIGHT = 600;
	GLFWwindow* Window = NULL;

	float FPS = -1;

	void Run() {
		RunAll();
		if (Error==SUCCESS) {
			Loop();
		}
		DestroyAll();
	}

	std::string GetGameTitle() {
		return "LithiumUniverse (" + GetGameVersion() + ") FPS: "+std::to_string(FPS);
	}

private:
	GameInstallError Error = SUCCESS;

	/* Создание окна */
	void CreateGameWindow() {
		Window = glfwCreateWindow(START_WINDOW_WIDTH, START_WINDOW_HEIGHT, GetGameTitle().c_str(), NULL, NULL);
		glfwSetWindowUserPointer(Window, this);
		if (!Window) {
			Error = GLFW_NOT_CREATE_WINDOW;
			Window = NULL;
		}
		else {
			Print("Window created!");
			glfwMakeContextCurrent(Window);
			glfwSetFramebufferSizeCallback(Window, WindowSizeChanged);
		}
	}

	/* Размер окна был изменён */
	static void WindowSizeChanged(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	/* Загрузка GLFW */
	void RunGLFW() {
		if (!glfwInit()) {
			Print("GLFW not loaded!");
			Error = GLFW_NOT_LOADED;
		}
		else {
			int major, minor, revision;
			glfwGetVersion(&major, &minor, &revision);
			Print("GLFW (" + std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(revision) + ")");
			CreateGameWindow();
		}
	}

	/* Загрузка GLAD */
	void RunGLAD() {
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			Error = GLAD_NOT_LOADED_GL;
			Print("GLAD not loaded GL!");
		}
		else {
			gladGLversionStruct VER = GLVersion;
			Print("GL (" + std::to_string(VER.major) + "." + std::to_string(VER.minor) + ")");
		}
		glViewport(0, 0, START_WINDOW_WIDTH, START_WINDOW_HEIGHT);
	}

	/* Загрузка всего */
	void RunAll() {
		RunGLFW();
		RunGLAD();
		InstallRender();

		Print("All started, and start Loop()!");
	}

	/* Вычесление FPS */
	float LastFPSTime = 0.0f;
	float LastFPSTimeForSecond = 0.0f;
	void CalculateFPS() {
		float currentTime = glfwGetTime();
		float deltaTime = currentTime - LastFPSTime;
		LastFPSTime = currentTime;
		if (currentTime - LastFPSTimeForSecond >= 0.5f) {
			LastFPSTimeForSecond = currentTime;
			FPS = (1 / deltaTime);
		}
	}

	/* Цикл GLFW */
	void LoopGLFW() {
		glfwSwapBuffers(Window);
		glfwPollEvents();

		glfwSetWindowTitle(Window, GetGameTitle().c_str());
	}

	/* Обработка клавиш */
	void ProcessInput()
	{
		if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			Print("EXIT!");
			glfwSetWindowShouldClose(Window, true);
		}
	}

	/* Цикл всего */
	void Loop() {
		while (!glfwWindowShouldClose(Window)) {
			ProcessInput();

			Render();
			CalculateFPS();

			LoopGLFW();
		}
	}

	/* Очистить GLFW */
	void DestroyGLFW() {
		glfwTerminate();
	}

	/* Очистить всё */
	void DestroyAll() {
		if (Error != GLFW_NOT_LOADED) {
			if (Error != GLFW_NOT_CREATE_WINDOW) {

			}
			DestroyGLFW();
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