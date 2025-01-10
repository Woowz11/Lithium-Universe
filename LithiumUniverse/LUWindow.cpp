#include <GLFW/glfw3.h>
#include "Console.h";
#include "GlobalRender.h";

int CreateGameWindow() {
	GLFWwindow* Window;
	
	if (!glfwInit()) {
		return -1;
	}
	
	Window = glfwCreateWindow(800, 600, "LithiumUniverse (0.0.0c)", NULL, NULL);
	if (!Window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(Window);
	while (!glfwWindowShouldClose(Window)) {
		glfwSwapBuffers(Window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}