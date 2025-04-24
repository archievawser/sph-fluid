#include <memory>
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "core/camera.h"
#include "core/application.h"
#include "core/scene.h"
#include "core/shapes.h"
#include "core/geometry.h"
#include "core/vertex.h"
#include "core/texture.h"
#include "core/assetLoader.h"
#include "utils/utils.h"


void Shapes::Init()
{
	Core::Vertex vertices[] = {
		{ {  1,  1,  0 }, {  1,  1 } },
		{ { -1,  1,  0 }, { -1,  1 } },
		{ { -1, -1,  0 }, { -1, -1 } },
		{ {  1, -1,  0 }, {  1, -1 } }
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	std::shared_ptr<Core::Geometry> quadGeometry = std::make_shared<Core::Geometry>(indices, 6, vertices, 4);
	std::shared_ptr<Core::Geometry> circleGeometry = Core::FbxUtil::Load("circle.fbx");
	std::shared_ptr<Core::Geometry> circleOutlineGeometry = Core::FbxUtil::Load("outline.fbx");

	std::shared_ptr<Core::Shader> texShader = Core::Shader::FromPath("src/Shaders/texturedUnlitVert.glsl", "src/Shaders/texturedUnlitFrag.glsl");
	std::shared_ptr<Core::Shader> colShader = Core::Shader::FromPath("src/Shaders/coloredUnlitVert.glsl", "src/Shaders/coloredUnlitFrag.glsl");

	mQuad = Core::Scene::Instantiate<Core::Renderable>(quadGeometry, colShader);
	mCircle = Core::Scene::Instantiate<Core::Renderable>(circleGeometry, colShader);
	mCircleOutline = Core::Scene::Instantiate<Core::Renderable>(circleOutlineGeometry, colShader);
}

void Shapes::DrawQuad(Vector3 position, Vector2 size, Color3 color)
{
	std::shared_ptr<Core::Camera> cam = Core::Application::Instance->GetGameState()->GetCurrentScene()->CurrentCamera;
	Mat4x4 mvp = cam->Projection * cam->Transform * glm::scale(glm::translate(Mat4x4(1.0f), position), Vector3(size, 1.0));
	mQuad->Bind();
	mQuad->GetShader()->SetUniform("color", color);
	mQuad->GetShader()->SetUniform("mvp", mvp);
	
	glDrawElements(GL_TRIANGLES, mQuad->GetGeometry()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}

void Shapes::DrawCircle(Vector3 position, float radius, Color3 color)
{
	std::shared_ptr<Core::Camera> cam = Core::Application::Instance->GetGameState()->GetCurrentScene()->CurrentCamera;
	Mat4x4 mvp = cam->Projection * cam->Transform * glm::scale(glm::translate(Mat4x4(1.0f), position), Vector3(radius));

	mCircle->Bind();
	mCircle->GetShader()->SetUniform("color", color);
	mCircle->GetShader()->SetUniform("mvp", mvp);

	glDrawElements(GL_TRIANGLES, mCircle->GetGeometry()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}

void Shapes::DrawCircleOutline(Vector3 position, float radius, Color3 color)
{
	std::shared_ptr<Core::Camera> cam = Core::Application::Instance->GetGameState()->GetCurrentScene()->CurrentCamera;
	Mat4x4 mvp = cam->Projection * cam->Transform * glm::scale(glm::translate(Mat4x4(1.0f), position), Vector3(radius));

	mCircleOutline->Bind();
	mCircleOutline->GetShader()->SetUniform("color", color);
	mCircleOutline->GetShader()->SetUniform("mvp", mvp);

	glDrawElements(GL_TRIANGLES, mCircleOutline->GetGeometry()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
}

std::shared_ptr<Core::Texture> Shapes::mCircleTex;// = Core::Scene::Instantiate<Core::Texture>("circle.png");
std::shared_ptr<Core::Renderable> Shapes::mQuad; //= Core::Scene::Instantiate<Core::Renderable>(quadGeometry, colShader);
std::shared_ptr<Core::Renderable> Shapes::mCircle;// = Core::Scene::Instantiate<Core::Renderable>(quadGeometry, texShader);
std::shared_ptr<Core::Renderable> Shapes::mCircleOutline;// = Core::Scene::Instantiate<Core::Renderable>(quadGeometry, texShader);