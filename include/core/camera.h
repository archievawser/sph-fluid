#pragma once
#include "glm/glm.hpp"
#include "core/entity.h"
#include "utils/utils.h"


namespace Core
{
	class Camera : public Entity
	{
	public:
		Camera();
		Camera(const Camera& other);

	public:
		virtual void Start() override;
		virtual void Tick(float deltaTime) override;

	public:
		Mat4x4 Transform;
		Mat4x4 Projection;
	};
}