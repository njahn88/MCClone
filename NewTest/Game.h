#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "Scene.h"
#include "Coordinator.h"
#include "Systems.h"

struct GameSpecifications {
	int m_windowWidth = 500;
	int m_windowHeight = 500;
	int m_frameRateLimit = 0;
	const char* m_gameName = "Game Title";
};


class Game {
private:
	bool m_gameRunning = true;
	GLFWwindow* m_window = nullptr;
	Scene* m_currentScene = nullptr;
	Coordinator* m_coordinator = nullptr;
	std::shared_ptr<RenderSystem> m_renderSystem;
public:
	Game(const GameSpecifications& gameSpecifications = GameSpecifications());
	~Game();

	void Run();
	void Stop();
};
