#pragma once
#include "System.h"
#include <iostream>
#include "Coordinator.h"
#include "Components.h"

extern Coordinator coordinator;

class RenderSystem : public System {
public:
	void Tick() {
		for (const auto& entity : m_Entities) {
			//std::cout << "Rendering" << std::endl;
		}
	}
};