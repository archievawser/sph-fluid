#pragma once
#include "glm/glm.hpp"
#include "core/renderable.h"
#include "utils/utils.h"
#include "core/texture.h"


namespace Core
{
	class Sprite : public Renderable
	{
	public:
		Sprite(const char* texturePath);
		Sprite(const Sprite& other);
		
	public:
		virtual void Tick(float deltaTime) override;

	public:
		Mat4x4 Transform;
		std::shared_ptr<Texture> Brush;

	private:
		typedef Renderable Base;
	};
}