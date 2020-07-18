#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
// #include <imgui/imgui.h>
// #include <imgui/imgui_impl_glfw_gl3.h>

#include "MyGlWindow.h"

MyGlWindow * glWindow;

bool lbutton_down;
bool rbutton_down;
bool mbutton_down;
double m_lastMouseX;
double m_lastMouseY;
double cx, cy;

#define MAXSAMPLES 100
int tickindex = 0;
int ticksum = 0;
int ticklist[MAXSAMPLES];

void window_size_callback(GLFWwindow* window, int width, int height)
{
	glWindow->setSize(width, height);
	glWindow->setAspect(width / (float)height);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	cx = xpos;
	cy = ypos;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		m_lastMouseX = xpos;
		m_lastMouseY = ypos;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (GLFW_PRESS == action)
			lbutton_down = true;
		else if (GLFW_RELEASE == action)
			lbutton_down = false;
	}

	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (GLFW_PRESS == action)
			rbutton_down = true;
		else if (GLFW_RELEASE == action)
			rbutton_down = false;
	}

	else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
		if (GLFW_PRESS == action)
			mbutton_down = true;
		else if (GLFW_RELEASE == action)
			mbutton_down = false;
	}
}

void mouseDragging(double width, double height)
{

	//if (!ImGui::IsMouseHoveringAnyWindow()) {
		if (lbutton_down) {
			float fractionChangeX = static_cast<float>(cx - m_lastMouseX) / static_cast<float>(width);
			float fractionChangeY = static_cast<float>(m_lastMouseY - cy) / static_cast<float>(height);
			glWindow->m_viewer->rotate(fractionChangeX, fractionChangeY);
		}
		else if (mbutton_down) {
			float fractionChangeX = static_cast<float>(cx - m_lastMouseX) / static_cast<float>(width);
			float fractionChangeY = static_cast<float>(m_lastMouseY - cy) / static_cast<float>(height);
			glWindow->m_viewer->zoom(fractionChangeY);
		}
		else if (rbutton_down) {
			float fractionChangeX = static_cast<float>(cx - m_lastMouseX) / static_cast<float>(width);
			float fractionChangeY = static_cast<float>(m_lastMouseY - cy) / static_cast<float>(height);
			glWindow->m_viewer->translate(-fractionChangeX, -fractionChangeY, 1);
		}
	//}

	m_lastMouseX = cx;
	m_lastMouseY = cy;
}

double CalcAverageTick(int newtick)
{
	ticksum -= ticklist[tickindex];
	ticksum += newtick;
	ticklist[tickindex] = newtick;

	if (++tickindex == MAXSAMPLES)
		tickindex = 0;

	return((double)ticksum / MAXSAMPLES);
}

int main(void)
{
	GLFWwindow* window;

	// Initialize the library
	if (!glfwInit())
	{
		std::cout << "GLFW Initialization has failed" << std:: endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	int width = 800;
	int height = 800;

	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(width, height, "OpenGL FrameWork", NULL, NULL);
	
	if (!window)
	{
		glfwTerminate();
		std::cout << "GLFW window create failed" << std::endl;
		return -1;
	}
	
	// Setup ImGui binding
	// ImGui_ImplGlfwGL3_Init(window, true);

	glfwMakeContextCurrent(window);

	// Make the window's context current

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cout << "glewInit failed" << std::endl;
		return 0;
	}

	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

	// Enable vsync
	glfwSwapInterval(1);

	glWindow = new MyGlWindow(width, height);

	bool show_test_window = true;
	bool show_another_window = false;

	double previousTime = glfwGetTime();
	double lastTime;
	double avgDrawTime = 0;

	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetKeyCallback(window, key_callback);
	
	glfwSetWindowTitle(window, "myGlWindow");
	
	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		// Rendering
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		
		glClearColor(0.5, 0.5, 0.5, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		
		lastTime = clock();
		glWindow->draw();

		avgDrawTime = CalcAverageTick(clock() - lastTime);

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
		// glfwWaitEvents();

		mouseDragging(display_w, display_h);
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}
