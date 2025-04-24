#include "core/entity.h"
#include "utils/utils.h"


namespace Core
{
	template<typename T>
	StaticObjectPool<T>::StaticObjectPool()
	{
	}
	Entity::Entity()
	{
	}
	Entity::Entity(Entity& other)
	{
		RABID_WARNING("Entity object has been copied");

		parent = other.parent;
		children = other.children;
	}
	Entity::~Entity()
	{
	}
}