#version 430 core
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct SpatialEntry
{
	int ParticleIndex;
	int GridCell;
};

layout(std430, binding = 3) buffer ppBuf
{
	vec2 PredictedPositions[];
};

layout(std430, binding = 6) buffer seBuf
{
	SpatialEntry SpatialEntries[];
};

layout(std430, binding = 7) buffer siBuf
{
	int StartIndices[];
};

layout(std430, binding = 1) buffer pBuf
{
	vec2 Positions[];
};

layout(std430, binding = 2) buffer vBuf
{
	vec2 Velocities[];
};

layout(std430, binding = 4) buffer dBuf
{
	float Densities[];
};

uniform float deltaTime;

float Smooth(float radius, float dist)
{
	float volume = 3.1415926535897932384626433832795 * pow(radius, 5) / 10.0f;
	float v = max(radius - dist, 0.0f);
	return pow(v, 3.0f) / volume;
}

float SmoothDerivative(float radius, float dist)
{
	float volume = 3.1415926535897932384626433832795 * pow(radius, 5) / 10.0f;
	float v = max(radius - dist, 0.0f);

	return 3.0f * pow(v, 2.0f) * (-1.0f / volume) * ((3.1415926535897932384626433832795 * pow(radius, 4)) / 2.0f) * (-1.0f / radius);
}

int GetCellKey(vec2 coord)
{
	int a = int(coord.x * 15823);
	int b = int(coord.y * 9737333);
	return int(mod(a + b, 6000));
}

vec2 GetCellCoord(vec2 position)
{
	int cellX = int(position.x / 40.f);
	int cellY = int(position.y / 40.f);
	return vec2(cellX, cellY);
}

float CalculatePressure(int particleIndex)
{
	float densityOffset = Densities[particleIndex] - 0.05f; // target dens
	return densityOffset * 0.125f; // pressure power 
}

vec2 GetPressureForce(int particleIndex)
{
	vec2 force = vec2(0);

	vec2 cellCoordOffsets[9];

	cellCoordOffsets[0] = vec2(-1, 0);
	cellCoordOffsets[1] = vec2(-1, 1);
	cellCoordOffsets[2] = vec2(-1, -1);
	cellCoordOffsets[3] = vec2(0, 1);
	cellCoordOffsets[4] = vec2(0, -1);
	cellCoordOffsets[5] = vec2(1, 0);
	cellCoordOffsets[6] = vec2(1, 1);
	cellCoordOffsets[7] = vec2(1, -1);
	cellCoordOffsets[8] = vec2(0, 0);

	float myPressure = CalculatePressure(particleIndex);

	vec2 cellCoord = GetCellCoord(PredictedPositions[particleIndex]);
	for (int cellCoordIndex = 0; cellCoordIndex < 9; cellCoordIndex++)
	{
		vec2 offset = cellCoordOffsets[cellCoordIndex];
		int iteratingCell = GetCellKey(cellCoord + offset);
		int beginning = StartIndices[iteratingCell];

		for (int i = beginning; i < 6000; i++)
		{
			int otherIndex = SpatialEntries[i].ParticleIndex;

			if (SpatialEntries[i].GridCell != iteratingCell)
				break;

			if (otherIndex == particleIndex)
				continue;

			vec2 diff = PredictedPositions[otherIndex] - PredictedPositions[particleIndex];
			float dist = length(diff);

			if (dist < 40.f)
			{
				vec2 direction = diff / dist;

				float slope = SmoothDerivative(40.f, dist);
				float pressure = (myPressure + CalculatePressure(otherIndex)) / 2.0f;
				force += pressure * -direction * slope * 1.0f / Densities[particleIndex];
			}
		}
	}

	return force;
}

void ResolveCollisions(int particleIndex)
{
	vec2 halfBounds = vec2(1200, 1200) / 2.0f; // bounds
	halfBounds -= vec2(3);

	if (abs(Positions[particleIndex].x) > halfBounds.x)
	{
		int sign = Positions[particleIndex].x < 0 ? -1 : 1;
		Velocities[particleIndex].x *= -0.4f;
		Positions[particleIndex].x = halfBounds.x * sign;
	}

	if (abs(Positions[particleIndex].y) > halfBounds.y)
	{
		int sign = Positions[particleIndex].y < 0 ? -1 : 1;
		Velocities[particleIndex].y *= -0.4f;
		Positions[particleIndex].y = halfBounds.y * sign;
	}
}

vec2 CalculateViscosityForce(int particleIndex)
{
	vec2 viscosityForce = vec2(0.f);
	vec2 position = Positions[particleIndex];

	int iteratingCell = GetCellKey(GetCellCoord(Positions[particleIndex]));
	int beginning = StartIndices[iteratingCell];

	for (int i = beginning; i < 6000; i++)
	{
		if (SpatialEntries[i].GridCell != iteratingCell)
			break;

		float dist = length(Positions[particleIndex] - Positions[i]);
		float influence = Smooth(40.f, dist);
		viscosityForce += (Velocities[i] - Velocities[particleIndex]) * influence;
	}

	return viscosityForce * 8.f;
}

void main()
{
	int pID = int(gl_GlobalInvocationID.x);
	float dt = 1.f / 240.f;
	vec2 pressureForce = GetPressureForce(pID);
	vec2 pressureAcceleration = pressureForce / Densities[pID];

	Velocities[pID] += vec2(0, -6000.f * dt * dt) + pressureAcceleration * dt + CalculateViscosityForce(pID);

	Positions[pID] += Velocities[pID];

	ResolveCollisions(pID);
}