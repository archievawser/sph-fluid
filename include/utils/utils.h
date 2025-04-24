#pragma once
#include <iostream>
#include "glm/glm.hpp"

#define RABID_FILEPATH_BARE (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define RABID_LOG(msg) std::cout << "[" << RABID_FILEPATH_BARE << ":" << __LINE__ << "] " << msg << std::endl
#define RABID_LOG_WITH_PREFIX(prefix, msg) std::cout << prefix << " "; RABID_LOG(msg)
#define RABID_ERROR(msg) RABID_LOG_WITH_PREFIX("[error]", msg)
#define RABID_WARNING(msg) RABID_LOG_WITH_PREFIX("[warning]", msg)

typedef glm::vec2 Vector2;
typedef glm::vec3 Vector3;
typedef glm::vec3 Color3;
typedef glm::mat4x4 Mat4x4;
typedef glm::ivec2 Vector2Int;
typedef glm::ivec3 Vector3Int;