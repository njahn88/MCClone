#pragma once
#include "Game.h"
#include <iostream>

struct RenderGroup {
	unsigned int m_Vao = -1;
	unsigned int m_Vbo = -1;
};

struct Scene {
private:
	const char* m_sceneName = "Scene Name";
	RenderGroup* m_staticGeo = new RenderGroup{};
	RenderGroup* m_dynamicGeo = new RenderGroup{};
	RenderGroup* m_characterGeo = new RenderGroup{};
	RenderGroup* m_dynamicObj = new RenderGroup{};

	void SetUpRenderGroup(RenderGroup* renderGroup) {
		glGenVertexArrays(1, &renderGroup->m_Vao);
		glBindVertexArray(renderGroup->m_Vao);

		glGenBuffers(1, &renderGroup->m_Vbo);
		glBindBuffer(GL_ARRAY_BUFFER, renderGroup->m_Vbo);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void DestroyRenderGroup(RenderGroup* renderGroup) {
		glDeleteVertexArrays(1, &renderGroup->m_Vao);
		glDeleteBuffers(1, &renderGroup->m_Vbo);
	}
public:
	Scene() {
		SetUpRenderGroup(m_staticGeo);
		SetUpRenderGroup(m_dynamicGeo);
		SetUpRenderGroup(m_characterGeo);
		SetUpRenderGroup(m_dynamicObj);
	}

	~Scene() {
		DestroyRenderGroup(m_staticGeo);
		DestroyRenderGroup(m_dynamicGeo);
		DestroyRenderGroup(m_characterGeo);
		DestroyRenderGroup(m_dynamicObj);
	}
	//virtual void EnterScene() = 0;
	//virtual void ExitScene() = 0;
	virtual void UpdateScene(float deltaTime) = 0;
	virtual void RenderScene() = 0;
};

struct MainMenu : public Scene {
public:
	void UpdateScene(float deltaTime) override {
		std::cout << "Updating Scene" << std::endl;
	}

	void RenderScene() override {
		std::cout << "Rendering Scene" << std::endl;
	}
};
