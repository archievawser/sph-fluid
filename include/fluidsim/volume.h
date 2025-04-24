#pragma once
#include <memory>
#include <cstdint>
#include <vector>
#include "core/entity.h"
#include "core/gradient.h"
#include "core/shader.h"
#include "utils/utils.h"
#include "fluidsim/particle.h"


struct SpatialGridEntry
{
	SpatialGridEntry();
	SpatialGridEntry(int particleIndex, int gridCell);

	bool operator < (const SpatialGridEntry& rhs);

	uint32_t ParticleIndex; // get Particle instance with Volume::Particles[ParticleIndex]
	uint32_t GridCell;
};


class Volume : public Core::Entity
{
public:
	Volume(Vector2 bounds, uint32_t particleCount);
	Volume(const Volume& other);
	~Volume() noexcept;

public:
	virtual void Start() override;
	virtual void Tick(float deltaTime) override;
	float CalculateDensity(int particleIndex);
	Vector2 CalculateViscosityForce(int particleIndex);
	float Smooth(float radius, float dist);
	float SmoothDerivative(float radius, float dist);
	void UpdateSpatialLookup();
	Vector2 GetPressureForce(int particleIndex);
	void ClearVelocities();
	void ResolveCollisions(int particleIndex);
	int GetCellKey(Vector2Int coord);
	Vector2Int GetCellCoord(Vector2 position);
	void UpdateVelocity(int particleIndex);
	float CalculatePressure(int particleIndex);

public:
	Vector2 Bounds;
	int ParticleCount;
	float GodRadius;
	float GradientScale;
	float AverageDensity;
	float ViscosityStrength;
	float Gravity;
	float PullForce;
	float PushForce;
	float TwistForce;
	float PressurePower;
	float TargetDensity;
	float InfluenceRadius;
	float ParticleSize;
	std::vector<SpatialGridEntry> SpatialEntries;
	std::vector<uint32_t> StartIndices;

	/* particle data */
	std::vector<Vector2> Positions;
	std::vector<Vector2> PredictedPositions;
	std::vector<Vector2Int> CellCoords;
	std::vector<Vector2> Velocities;
	std::vector<uint32_t> CurrentCells;
	std::vector<float> Densities;

	/* handles the particle data on the GPU in compute shaders */
	Core::LargeShaderBuffer<Vector2> PositionsSSBO;
	Core::LargeShaderBuffer<Vector2> PredictedPositionsSSBO;
	Core::LargeShaderBuffer<Vector2> VelocitiesSSBO;
	Core::LargeShaderBuffer<Vector2Int> CellCoordsSSBO;
	Core::LargeShaderBuffer<float> DensitiesSSBO;
	Core::LargeShaderBuffer<SpatialGridEntry> SpatialEntriesSSBO;
	Core::LargeShaderBuffer<uint32_t> CurrentCellsSSBO;
	Core::LargeShaderBuffer<uint32_t> StartIndicesSSBO;

	std::shared_ptr<Core::ComputeShader> PositionCompute;
	std::shared_ptr<Core::ComputeShader> VelocityCompute;
	
private:
	Gradient<Color3> mParticleColor;
};