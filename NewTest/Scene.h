#pragma once
#include "Game.h"


struct Scene {
private:
	const char* m_sceneName = "Scene Name";
	unsigned int m_Vao = -1;
	unsigned int m_Vbo = -1;
public:
	Scene() {
		glGenVertexArrays(1, &m_Vao);
		glBindVertexArray(m_Vao);

		glGenBuffers(1, &m_Vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);


	}

	~Scene() {
		glBindVertexArray(m_Vao);
	}
	virtual void EnterScene() = 0;
	virtual void ExitScene() = 0;
	virtual void RenderScene() = 0;
};
