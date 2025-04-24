#include <cstdint>
#include <string>
#include <memory>
#include <istream>
#include "GL/glew.h"
#include "glm/gtc/matrix_transform.hpp"
#include "core/application.h"
#include "core/geometry.h"
#include "core/shader.h"
#include "core/renderer.h"
#include "core/renderable.h"
#include "core/scene.h"
#include "core/sprite.h"
#include "core/entity.h"
#include "core/engineTestScene.h"
#include "core/shapes.h"
#include "core/clock.h"
#include "core/input.h"
#include "core/assetLoader.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "utils/utils.h"


namespace Core
{
	Application* Application::Instance;

	Application::Application(uint32_t width, uint32_t height, std::string title)
		: Width(width), Height(height), mTitle(title), mRenderer(nullptr)
	{ 
		Instance = this;
	}

	void Application::init()
	{
		if (glfwInit() != GLFW_TRUE)
		{
			RABID_ERROR("Failed to initialize GLFW");
			return;
		}

		GLFWwindow* window = glfwCreateWindow(Width, Height, mTitle.c_str(), nullptr, nullptr);

		if (!window)
		{
			RABID_ERROR("Failed to create GLFW window");
			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(window);

		mRenderer = new Renderer(window);

		if (glewInit() != GLEW_OK)
		{
			RABID_ERROR("Failed to initialize GLEW");
			glfwTerminate();
			return;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

		// Initialize static classes
		Input::Init(mRenderer->GetWindow());
		FbxUtil::Init();
		Shapes::Init();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		ImGuiStyle& style = ImGui::GetStyle();
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.07f, 1.0f);
		style.Colors[ImGuiCol_Text] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.17f, 0.17f, 0.17f, 1.0f);
		style.WindowPadding = ImVec2(5.0f, 3.0f);
		
		io.FontGlobalScale = 1.5f;
		style.ScaleAllSizes(1.5f);

		ImGui_ImplGlfw_InitForOpenGL(mRenderer->GetWindow(), true);
		ImGui_ImplOpenGL3_Init();
	}


	void Application::Start()
	{
		init();


		RABID_LOG("Application started");

		mGameState = std::make_shared<GameState>();
		mGameState->SetCurrentScene(std::make_shared<EngineTestScene>());

		Clock deltaTimer;

		GLFWwindow* window = mRenderer->GetWindow();
		glfwSwapInterval(0);
		
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		while (!glfwWindowShouldClose(window))
		{
			glClear(GL_COLOR_BUFFER_BIT);

			glfwPollEvents();

			float dt = deltaTimer.Restart();
			
			mGameState->GetCurrentScene()->Tick(dt / 1000.0f);

			Input::WindowPositionDelta = Vector2(0.f);

			glfwSwapBuffers(window);
		}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}
