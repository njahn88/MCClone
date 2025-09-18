#pragma once
#include "Entity.h"
#include <set>

struct Scene {
	std::set<Entity> m_entities{};
};
