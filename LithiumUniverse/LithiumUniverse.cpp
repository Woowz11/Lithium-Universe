#include <GLFW/glfw3.h>
#include <string>
#include "LibControl.h";
#include "GlobalRender.h";
#include "LUWindow.h";
#include "Console.h";
#include "BaseConstants.h";

GLFWwindow* GameWindow;

void GlobalUpdate() {
	glfwSwapBuffers(GameWindow);
	glfwPollEvents();
}

int Start() {
	Print("LithiumUniverse (" + GetGameVersion() + ") started!");

	if (!Initializate_GLFW()) {
		return -1;
	}

	/* Создание окна */
	GameWindow = CreateGameWindow();
	if (GameWindow == NULL) {
		Terminate_GLFW();
		return -1;
	}

	CreateVulkan();

	while (!glfwWindowShouldClose(GameWindow)) {
		GlobalUpdate();
	}

	Terminate_Vulkan();
	Terminate_GLFW();

	return 0;
}