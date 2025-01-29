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
#include <thread>
#include <string>
#include <vector>
#include <chrono>
#include <array>
#include <set>

#include "LithiumUniverse.h";
#include "GlobalResources.h";
#include "BaseConstants.h";
#include "GlobalRender.h";
#include "GlobalPhysic.h";
#include "GameControls.h";
#include "DebugGetter.h";
#include "GameObject.h";
#include "GlobalLua.h";
#include "GlobalUI.h";
#include "GameData.h";
#include "Console.h";

enum GameInstallError {
	SUCCESS                = 0,
	GLFW_NOT_CREATE_WINDOW = 1,
	GLAD_NOT_LOADED_GL     = 2,
	GLFW_NOT_LOADED        = 3
};

class GameInstalls {
private:
	/* Ошибка */
	GameInstallError GlobalError = SUCCESS;

	/* Лимит FPS */
	std::chrono::duration<float> FrameDuration;
	bool FPSLimit = false;

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
			glfwSetFramebufferSizeCallback(Window, WindowSizeChanged);
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
		CURRENT_WINDOW_WIDTH = width;
		CURRENT_WINDOW_HEIGHT = height;
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
		InstallRender();
		InstallLua();
		InstallDebug();
		UpdateResources();
		InstallRenderAfterResources();

		Print("LU", "All started, and start Loop()!");
		Print("LU", "=============== [$$YRUNTIME$$_] ===============");
	}

	/* Вычесление FPS */
	float LastFPSTime = 0.0f;
	float LastFPSTimeForSecond = 0.0f;
	std::chrono::high_resolution_clock::time_point LastFPSTimePoint;
	void CalculateFPS() {
		Time = glfwGetTime();

		auto Now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> Elapsed = Now - LastFPSTimePoint;
		DeltaTime = Elapsed.count();
		LastFPSTimePoint = Now;

		if (DeltaTime > 0) {
			FPS = (1 / DeltaTime);
		}
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
		glm::vec2 Pos  = (glm::vec2(xpos / (double)CURRENT_WINDOW_WIDTH, ypos / (double)CURRENT_WINDOW_HEIGHT) * glm::vec2(1,-1) - glm::vec2(0.5f, -0.5f)) * glm::vec2(2,2);
		glm::vec2 Pos2 = (glm::vec2(xpos / (double)START_WINDOW_WIDTH, ypos / (double)START_WINDOW_HEIGHT) * glm::vec2(1, -1) - glm::vec2(0.5f, -0.5f)) * glm::vec2(2, 2);
		glm::vec2 Pos3 = Pos2 + glm::vec2(1 - (double)CURRENT_WINDOW_WIDTH / (double)START_WINDOW_WIDTH, -(1 - (double)CURRENT_WINDOW_HEIGHT / (double)START_WINDOW_HEIGHT));
		MousePosition = Pos;
		MousePositionNonResize = Pos3;
		MouseWorldPosition = ScreenPositionToWorld(Pos3, false, true);
		MousePositionScreen = glm::vec2(xpos, CURRENT_WINDOW_HEIGHT - ypos);
		MouseMove();
	}

	/* Ограничить FPS */
	std::chrono::steady_clock::time_point FrameStart;
	void LimitFPS() {
		auto FrameEnd = std::chrono::steady_clock::now();
		std::chrono::duration<float> FrameTime = FrameEnd - FrameStart;
		if (FrameTime < FrameDuration) {
			std::this_thread::sleep_for(FrameDuration - FrameTime);
		}
	}

	/* Цикл всего */
	void Loop() {
		while (!glfwWindowShouldClose(Window)) {
			auto StartTime = std::chrono::high_resolution_clock::now();

			if (FPSLimit) {
				FrameStart = StartTime;
			}

			Controls();

			Render(UpdatePhysic());

			LoopGLFW();
			
			if (FPSLimit) {
				LimitFPS();
			}
			CalculateFPS();

			auto EndTime = std::chrono::high_resolution_clock::now();
			MS = std::chrono::duration_cast<std::chrono::microseconds>(EndTime - StartTime).count() / 1000.0;
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
public:
	GLFWwindow* Window = NULL;

	/* Запуск игры */
	void Run() {
		RunAll();
		if (GlobalError == SUCCESS) {
			Loop();
		}
		DestroyAll();
	}

	std::string GetGameTitle() {
		return "LithiumUniverse (" + Version + ") MS[" + RemoveStringPart(ToStringNumber(MS), 4) + "] FPS[" + RemoveStringPart(ToStringNumber(FPS),6) + "] SP[" + RemoveStringPart(ToStringNumber(GetSimulationSpeed()),3) + "]";
	}

	/* ==== Управление, другие функции ==== */

	int KeyPressed(int Key) {
		return glfwGetKey(Window, Key);
	}

	void ExitGame() {
		glfwSetWindowShouldClose(Window, true);
	}

	void SetFPSLimit(int newFpsLimit) {
		if (newFpsLimit <= 0) {
			FPSLimit = false;
			Print("LU", "FPS limit removed!");
		}
		else {
			FrameDuration = std::chrono::duration<float>(1.0f / newFpsLimit);
			FPSLimit = true;
			Print("LU","FPS limit set to " + std::to_string(newFpsLimit) + "!");
		}
	}
};

/* Запуск игры */
GameInstalls Game;
int Run() {
	try {
#ifdef NDEBUG
		DeveloperVersion = false;
#else
		DeveloperVersion = true;
#endif
		Version = GetGameVersion();
		InstallConsole();
		Print("LU", "Run LithiumUniverse (" + Version + ")!");

		Game.Run();

		Print("LU", "$$GGame has been exit successfully!$$_");
		CloseConsole();
		End();
	}
	catch (const std::exception& e) {
		std::string Error = e.what();
		Fatal("LU CRASH", Error);
		Fatal("LU CRASH", "$$rThe game was terminated with an error!$$_");
		return EXIT_FAILURE;
	}
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

/* Установить лимит FPS */
void SetFPSLimit(int NewFPSLimit) {
	Game.SetFPSLimit(NewFPSLimit);
}