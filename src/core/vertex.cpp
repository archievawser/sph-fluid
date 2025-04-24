#include "glm/glm.hpp"
#include "utils/utils.h"
#include "core/vertex.h"


namespace Core
{
	Vertex::Vertex(Vector3 position, Vector2 uv)
		: Position(position), UV(uv)
	{ }
}