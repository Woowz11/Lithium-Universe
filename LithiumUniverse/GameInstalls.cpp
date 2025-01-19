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

#include "LithiumUniverse.h";
#include "BaseConstants.h";
#include "GlobalRender.h";
#include "GlobalPhysic.h";
#include "GameControls.h";
#include "GameObject.h";
#include "GlobalUI.h";
#include "GameData.h";
#include "Console.h";

enum GameInstallError {
	SUCCESS                = 0,
	GLFW_NOT_CREATE_WINDOW = 1,
	GLAD_NOT_LOADED_GL     = 2,
	GLFW_NOT_LOADED        = 3
};

uint32_t CURRENT_WINDOW_WIDTH_  = 0;
uint32_t CURRENT_WINDOW_HEIGHT_ = 0;

class GameInstalls {
public:
	std::string GamePath;
	const uint32_t START_WINDOW_WIDTH = 800;
	const uint32_t START_WINDOW_HEIGHT = 600;
	GLFWwindow* Window = NULL;

	float FPS = -1;

	/* Запуск игры */
	void Run(std::string GamePath_) {
		GamePath = GamePath_;
		RunAll();
		if (GlobalError==SUCCESS) {
			Loop();
		}
		DestroyAll();
	}

	std::string GetGameTitle() {
		return "LithiumUniverse (" + GetGameVersion() + ") FPS: " + FillString(std::to_string(FPS),' ',10,false) + " SP: " + std::to_string(GetSimulationSpeed());
	}

	/* ==== Управление, другие функции ==== */

	int KeyPressed(int Key) {
		return glfwGetKey(Window, Key);
	}

	void ExitGame() {
		glfwSetWindowShouldClose(Window, true);
	}

private:
	GameInstallError GlobalError = SUCCESS;

	/* ==== Основа ==== */
	 
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
			CURRENT_WINDOW_WIDTH_ = START_WINDOW_WIDTH;
			CURRENT_WINDOW_HEIGHT_ = START_WINDOW_HEIGHT;
			glfwSetFramebufferSizeCallback(Window, WindowSizeChanged);
			UpdateWindowSize(START_WINDOW_WIDTH, START_WINDOW_HEIGHT);
			glfwSetWindowFocusCallback(Window, [](GLFWwindow* window, int focused) {
				GameInFocus = focused;
			});
			glfwSetKeyCallback(Window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
				if (GameInFocus) {
					ControlsKeyboard(key, action);
				}
			});
			glfwSetMouseButtonCallback(Window, [](GLFWwindow* window, int button, int action, int mods) {
				if (GameInFocus) {
					MouseClick(button, action);
				}
			});
			glfwSetScrollCallback(Window, [](GLFWwindow* window, double xoffset, double yoffset) {
				if (GameInFocus) {
					MouseScroll(yoffset);
				}
			});
		}
	}

	/* Размер окна был изменён */
	static void WindowSizeChanged(GLFWwindow* window, int width, int height)
	{
		CURRENT_WINDOW_WIDTH_ = width;
		CURRENT_WINDOW_HEIGHT_ = height;
		UpdateWindowSize(width, height);
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
			glEnable(GL_DEPTH_TEST);

			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

#ifdef NDEBUG
#else
			glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* msg, const void* data) {
				std::string _source;
				std::string _type;

				switch (source) {
				case GL_DEBUG_SOURCE_API:
					_source = "API";
					break;
				case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
					_source = "WIN";
					break;
				case GL_DEBUG_SOURCE_SHADER_COMPILER:
					_source = "SHADER";
					break;
				case GL_DEBUG_SOURCE_THIRD_PARTY:
					_source = "THIRDPARTY";
					break;
				case GL_DEBUG_SOURCE_APPLICATION:
					_source = "APP";
					break;
				case GL_DEBUG_SOURCE_OTHER:
					_source = "OTHER";
					break;
				default:
					_source = "UNKNOWN";
					break;
				}

				switch (type) {
				case GL_DEBUG_TYPE_ERROR:
					_type = "ERROR";
					break;
				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
					_type = "DEPRECATED BEHAVIOR";
					break;
				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
					_type = "UDEFINED BEHAVIOR";
					break;
				case GL_DEBUG_TYPE_PORTABILITY:
					_type = "PORTABILITY";
					break;
				case GL_DEBUG_TYPE_PERFORMANCE:
					_type = "PERFORMANCE";
					break;
				case GL_DEBUG_TYPE_OTHER:
					_type = "OTHER";
					break;
				case GL_DEBUG_TYPE_MARKER:
					_type = "MARKER";
					break;
				default:
					_type = "UNKNOWN";
					break;
				}

				switch (severity) {
				case GL_DEBUG_SEVERITY_HIGH:
					Error("GL " + _source + "/" + _type, msg);
					break;
				case GL_DEBUG_SEVERITY_MEDIUM:
					WarnSerious("GL " + _source + "/" + _type, msg);
					break;
				case GL_DEBUG_SEVERITY_LOW:
					Warn("GL " + _source + "/" + _type, msg);
					break;
				case GL_DEBUG_SEVERITY_NOTIFICATION:
					PrintImportant("GL " + _source + "/" + _type, msg);
					break;
				default:
					Print("GL " + _source + "/" + _type, msg);
					break;
				}
			}, NULL);
#endif
		}
	}

	/* Загрузка всего */
	void RunAll() {
		RunGLFW();
		RunGLAD();
		InstallRender(GamePath, START_WINDOW_WIDTH, START_WINDOW_HEIGHT, DeveloperVersion);

		Print("LU", "All started, and start Loop()!");
		Print("LU", "=============== [RUNTIME] ===============");
	}

	/* Вычесление FPS */
	float LastFPSTime = 0.0f;
	float LastFPSTimeForSecond = 0.0f;
	void CalculateFPS() {
		Time = glfwGetTime();
		DeltaTime = Time - LastFPSTime;
		LastFPSTime = Time;
		FPS = (1 / DeltaTime);
		GameDeltaTime = DeltaTime * GetSimulationSpeed();
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

		double xpos, ypos;
		glfwGetCursorPos(Window, &xpos, &ypos);
		glm::vec2 Pos  = (glm::vec2(xpos / (double)CURRENT_WINDOW_WIDTH_, ypos / (double)CURRENT_WINDOW_HEIGHT_) * glm::vec2(1,-1) - glm::vec2(0.5f, -0.5f)) * glm::vec2(2,2);
		glm::vec2 Pos2 = (glm::vec2(xpos / (double)START_WINDOW_WIDTH, ypos / (double)START_WINDOW_HEIGHT) * glm::vec2(1, -1) - glm::vec2(0.5f, -0.5f)) * glm::vec2(2, 2);
		glm::vec2 Pos3 = Pos2 + glm::vec2(1 - (double)CURRENT_WINDOW_WIDTH_ / (double)START_WINDOW_WIDTH, -(1 - (double)CURRENT_WINDOW_HEIGHT_ / (double)START_WINDOW_HEIGHT));
		MousePosition = Pos;
		MousePositionNonResize = Pos3;
		MouseWorldPosition = ScreenPositionToWorld(Pos3, false, true);
		MouseMove();
	}

	/* Цикл всего */
	void Loop() {
		while (!glfwWindowShouldClose(Window)) {
			Controls();

			UpdateUI();
			Render(UpdatePhysic());
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
		ClearData();
	}
};

/* Запуск игры */
GameInstalls Game;
int Run(std::string GamePath_) {
#ifdef NDEBUG
	DeveloperVersion = false;
#else
	DeveloperVersion = true;
#endif

	Print("LU", "Run LithiumUniverse (" + GetGameVersion() + (DeveloperVersion ? " $$YDEV$$_" : "") + ")!");

	try {
		Game.Run(GamePath_);
	}
	catch (const std::exception& e) {
		std::string Error = e.what();
		Fatal("LU CRASH", Error);
		Fatal("LU CRASH", "$$rThe game was terminated with an error!$$_");
		return EXIT_FAILURE;
	}
	Print("LU", "$$GGame has been exit successfully!$$_");
	End();
	return EXIT_SUCCESS;
}

/* Получить нажатие клавиши */
int KeyPressed(int Key) {
	return Game.KeyPressed(Key);
}

/* Закрыть игру */
void ExitGame() {
	Game.ExitGame();
}