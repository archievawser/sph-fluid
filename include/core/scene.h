#pragma once
#include <memory>
#include <vector>
#include "core/entity.h"
#include "core/camera.h"


namespace Core
{
	class Scene
	{
	protected:
		Scene();
		Scene(Scene& other) = delete;
		virtual ~Scene();

	public:
		void Add(std::shared_ptr<Entity> entity);
		virtual void Start() = 0;
		virtual void Tick(float deltaTime) = 0;
		template<typename T, typename... Args>
		static inline std::shared_ptr<T> Instantiate(Args&&... args) noexcept
		{
			return std::shared_ptr<T>(StaticObjectPool<T>::RequestNew(args...), StaticObjectPool<T>::OnPointerDied);
		}
		template<typename T>
		static inline std::shared_ptr<T> Instantiate() noexcept
		{
			return std::shared_ptr<T>(StaticObjectPool<T>::RequestNew(), StaticObjectPool<T>::OnPointerDied);
		}

		inline std::vector<std::shared_ptr<Entity>>& GetEntities() { return entities; }

	public:
		typedef Scene Base;
		std::shared_ptr<Camera> CurrentCamera;
		bool Started;

	private:
		std::vector<std::shared_ptr<Entity>> entities;
	};
}