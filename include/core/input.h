#pragma once
#include <map>
#include <functional>
#include <string>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "utils/utils.h"


namespace Core
{
	typedef void(*Callback)();

	class Input
	{
	public:
		Input() = delete;

	public:
		/// <summary>
		/// Initialize after a successful call to glfwInit()
		/// </summary>
		static void Init(GLFWwindow* window);

	public:
		static inline bool IsKeyDown(int key) { return mIsKeyPressed[key]; }
		static inline void BindEventToKey(int key, std::function<void()> event) { mInputEventMap[key] = event; }
		static inline bool IsMouseButtonDown(int button) { return mIsMouseButtonPressed[button]; }
		static void OnKeyPressed(GLFWwindow* mWindow, int key, int scancode, int action, int mods);
		static void OnCursorMoved(GLFWwindow* window, double xpos, double ypos);
		static void OnMouseButtonPressed(GLFWwindow* window, int button, int action, int mods);
		static void OnWindowMoved(GLFWwindow* window, int xpos, int ypos);

	public:
		static Vector2Int MousePosition;
		static Vector2Int WindowPositionDelta;

	private:
		static bool mIsKeyPressed[512];
		static bool mIsMouseButtonPressed[512];
		static std::map<int, std::function<void()>> mInputEventMap;
	};
}