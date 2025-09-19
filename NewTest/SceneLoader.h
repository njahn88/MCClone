#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include "Scene.h"
#include "inc/json.hpp"

using json = nlohmann::json;

struct SceneLoader {
private:
	Coordinator* m_coordinator;
	std::vector<Scene> m_sceneData{};
public:
	SceneLoader(Coordinator* coordinator) : m_coordinator(coordinator) {}
	void LoadScenes() { //Load all components for all scenes
		std::ifstream file("ECSData.txt");

		if (!file.is_open()) {
			std::cerr << "Failed to open ECSData.txt";
			return;
		}
		
		json j;
		file >> j;

		for (auto& scene : j["scenes"]) {
			Scene newScene;
			std::cout << "Scene being init" << std::endl;
			for (auto& entity : scene["entities"]) {
				Entity newEntity = m_coordinator->CreateEntity();
				newScene.m_entities.insert(newEntity);

				std::cout << "Entity " << newEntity << " being init: " << std::endl;

				if (entity.contains("transform")) {
					Transform transform;

					auto& t = entity["transform"];

					float px = t["position"]["x"];
					float py = t["position"]["y"];
					float pz = t["position"]["z"];

					float rx = t["rotation"]["x"];
					float ry = t["rotation"]["y"];
					float rz = t["rotation"]["z"];

					float sx = t["scale"]["x"];
					float sy = t["scale"]["y"];
					float sz = t["scale"]["z"];

					transform.m_position = Position{ px, py, pz };
					transform.m_rotation = Rotation{ px, py, pz };
					transform.m_scale = Scale{ px, py, pz };

					m_coordinator->AddComponent<Transform>(
						newEntity,
						transform
					);
					std::cout << "Entity pos: " << px << ", " << py << ", " << pz << "\n";
					std::cout << "Entity rot: " << rx << ", " << ry << ", " << rz << "\n";
					std::cout << "Entity scale: " << sx << ", " << sy << ", " << sz << "\n";
				}
			}
			m_sceneData.push_back(newScene);
		}
	}
};
