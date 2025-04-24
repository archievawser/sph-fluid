#include "GL/glew.h"
#include "core/renderer.h"


namespace Core
{
	Renderer::Renderer(GLFWwindow* window)
		: window(window)
	{ }

	void Renderer::Render(Renderable& renderable) const
	{
		renderable.Bind();

		glDrawElements(GL_TRIANGLES, renderable.GetGeometry()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
	}
}
