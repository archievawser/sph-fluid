#pragma once
#include <cstdint>
#include <string>
#include <memory>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "core/renderer.h"
#include "core/gameState.h"
#include "utils/utils.h"


namespace Core
{
	class Application
	{
	public:
		Application(uint32_t width, uint32_t height, std::string title);
		Application(const Application& other) = delete;

	public:
		void Start();
		inline const Renderer& GetRenderer() const { return *mRenderer; }
		inline const std::shared_ptr<GameState> GetGameState() const { return mGameState; }

	public:
		static Application* Instance;
		uint32_t Width;
		uint32_t Height;

	private:
		void init();

	private:
		std::string mTitle;
		std::shared_ptr<GameState> mGameState;
		Renderer* mRenderer;
		
	};
}