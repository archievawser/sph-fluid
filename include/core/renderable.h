#pragma once
#include "core/geometry.h"
#include "core/shader.h"
#include "core/entity.h"


namespace Core
{
	class Renderable : public Entity
	{
	public:
		Renderable();
		Renderable(std::shared_ptr<Geometry> geometry, std::shared_ptr<Shader> shader);
		Renderable(const Renderable& other);
		virtual ~Renderable();

	public:
		virtual void Start() override;
		virtual void Tick(float deltaTime) override;
		inline void Bind() const { mGeometry->Bind(); mShader->Bind(); }
		inline std::shared_ptr<Shader> GetShader() const { return mShader; }
		inline std::shared_ptr<Geometry> GetGeometry() const { return mGeometry; }

	protected:
		std::shared_ptr<Geometry> mGeometry;
		std::shared_ptr<Shader> mShader;

	private:
		typedef Entity Base;
	};
}