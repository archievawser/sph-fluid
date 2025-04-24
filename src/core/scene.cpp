#include "core/scene.h"
#include "core/entity.h"
#include "core/camera.h"
#include "utils/utils.h"


namespace Core
{
	Scene::Scene()
		: Started(false), entities()
	{
		std::shared_ptr<Camera> camera (Instantiate<Camera>());
		Add(camera);
		CurrentCamera = camera;
	}

	Scene::~Scene()
	{

	}

	void Scene::Add(std::shared_ptr<Entity> entity)
	{
		entities.push_back(entity);
		entity->Start();
	}

	void Scene::Start()
	{
		Started = true;
		if (Started)
			return;
	}

	void Scene::Tick(float deltaTime)
	{
		for (std::shared_ptr<Entity>& entity : GetEntities())
		{
			if(entity->ShouldTickAutomatically)
			{
				entity->Tick(deltaTime);
			}
		}
	}
}