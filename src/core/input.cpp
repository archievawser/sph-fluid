#include "core/input.h"
#include "core/clock.h"
#include "core/application.h"
#include "utils/utils.h"


namespace Core
{
	bool Input::mIsKeyPressed[512];
	bool Input::mIsMouseButtonPressed[512];
	Vector2Int Input::MousePosition;
	Vector2Int Input::WindowPositionDelta;
	std::map<int, std::function<void()>> Input::mInputEventMap;

	void Input::OnKeyPressed(GLFWwindow* mWindow, int key, int scancode, int action, int mods)
	{
		bool isBeingPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
		mIsKeyPressed[key] = isBeingPressed;
		if(mInputEventMap.find(key) != mInputEventMap.end())
			mInputEventMap[key]();
	} 

	void Input::Init(GLFWwindow* window)
	{
		glfwSetKeyCallback(window, Input::OnKeyPressed);
		glfwSetCursorPosCallback(window, Input::OnCursorMoved);
		glfwSetMouseButtonCallback(window, Input::OnMouseButtonPressed);
		glfwSetWindowPosCallback(window, OnWindowMoved);
	}

	void Input::OnCursorMoved(GLFWwindow* window, double xpos, double ypos)
	{
		MousePosition = Vector2Int(xpos, ypos);
	}

	void Input::OnMouseButtonPressed(GLFWwindow* window, int button, int action, int mods)
	{
		bool isBeingPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
		mIsMouseButtonPressed[button] = isBeingPressed;
	}

	void Input::OnWindowMoved(GLFWwindow* window, int xpos, int ypos)
	{
		static int lastXPos = xpos;
		static int lastYPos = ypos;
		static Clock deltaTimer;

		WindowPositionDelta = Vector2Int(xpos - lastXPos, ypos - lastYPos);

		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		Application::Instance->GetGameState()->GetCurrentScene()->Tick(std::min((float)deltaTimer.Restart(), 1.f / 60.f) / 1000.0f);

		glfwSwapBuffers(window);
	}
}