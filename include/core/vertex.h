#pragma once
#include "glm/glm.hpp"
#include "utils/utils.h"


namespace Core
{
	struct Vertex
	{
		Vertex(Vector3 position, Vector2 uv);

		Vector3 Position;
		Vector2 UV;
	};
}