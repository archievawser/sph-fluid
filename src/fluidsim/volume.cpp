#include <cstdint>
#include <numbers>
#include <memory>
#include <vector>
#include <functional>
#include <algorithm>
#include <intrin.h>
#include "GL/glew.h"
#include <random>
#include <execution>
#include <ppl.h>
#include "glm/gtx/hash.hpp"
#include "core/gradient.h"
#include "core/application.h"
#include "core/renderer.h"
#include "core/clock.h"
#include "core/scene.h"
#include "core/shapes.h"
#include "core/clock.h"
#include "core/input.h"
#include "utils/utils.h"
#include "fluidsim/volume.h"


Volume::Volume(Vector2 bounds, uint32_t particleCount)
	: Bounds(bounds), ParticleCount(particleCount), PullForce(80.f), PushForce(80.f), TwistForce(5.f), 
	  PressurePower(0.125f), TargetDensity(0.005f), Gravity(-6000.f), GodRadius(100.f), ViscosityStrength(8.f), 
	  GradientScale(0.11f)
{
	SpatialEntries = std::vector<SpatialGridEntry>();
	SpatialEntries.resize(ParticleCount);
	StartIndices = std::vector<uint32_t>();
	StartIndices.resize(ParticleCount);
	InfluenceRadius = 40.f;
	ParticleSize = 3.f;
	
	mParticleColor = Gradient<Color3>({
		GradientKeyframe(0.0f, Gradient<Color3>::FromHex(0x0061FF)),
		GradientKeyframe(0.26f, Gradient<Color3>::FromHex(0x6253A8)),
		GradientKeyframe(0.62f, Gradient<Color3>::FromHex(0xF1A210)),
		GradientKeyframe(1.0f, Gradient<Color3>::FromHex(0xAF1010))
	});

	PositionCompute = std::make_shared<Core::ComputeShader>("src/Shaders/particlePosition.glsl");
	VelocityCompute = std::make_shared<Core::ComputeShader>("src/Shaders/particleVelocity.glsl");
}

Volume::Volume(const Volume& other)
	: Bounds(other.Bounds), ParticleCount(other.ParticleCount)
{ }

Volume::~Volume()
{
}

void Volume::Start()
{

	static std::default_random_engine e;
	static std::uniform_real_distribution<> dis(-Bounds.x / 2.0f, Bounds.x / 2.0f); // range [0, 1)

	std::shared_ptr<Core::Scene> scene = Core::Application::Instance->GetGameState()->GetCurrentScene();

	Positions = std::vector<Vector2>(ParticleCount);
	PositionsSSBO = Core::LargeShaderBuffer(Positions);
	PositionsSSBO.Bind(1);

	Velocities = std::vector<Vector2>(ParticleCount);
	VelocitiesSSBO = Core::LargeShaderBuffer(Velocities);
	VelocitiesSSBO.Bind(2);

	PredictedPositions = std::vector<Vector2>(ParticleCount);
	PredictedPositionsSSBO = Core::LargeShaderBuffer(PredictedPositions);
	PredictedPositionsSSBO.Bind(3);

	Densities = std::vector<float>(ParticleCount);
	DensitiesSSBO = Core::LargeShaderBuffer(Densities);
	DensitiesSSBO.Bind(4);

	CurrentCells = std::vector<uint32_t>(ParticleCount);
	CurrentCellsSSBO = Core::LargeShaderBuffer(CurrentCells);
	CurrentCellsSSBO.Bind(5);

	CellCoords = std::vector<Vector2Int>(ParticleCount);
	CellCoordsSSBO = Core::LargeShaderBuffer(CellCoords);
	CellCoordsSSBO.Bind(6);
	
	SpatialEntriesSSBO = Core::LargeShaderBuffer(SpatialEntries);
	SpatialEntriesSSBO.Bind(6);
	
	StartIndicesSSBO = Core::LargeShaderBuffer(StartIndices);
	StartIndicesSSBO.Bind(7);

	for (int i = 0; i < ParticleCount; i++)
		Positions[i] = Vector2(dis(e), dis(e));

	Core::Input::BindEventToKey(GLFW_KEY_E, [this]()
	{
		ClearVelocities();
	});
}

void Volume::Tick(float deltaTime)
{
	Shapes::DrawQuad(Vector3(0.0f), Bounds / 2.0f, Color3(0.03f, 0.03f, 0.03f));

	if(Core::Input::IsMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) || Core::Input::IsMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
		Shapes::DrawCircleOutline(Vector3(Vector2(Core::Input::MousePosition.x, -Core::Input::MousePosition.y) + Vector2(-(Core::Application::Instance->Width / 2.0f), Core::Application::Instance->Height / 2.0f), 0.f), GodRadius, Color3(0.05f));

	if (Core::Input::IsKeyDown(GLFW_KEY_C))
		Bounds.x = 2000;

	int substeps = 3;
	for (int i = 0; i < substeps; i++)
	{
		UpdateSpatialLookup();

		concurrency::parallel_for(0, ParticleCount, [&](int i) {
			PredictedPositions[i] = Positions[i] + Velocities[i];
			Densities[i] = CalculateDensity(i);
		});

		/*PositionCompute->Dispatch(ParticleCount, 1, 1);
		VelocityCompute->SetUniform("deltaTime", deltaTime);
		VelocityCompute->Dispatch(ParticleCount, 1, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		PositionsSSBO.ReadInto(&Positions);
		VelocitiesSSBO.ReadInto(&Velocities);*/
		
		concurrency::parallel_for(0, ParticleCount, [&](int i) {
			UpdateVelocity(i);
		});

		concurrency::parallel_for(0, ParticleCount, [&](int i) {
			
			Positions[i] += Velocities[i];
			ResolveCollisions(i);
		});

	}

	for (int i = 0; i < ParticleCount; i++)
	{
		Shapes::DrawCircle(Vector3(Positions[i], 0.0f), ParticleSize, mParticleColor.Sample((glm::length(Velocities[i]) - 0.003f) * GradientScale));
	}

	if (Core::Input::IsKeyDown(GLFW_KEY_A))
		Bounds.x -= 20.0f;

	if (Core::Input::IsKeyDown(GLFW_KEY_D))
		Bounds.x += 20.0f;
}

float Volume::CalculateDensity(int particleIndex)
{
	float density = 0.0f;

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

	Vector2Int cellCoord = GetCellCoord(PredictedPositions[particleIndex]);
	for (Vector2Int& offset : cellCoordOffsets)
	{
		int iteratingCell = GetCellKey(cellCoord + offset);
		int beginning = StartIndices[iteratingCell];

		for (int i = beginning; i < ParticleCount; i++)
		{
			int otherIndex = SpatialEntries[i].ParticleIndex;

			if (SpatialEntries[i].GridCell != iteratingCell)
				break;

			Vector2 diff = PredictedPositions[otherIndex] - PredictedPositions[particleIndex];
			float dist = glm::length(diff);

			if (dist < InfluenceRadius)
			{
				float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
				density += Smooth(InfluenceRadius, distance);
			}
		}
	}

	return density;
}

Vector2 Volume::CalculateViscosityForce(int particleIndex)
{
	Vector2 viscosityForce = Vector2(0.f);
	Vector2 position = Positions[particleIndex];

	int iteratingCell = GetCellKey(GetCellCoord(Positions[particleIndex]));
	int beginning = StartIndices[iteratingCell];

	for (int i = beginning; i < ParticleCount; i++)
	{
		if (SpatialEntries[i].GridCell != iteratingCell)
			break;

		float dist = glm::length(Positions[particleIndex] - Positions[i]);
		float influence = Smooth(InfluenceRadius, dist);
		viscosityForce += (Velocities[i] - Velocities[particleIndex]) * influence;
	}

	return viscosityForce * ViscosityStrength;
}

float Volume::SmoothDerivative(float radius, float dist)
{
	const float volume = std::numbers::pi * std::pow(radius, 5) / 10.0f;
	float v = std::max(radius - dist, 0.0f);

	return 3.0f * std::pow(v, 2.0f) * (-1.0f / volume) * ((std::numbers::pi * std::pow(radius, 4)) / 2.0f) * (-1.0f / radius);
}

void Volume::UpdateSpatialLookup()
{

	concurrency::parallel_for(0, ParticleCount, [&](int i) {
		Vector2Int cellCoord = GetCellCoord(Positions[i]);
		int cellKey = GetCellKey(cellCoord);
		SpatialEntries[i].ParticleIndex = i;
		SpatialEntries[i].GridCell = cellKey;
		StartIndices[i] = std::numeric_limits<int>::max();
		CurrentCells[i] = cellKey;
		CellCoords[i] = cellCoord;
	});


	std::sort(std::execution::par_unseq, SpatialEntries.begin(), SpatialEntries.end(), [](const SpatialGridEntry& lhs, const SpatialGridEntry& rhs)
	{
		return lhs.GridCell < rhs.GridCell;
	});
	

	concurrency::parallel_for(0, ParticleCount, [&](int i) {
		int key = SpatialEntries[i].GridCell;
		int keyPrev = i == 0 ? -1 : SpatialEntries[i - 1].GridCell;

		if (key != keyPrev)
		{
			StartIndices[key] = i;
		}
	});

	/*CellCoordsSSBO.Write(CellCoords);
	SpatialEntriesSSBO.Write(SpatialEntries);*/
}

Vector2 Volume::GetPressureForce(int particleIndex)
{
	static std::default_random_engine e;
	static std::uniform_real_distribution<> dis(-1, 1);

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

	float myPressure = CalculatePressure(particleIndex);

	for (Vector2Int& offset : cellCoordOffsets)
	{
		int iteratingCell = GetCellKey(CellCoords[particleIndex] + offset);
		int beginning = StartIndices[iteratingCell];

		for (int i = beginning; i < ParticleCount; i++)
		{
			int otherIndex = SpatialEntries[i].ParticleIndex;

			if (SpatialEntries[i].GridCell != iteratingCell)
				break;

			if (otherIndex == particleIndex)
				continue;

			Vector2 diff = PredictedPositions[otherIndex] - PredictedPositions[particleIndex];
			float dist = glm::length(diff);

			if (dist < InfluenceRadius)
			{
				Vector2 direction = dist == 0 ? Vector2(dis(e), dis(e)) : diff / dist;

				float slope = SmoothDerivative(InfluenceRadius, dist);
				float pressure = (myPressure + CalculatePressure(otherIndex)) / 2.0f;
				force += pressure * -direction * slope * 1.0f / Densities[particleIndex];
			}
		}
	}

	return force;
}

void Volume::ClearVelocities()
{
	for (auto& v : Velocities)
		v = Vector2();
}

void Volume::ResolveCollisions(int particleIndex)
{
	Vector2 halfBounds = Bounds / 2.0f;
	halfBounds -= Vector2(ParticleSize);

	if (std::abs(Positions[particleIndex].x) > halfBounds.x)
	{
		int sign = Positions[particleIndex].x < 0 ? -1 : 1;
		Velocities[particleIndex].x *= -0.4f;
		Positions[particleIndex].x = halfBounds.x * sign;
	}

	if (std::abs(Positions[particleIndex].y) > halfBounds.y)
	{
		int sign = Positions[particleIndex].y < 0 ? -1 : 1;
		Velocities[particleIndex].y *= -0.4f;
		Positions[particleIndex].y = halfBounds.y * sign;
	}
}

int Volume::GetCellKey(Vector2Int coord)
{
	//int key = std::hash<Vector2Int>()(coord) % ParticleCount;
	uint32_t a = (uint32_t)coord.x * 15823;
	uint32_t b = (uint32_t)coord.y * 9737333;
	return (a + b) % ParticleCount;
}

Vector2Int Volume::GetCellCoord(Vector2 position)
{
	int cellX = (int)(position.x / InfluenceRadius);
	int cellY = (int)(position.y / InfluenceRadius);
	return Vector2Int(cellX, cellY);
}

void Volume::UpdateVelocity(int particleIndex)
{
	float dt = 1.f / 240.f;
	Vector2 pressureForce = GetPressureForce(particleIndex);
	Vector2 pressureAcceleration = pressureForce / Densities[particleIndex];

	Velocities[particleIndex] += Vector2(0, Gravity * dt * dt) + pressureAcceleration * dt + CalculateViscosityForce(particleIndex);

	if (Core::Input::IsMouseButtonDown(GLFW_MOUSE_BUTTON_MIDDLE))
	{
		Vector2 diff = PredictedPositions[particleIndex] - (Vector2(Core::Input::MousePosition.x, -Core::Input::MousePosition.y) + Vector2(-(Core::Application::Instance->Width / 2.0f), Core::Application::Instance->Height / 2.0f));
		float mag = glm::length(diff);
		Velocities[particleIndex] -= Vector2(diff.y, -diff.x) / std::max(mag * mag, 1.0f) * TwistForce * dt;
	}

	if (Core::Input::IsMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
	{
		Vector2 diff = PredictedPositions[particleIndex] - (Vector2(Core::Input::MousePosition.x, -Core::Input::MousePosition.y) + Vector2(-(Core::Application::Instance->Width / 2.0f), Core::Application::Instance->Height / 2.0f));
		float mag = glm::length(diff);
		if (mag < GodRadius)
		{
			Velocities[particleIndex] -= (diff / mag) * PullForce * dt;
		}
	}

	if (Core::Input::IsMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
	{
		Vector2 diff = PredictedPositions[particleIndex] - (Vector2(Core::Input::MousePosition.x, -Core::Input::MousePosition.y) + Vector2(-(Core::Application::Instance->Width / 2.0f), Core::Application::Instance->Height / 2.0f));
		float mag = glm::length(diff);
		if (mag < GodRadius)
		{
			Velocities[particleIndex] += (diff / mag) * PushForce * dt;
		}
	}

	Velocities[particleIndex] += (Vector2)Core::Input::WindowPositionDelta / 10000.f;
}

float Volume::CalculatePressure(int particleIndex)
{
	float densityOffset = Densities[particleIndex] - TargetDensity;
	return densityOffset * PressurePower;
}

float Volume::Smooth(float radius, float dist)
{
	const float volume = std::numbers::pi * std::pow(radius, 5) / 10.0f;
	float v = std::max(radius - dist, 0.0f);
	return std::pow(v, 3.0f) / volume;
}

SpatialGridEntry::SpatialGridEntry()
	: ParticleIndex(0), GridCell(0)
{
}

SpatialGridEntry::SpatialGridEntry(int particleIndex, int gridCell)
	: ParticleIndex(particleIndex), GridCell(gridCell)
{ }

bool SpatialGridEntry::operator<(const SpatialGridEntry & rhs)
{
	return GridCell < rhs.GridCell;
}
