#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include "LibControl.h";
#include "Console.h";
#include "GlobalRender.h";
#include "LithiumUniverse.h";
#include "BaseConstants.h";

GLFWwindow* CreateGameWindow() {
	GLFWwindow* Window = NULL;
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	std::string WindowTitle = "LithiumUniverse (" + GetGameVersion() + ")";
	Window = glfwCreateWindow(WIDTH, HEIGHT, WindowTitle.c_str(), NULL, NULL);
	
	if (!Window) {
		Terminate_GLFW();
		return NULL;
	}

	Print("Window created!");

	glfwMakeContextCurrent(Window);

	return Window;
}