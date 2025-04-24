#pragma once
#include "core/texture.h"
#include "core/renderable.h"
#include "utils/utils.h"


class Shapes
{
public:
	Shapes() = delete;
	Shapes(const Shapes& other) = delete;
	~Shapes() = delete;

public:
	static void Init();
	static void DrawQuad(Vector3 position, Vector2 size, Color3 color);
	static void DrawCircle(Vector3 position, float radius, Color3 color);
	static void DrawCircleOutline(Vector3 position, float radius, Color3 color);

private:
	static std::shared_ptr<Core::Renderable> mQuad;
	static std::shared_ptr<Core::Renderable> mCircle;
	static std::shared_ptr<Core::Renderable> mCircleOutline;
	static std::shared_ptr<Core::Texture> mCircleTex;
};