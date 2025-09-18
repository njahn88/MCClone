#pragma once
#include <memory>
#include "Signature.h"
#include "Entity.h"
#include <unordered_map>
#include "System.h"
#include <iostream>

class SystemManager
{
public:
	template<typename T>
	std::shared_ptr<T> RegisterSystem()
	{
		const char* typeName = typeid(T).name();

		assert(mSystems.find(typeName) == mSystems.end() && "Registering system more than once.");

		auto system = std::make_shared<T>();
		mSystems.insert({ typeName, system });
		return system;
	}

	template<typename T>
	void SetSignature(Signature signature)
	{
		const char* typeName = typeid(T).name();

		assert(mSystems.find(typeName) != mSystems.end() && "System used before registered.");

		mSignatures.insert({ typeName, signature });
	}

	void EntityDestroyed(Entity entity)
	{
		for (auto const& pair : mSystems)
		{
			auto const& system = pair.second;

			system->m_Entities.erase(entity);
		}
	}

	void EntitySignatureChanged(Entity entity, Signature entitySignature)
	{
		// Notify each system that an entity's signature changed
		for (auto const& pair : mSystems)
		{
			auto const& type = pair.first;
			auto const& system = pair.second;
			auto const& systemSignature = mSignatures[type];

			if ((entitySignature & systemSignature) == systemSignature)
			{
				system->m_Entities.insert(entity);
			}
			else
			{
				system->m_Entities.erase(entity);
			}
		}
	}

private:
	std::unordered_map<const char*, Signature> mSignatures{};

	std::unordered_map<const char*, std::shared_ptr<System>> mSystems{};
};

