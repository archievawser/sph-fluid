#include "glm/gtc/matrix_transform.hpp"
#include "core/sprite.h"
#include "core/application.h"
#include "core/vertex.h"
#include "core/clock.h"
#include "core/assetLoader.h"
#include "utils/utils.h"


namespace Core
{
	Sprite::Sprite(const char* imagePath)
		: Brush(std::move(std::make_shared<Texture>(imagePath))), Transform(Mat4x4(1.0f))
	{
		ScopeTimer timer("FbxLoadTimer");

		Vertex vertices[] = {
			{ { 1, 1, 0 }, { 1, 1 }},
			{ { -1, 1, 0 }, { -1, 1 }},
			{ { -1, -1, 0 }, { -1, -1 }},
			{ { 1, -1, 0 }, { 1, -1 }}
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		mGeometry = std::make_shared<Geometry>(indices, 6, vertices, 4);

		mShader = Shader::FromPath("src/Shaders/texturedUnlitVert.glsl", "src/Shaders/texturedUnlitFrag.glsl");
		Transform = glm::translate(Transform, Vector3(0, 0, -100));
		Transform = glm::scale(Transform, Vector3(50, 50, 50));
	}

	Sprite::Sprite(const Sprite& other)
		: Base(other), Brush(std::move(std::make_shared<Texture>(other.Brush->GetSource())))
	{
		Transform = other.Transform;
		mShader = other.GetShader();
		mGeometry = other.GetGeometry();
	}

	void Sprite::Tick(float deltaTime)
	{
		std::shared_ptr<Camera> cam = Application::Instance->GetGameState()->GetCurrentScene()->CurrentCamera;
		Mat4x4 mvp = cam->Projection * cam->Transform * Transform;

		Bind();
		Brush->Bind(0);

		mShader->SetUniform("mvp", mvp);
		glDrawElements(GL_TRIANGLES, mGeometry->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
	}
}