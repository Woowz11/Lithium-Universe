#include <glad/glad.h>
#include <GLFW/glfw3.h>
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
		if (GlobalError==SUCCESS) {
			Loop();
		}
		DestroyAll();
	}

	std::string GetGameTitle() {
		return "LithiumUniverse (" + GetGameVersion() + ") FPS: "+std::to_string(FPS);
	}

private:
	GameInstallError GlobalError = SUCCESS;

	/* Создание окна */
	void CreateGameWindow() {
		Window = glfwCreateWindow(START_WINDOW_WIDTH, START_WINDOW_HEIGHT, GetGameTitle().c_str(), NULL, NULL);
		glfwSetWindowUserPointer(Window, this);
		if (!Window) {
			GlobalError = GLFW_NOT_CREATE_WINDOW;
			Window = NULL;
		}
		else {
			Print("GLFW", "Window created!");
			glfwMakeContextCurrent(Window);
			glfwSwapInterval(0);
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
			Fatal("GLFW", "GLFW not loaded!");
			GlobalError = GLFW_NOT_LOADED;
		}
		else {
			int major, minor, revision;
			glfwGetVersion(&major, &minor, &revision);
			Print("GLFW", "GLFW (" + std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(revision) + ")");
			CreateGameWindow();
		}
	}

	/* Загрузка GLAD */
	void RunGLAD() {
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			GlobalError = GLAD_NOT_LOADED_GL;
			Fatal("GLAD", "GLAD not loaded GL!");
		}
		else {
			gladGLversionStruct VER = GLVersion;
			Print("GLAD", "GL (" + std::to_string(VER.major) + "." + std::to_string(VER.minor) + ")");
		}
		glEnable(GL_DEPTH_TEST);
	}

	/* Загрузка всего */
	void RunAll() {
		RunGLFW();
		RunGLAD();
		InstallRender(START_WINDOW_WIDTH,START_WINDOW_HEIGHT);

		Print("LU", "All started, and start Loop()!");
	}

	/* Вычесление FPS */
	float LastFPSTime = 0.0f;
	float LastFPSTimeForSecond = 0.0f;
	void CalculateFPS() {
		float currentTime = glfwGetTime();
		float deltaTime = currentTime - LastFPSTime;
		LastFPSTime = currentTime;
		FPS = (1 / deltaTime);
	}

	/* Цикл GLFW */
	void LoopGLFW() {
		glfwSwapBuffers(Window);
		glfwPollEvents();

		float currentTime = glfwGetTime();
		if (currentTime - LastFPSTimeForSecond >= 0.5f) {
			LastFPSTimeForSecond = currentTime;
			glfwSetWindowTitle(Window, GetGameTitle().c_str());
		}
	}

	/* Обработка клавиш */
	void ProcessInput()
	{
		if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(Window, true);
		}

		if (glfwGetKey(Window, GLFW_KEY_HOME) == GLFW_PRESS) {
			SetCameraPosition(0, 0);
		}

		bool W = glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS;
		bool S = glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS;
		bool D = glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS;
		bool A = glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS;

		bool SHIFT = glfwGetKey(Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
		bool CONTROL = glfwGetKey(Window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;

		float speed = (SHIFT?3:(CONTROL?0.3f:1));

		if (W && !S) {
			MoveCamera( 0,  speed);
		}
		if (S && !W) {
			MoveCamera( 0, -speed);
		}

		if (D && !A) {
			MoveCamera(speed,  0);
		}
		if (A && !D) {
			MoveCamera(-speed,  0);
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
		if (GlobalError != GLFW_NOT_LOADED) {
			if (GlobalError != GLFW_NOT_CREATE_WINDOW) {
				ClearRender();
			}
			DestroyGLFW();
		}
	}
};

int Run() {
	GameInstalls game;
	Print("LU", "Run LithiumUniverse (" + GetGameVersion() + (game.DeveloperVersion ? " $$CDEV$$_" : "") + ")!");

	try {
		game.Run();
	}
	catch (const std::exception& e) {
		std::string Error = e.what();
		Fatal("LU CRASH", Error);
		Fatal("LU CRASH", "The game was terminated with an error!");
		return EXIT_FAILURE;
	}
	Print("LU", "Game has been exit successfully!");
	return EXIT_SUCCESS;
}