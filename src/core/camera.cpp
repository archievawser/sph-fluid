#include "glm/gtc/matrix_transform.hpp"
#include "core/camera.h"
#include "core/application.h"


namespace Core
{
	Camera::Camera()
	{
		float right = (float)(Application::Instance->Width) / 2.f;
		float up = (float)(Application::Instance->Height) / 2.f;
		Projection = glm::ortho(-right, right, -up, up, 0.1f, 10000.0f);
		Transform = glm::translate(Mat4x4(1.0f), Vector3(0, 0, -5000));
	}

	Camera::Camera(const Camera& other)
	{
		Transform = other.Transform;
		Projection = other.Projection;
	}

	void Camera::Start()
	{

	}

	void Camera::Tick(float deltaTime)
	{

	}
}