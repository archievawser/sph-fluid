#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "core/engineTestScene.h"
#include "core/sprite.h"
#include "core/player.h"
#include "core/shapes.h"
#include "fluidsim/volume.h"
#include "utils/utils.h"


EngineTestScene::EngineTestScene()
{
	
}

void EngineTestScene::Start()
{
	Base::Start();
	//std::shared_ptsprite = Instantiate<Core::Sprite>("src/donut.png");
	//sprite->Transform = glm::scale(sprite->Transform, Vector3(3.0f));
	//Add(sprite);

	//Add(Instantiate<Core::Renderable>(Core::Geometry::FromObj("cube.obj"), Core::Shader::FromPath("src/Shaders/vertshader.glsl", "src/Shaders/fragshader.glsl")));

	//Add(Instantiate<Player>());
	mVolume = Instantiate<Volume>(Vector2(1200, 1200), 2000);
	Add(mVolume);
}

void EngineTestScene::Tick(float deltaTime)
{
	Base::Tick(deltaTime);

	static bool isMenuOpen = false;
	
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();

	ImGui::Begin("Fluid Simulation Menu", &isMenuOpen, 0);

	ImGui::Text("Current FPS: %f", 1.f / deltaTime);
	ImGui::Text("Average Density: %f", mVolume->AverageDensity);

	if (ImGui::CollapsingHeader("Particle Settings"))
	{
		ImGui::SliderFloat("Influence Radius", &mVolume->InfluenceRadius, 5.0f, 200.0f);
		ImGui::SliderFloat("Pressure Force", &mVolume->PressurePower, 0.0f, 1.0f);
		ImGui::SliderFloat("Target Density", &mVolume->TargetDensity, 0.0f, 0.01f);
		ImGui::SliderFloat("Viscosity", &mVolume->ViscosityStrength, 0.0f, 10.0f);
	}

	if (ImGui::CollapsingHeader("World Physics"))
	{
		ImGui::SliderFloat("Gravity", &mVolume->Gravity, 0.0f, -9000.0f);
	}

	if (ImGui::CollapsingHeader("God Powers"))
	{
		ImGui::SliderFloat("Twist Force", &mVolume->TwistForce, 0.0f, 60.0f);
		ImGui::SliderFloat("Pull Force", &mVolume->PullForce, 0.0f, 1200.0f);
		ImGui::SliderFloat("Push Force", &mVolume->PushForce, 0.0f, 1200.0f);
		ImGui::SliderFloat("God Radius", &mVolume->GodRadius, 0.0f, 1000.0f);
	}

	if (ImGui::CollapsingHeader("Debug"))
	{
		ImGui::SliderFloat("Gradient Scale", &mVolume->GradientScale, 0.f, 1.0f);
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
