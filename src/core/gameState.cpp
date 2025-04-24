#include "core/gameState.h"

Core::GameState::GameState()
{

}

Core::GameState::GameState(std::shared_ptr<Scene> scene)
{
	SetCurrentScene(scene);
}
