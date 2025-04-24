#pragma once
#include "core/entity.h"
#include "core/sprite.h"
#include "core/scene.h"
#include "utils/utils.h"


class Player : public Core::Entity
{
public:
	Player();
	Player(const Player& other);

public:
	virtual void Start() override;
	virtual void Tick(float deltaTime) override;

public:
	float WalkSpeed = 160.0f;
	Mat4x4 Transform;
	std::shared_ptr<Core::Sprite> Sprite;

private:
	typedef Core::Entity Base;
	unsigned long long mOnlineID;
};