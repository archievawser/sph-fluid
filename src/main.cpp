#include <memory>
#include <string>
#include <cstdint>
#include "GL/glew.h"
#include "core/application.h"


int main()
{
	std::unique_ptr<Core::Application> app (new Core::Application(2560, 1440, "RABID"));
	app->Start();

	return 0;
}