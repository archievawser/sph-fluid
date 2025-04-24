#pragma once
#include "core/entity.h"
#include "core/scene.h"
#include "fluidsim/volume.h"



class EngineTestScene : public Core::Scene
{
public:
	EngineTestScene();

public:
	virtual void Start() override;
	virtual void Tick(float deltaTime) override;

public:
	typedef Core::Scene Base;

private:
	std::shared_ptr<Volume> mVolume;
};