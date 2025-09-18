#pragma once
#include "Entity.h"
#include "Signature.h"
#include <queue>
#include <array>

class EntityManager {
public:
	EntityManager() {
		for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
			_availableEntities.push(entity);
		}
	}

	Entity CreateEntity() {
		Entity entity = _availableEntities.front();
		_availableEntities.pop();
		return entity;
	}

	void DestroyEntity(Entity entity) {
		_signatures[entity].reset();
		_availableEntities.push(entity);
	}

	void SetSignature(Entity entity, Signature signature) {
		_signatures[entity] = signature;
	}

	Signature GetSignature(Entity entity) {
		return _signatures[entity];
	}
private:
	std::queue<Entity> _availableEntities{};
	std::array<Signature, MAX_ENTITIES> _signatures{};
};
