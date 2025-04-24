#pragma once
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "core/renderable.h"


namespace Core
{
	class Renderer
	{
	public:
		Renderer(GLFWwindow* window);

	public:
		void Render(Renderable& renderable) const;
		//void Render(Scene& scene) const;

		inline GLFWwindow* GetWindow() const { return window; }

	private:
		GLFWwindow* window;
	};
}