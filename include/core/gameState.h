#pragma once
#include "core/scene.h"


namespace Core
{
	class GameState
	{
	public:
		GameState();
		GameState(std::shared_ptr<Scene> scene);
		
	public:
		inline std::shared_ptr<Scene> GetCurrentScene() { return mCurrentScene; }
		inline void SetCurrentScene(std::shared_ptr<Scene> scene) { mCurrentScene = scene; scene->Start(); }

	private:
		std::shared_ptr<Scene> mCurrentScene;
	};
}