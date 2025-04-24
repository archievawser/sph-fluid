#include <cmath>
#include <memory>
#include <random>
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "core/scene.h"
#include "core/application.h"
#include "core/shapes.h"
#include "fluidsim/particle.h"
#include "fluidsim/volume.h"
#include "core/input.h"
#include "utils/utils.h"

#define VALIDATE() assert(!(std::isnan(Velocity.x) || std::isnan(Velocity.y)))
/*

Particle::Particle(Volume* volume, Vector2 pos, Vector2 vel)
	: Position(pos), Velocity(vel), mVolume(volume)
{ 
	Size = 4;
}

Particle::Particle(const Particle & other)
	: Particle(other.mVolume, other.Position, other.Velocity)
{
}

Particle::~Particle()
{

}

void Particle::Start()
{
	
}

void Particle::Tick(float deltaTime)
{
	Velocity *= 0.99f;

	int substeps = 3;

	for (int i = 0; i < substeps; i++)
	{
		Density = mVolume->CalculateDensity(Position);
		UpdatePosition();
		ResolveCollisions();
		PredictedPosition = Position + Velocity;
		UpdateVelocity(deltaTime);
		
	}

	Shapes::DrawCircle(Vector3(Position, 0.0f), Size, Color3(glm::length(Velocity) / 5.0f, 0.5f, 1));
}

void Particle::ResolveCollisions()
{
	Vector2 halfBounds = mVolume->Bounds / 2.0f;
	halfBounds -= Vector2(Size);

	if (std::abs(Position.x) > halfBounds.x)
	{
		int sign = Position.x < 0 ? -1 : 1;
		Velocity.x *= -0.4f;
		Position.x = halfBounds.x * sign;
	}
	
	if (std::abs(Position.y) > halfBounds.y)
	{
		int sign = Position.y < 0 ? -1 : 1;
		Velocity.y *= -0.4f;
		Position.y = halfBounds.y * sign;
	}
}

void Particle::UpdateVelocity(float dt)
{
	dt = 1.0f / 240.0f;
	Vector2 pressureForce = GetPressureForce(PredictedPosition);
	Vector2 pressureAcceleration = pressureForce / Density;
	Velocity += Vector2(0, mVolume->Gravity * dt * dt) + (pressureAcceleration) * dt;
	Velocity += mVolume->CalculateViscosityForce(*this);

	if (Core::Input::IsMouseButtonDown(GLFW_MOUSE_BUTTON_MIDDLE))
	{
		Vector2 diff = PredictedPosition - (Vector2(Core::Input::MousePosition.x, -Core::Input::MousePosition.y) + Vector2(-(Core::Application::Instance->Width / 2.0f), Core::Application::Instance->Height / 2.0f));
		float mag = glm::length(diff);
		Velocity -= Vector2(diff.y, -diff.x) / std::max(mag * mag, 1.0f) * mVolume->TwistForce * dt;
	}

	if (Core::Input::IsMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
	{
		Vector2 diff = PredictedPosition - (Vector2(Core::Input::MousePosition.x, -Core::Input::MousePosition.y) + Vector2(-(Core::Application::Instance->Width / 2.0f), Core::Application::Instance->Height / 2.0f));
		float mag = glm::length(diff);
		if (mag < mVolume->GodRadius)
		{
			Velocity -= (diff / mag) * mVolume->PullForce * dt;
		}
	}
	
	if (Core::Input::IsMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
	{
		Vector2 diff = PredictedPosition - (Vector2(Core::Input::MousePosition.x, -Core::Input::MousePosition.y) + Vector2(-(Core::Application::Instance->Width / 2.0f), Core::Application::Instance->Height / 2.0f));
		float mag = glm::length(diff);
		if (mag < mVolume->GodRadius)
		{
			Velocity += (diff / mag) * mVolume->PushForce * dt;
		}
	}

	Velocity += (Vector2)Core::Input::WindowPositionDelta / 10000.f;
}

Vector2 Particle::GetPressureForce(Vector2 point)
{
	static std::default_random_engine e;
	static std::uniform_real_distribution<> dis(-1, 1); // range [0, 1)

	Vector2 force(0.0f);
	Vector2Int cellCoordOffsets[]
	{
		Vector2Int(-1, 0),
		Vector2Int(-1, 1),
		Vector2Int(-1, -1),
		Vector2Int(0, 1),
		Vector2Int(0, -1),
		Vector2Int(1, 0),
		Vector2Int(1, 1),
		Vector2Int(1, -1),
		Vector2Int(0, 0)
	};

	float myPressure = CalculatePressure();

	for (Vector2Int& offset : cellCoordOffsets)
	{
		int iteratingCell = mVolume->GetCellKey(CellCoord + offset);
		int beginning = mVolume->StartIndices[iteratingCell];

		for (int i = beginning; i < mVolume->ParticleCount; i++)
		{
			Particle& other = mVolume->Particles[mVolume->SpatialEntries[i].ParticleIndex];

			if (mVolume->SpatialEntries[i].GridCell != iteratingCell)
				break;

			if (&other == this)
				continue;

			Vector2 diff = other.PredictedPosition - PredictedPosition;
			float dist = glm::length(diff);

			if (dist < mVolume->InfluenceRadius)
			{
				Vector2 direction = dist == 0 ? Vector2(dis(e), dis(e)) : diff / dist;

				float slope = mVolume->SmoothDerivative(mVolume->InfluenceRadius, dist);
				float pressure = (myPressure + other.CalculatePressure()) / 2.0f;
				force += pressure * -direction * slope * 1.0f / Density;
			}
		}
	}

	return force;
}

float Particle::CalculatePressure()
{
	float densityOffset = Density - mVolume->TargetDensity;
	return densityOffset * mVolume->PressurePower;
}

void Particle::UpdatePosition()
{
	Position += Velocity;
}

int Particle::GetPartition()
{
	return 0;
}
*/