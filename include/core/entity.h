#pragma once
#include <memory>
#include <vector>
#include "utils/utils.h"


namespace Core
{
	/// <summary>
	/// Entity is the abstract base class for all runtime elements of a Scene
	/// </summary>
	class Entity
	{
	protected:
		Entity();
		Entity(Entity& other);
		virtual ~Entity();

	public:
		virtual void Start() = 0;
		virtual void Tick(float deltaTime) = 0;
		inline std::shared_ptr<Entity> GetParent() const { return parent; }
		inline void SetParent(std::shared_ptr<Entity> newParent) 
		{ 
			if (parent)
				parent->children.erase(std::find(parent->children.begin(), parent->children.end(), GameIdentity));

			parent = newParent; 
			newParent->children.push_back(GameIdentity);
		}

	public:
		std::shared_ptr<Entity> GameIdentity;
		bool ShouldTickAutomatically = true;

	private:
		typedef Entity Base;
		std::shared_ptr<Entity> parent;
		std::vector<std::shared_ptr<Entity>> children;
	};


	template<typename T>
	class StaticObjectPool
	{
	protected:
		StaticObjectPool();

	public:
		static void OnPointerDied(T* pointer)
		{
			RABID_WARNING(pointer << " just died...");

			for (int i = 0; i < Data.size(); i++)
			{
				if (Data[i] == pointer)
				{
					Data.erase(Data.begin() + i);
					break;
				}
			}
		}

		template<typename... ArgType>
		static T* RequestNew(ArgType&&... args) noexcept
		{
			Data.push_back(new T(args...));
			return Data.back();
		}

	public:
		static std::vector<T*> Data;
	};

	template<typename T>
	std::vector<T*> StaticObjectPool<T>::Data;
}