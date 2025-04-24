#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "core/scene.h"
#include "core/sprite.h"
#include "core/application.h"
#include "core/input.h"
#include "core/player.h"
#include "utils/utils.h"


Player::Player()
	: Transform(Mat4x4(1.0f)), mOnlineID(0)
{ }

Player::Player(const Player& other)
	: Transform(other.Transform), mOnlineID(other.mOnlineID)
{ }

void Player::Start()
{
	Sprite = Core::Scene::Instantiate<Core::Sprite>("donut.png");
	Core::Application::Instance->GetGameState()->GetCurrentScene()->Add(Sprite);
}

void Player::Tick(float deltaTime)
{
	float forwardAmount = (float)Core::Input::IsKeyDown(GLFW_KEY_W) - (float)Core::Input::IsKeyDown(GLFW_KEY_S);
	float rightAmount = (float)Core::Input::IsKeyDown(GLFW_KEY_D) - (float)Core::Input::IsKeyDown(GLFW_KEY_A);

	Vector2 moveAxis(rightAmount, forwardAmount);
	
	if(moveAxis.x || moveAxis.y)
		moveAxis = glm::normalize(moveAxis);

	Sprite->Transform = glm::translate(Sprite->Transform, Vector3(moveAxis * deltaTime * WalkSpeed, 0));
}