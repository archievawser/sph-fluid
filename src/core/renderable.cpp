#include "GL/glew.h"
#include "core/geometry.h"
#include "core/shader.h"
#include "core/renderable.h"


namespace Core
{
	Renderable::Renderable()
		: mGeometry(nullptr), mShader(nullptr)
	{ }

	Renderable::Renderable(std::shared_ptr<Geometry> geometry, std::shared_ptr<Shader> shader)
		: mGeometry(geometry), mShader(shader)
	{ }

	Renderable::Renderable(const Renderable& other)
	{
		mGeometry = other.GetGeometry();
		mShader = other.GetShader();
	}

	Renderable::~Renderable()
	{

	}

	void Renderable::Start()
	{

	}

	void Renderable::Tick(float deltaTime)
	{
		Bind();
		glDrawElements(GL_TRIANGLES, mGeometry->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
	}
}