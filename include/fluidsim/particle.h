#pragma once
#include "core/entity.h"
#include "core/sprite.h"
#include "fluidsim/particle.h"
#include "utils/utils.h"


/*
class Volume;

class Particle
{
public:
	Particle(Volume* volume, Vector2 pos, Vector2 vel);
	Particle(const Particle& other);
	~Particle();

public:
	//virtual void Start() override;
	//virtual void Tick(float deltaTime) override;
	void ResolveCollisions();
	void UpdateVelocity(float dt);
	void UpdatePosition();
	int GetPartition();
	Vector2 GetPressureForce(Vector2 sample);
	float CalculatePressure();

public:


	Vector2 Velocity;
	Vector2 Position;
	Vector2 PredictedPosition;
	Vector2Int CellCoord;
	int CurrentCell;
	float Density;
	float Size;

private:
	Volume* mVolume;
};*/