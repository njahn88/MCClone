#pragma once
#include "Entity.h"
#include <array>
#include <unordered_map>

class IComponentArray {
public:
	virtual ~IComponentArray() = default;
	virtual void EntityDestroyed(Entity entity) = 0;
};

template<typename T>
class ComponentArray : public IComponentArray {
public:
	void InsertData(Entity entity, T component) {
		size_t newIndex = size;
		_entityToIndexMap[entity] = newIndex;
		_indexToEntityMap[newIndex] = entity;
		_componentArray[newIndex] = component;
		++size;
	}

	void RemoveData(Entity entity) {
		size_t indexOfRemovedEntity = _entityToIndexMap[entity];
		size_t indexOfLastElement = size - 1;
		_componentArray[indexOfRemovedEntity] = _componentArray[indexOfLastElement];

		Entity entityOfLastElement = _indexToEntityMap[indexOfLastElement];
		_entityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
		_indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

		_entityToIndexMap.erase(entity);
		_indexToEntityMap.erase(indexOfLastElement);

		--size;
	}

	T& GetData(Entity entity) {
		return _componentArray[_entityToIndexMap[entity]];
	}

	void EntityDestroyed(Entity entity) override {
		if (_entityToIndexMap.find(entity) != _entityToIndexMap.end()) {
			RemoveData(entity);
		}
	}
private:
	std::array<T, MAX_ENTITIES> _componentArray;

	std::unordered_map<Entity, size_t> _entityToIndexMap;
	std::unordered_map<size_t, Entity> _indexToEntityMap;

	size_t size;
};

