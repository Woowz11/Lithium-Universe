#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include "LibControl.h";
#include "GlobalRender.h";
#include "InstallVulkan.h";
#include "LUWindow.h";
#include "Console.h";
#include "BaseConstants.h";

GLFWwindow* GameWindow;

#ifdef NDEBUG
const bool DeveloperVersion = false;
#else
const bool DeveloperVersion = true;
#endif

void GlobalUpdate() {
	glfwSwapBuffers(GameWindow);
	glfwPollEvents();
}

int Start() {
	Print("LithiumUniverse (" + GetGameVersion() + (DeveloperVersion?" DEV" : "") + ") started!");

	try {

		if (!Initializate_GLFW()) {
			return EXIT_FAILURE;
		}

		/* Создание окна */
		GameWindow = CreateGameWindow();
		if (GameWindow == NULL) {
			Terminate_GLFW();
			return EXIT_FAILURE;
		}

		CreateVulkan();

		while (!glfwWindowShouldClose(GameWindow)) {
			GlobalUpdate();
		}

		Terminate_Vulkan();
		Terminate_GLFW();

		return EXIT_SUCCESS;

	}
	catch (const std::exception& e) {
		Print(e.what());
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}