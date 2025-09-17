#pragma once

#include <string>

namespace Core {

	struct ApplicationSpecification {
		std::string Name = "Application";
	};

	class Application {
	public:
		Application(const ApplicationSpecification& specification = ApplicationSpecification());
		~Application();

		void Run();
		void Stop();
	};
};
