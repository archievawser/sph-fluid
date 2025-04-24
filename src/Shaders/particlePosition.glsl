#version 430 core
layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

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

float CalculateDensity(int particleIndex)
{
	float density = 0.f;

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

			vec2 diff = PredictedPositions[otherIndex] - PredictedPositions[particleIndex];
			float dist = length(diff);

			if (dist < 40.f)
			{
				float distance = sqrt(diff.x * diff.x + diff.y * diff.y);
				density += Smooth(40.f, distance);
			}
		}
	}

	return density;
}

void main()
{
	int pID = int(gl_GlobalInvocationID.x);

	PredictedPositions[pID] = Positions[pID] + Velocities[pID];
	Densities[pID] = CalculateDensity(pID);
}