#include "Engine/Application.h"

#include <iostream>

int main()
{
	Application app(800, 600, "MTG Engine");

	if (!app.initialize())
	{
		return -1;
	}

	app.run();

	return 0;
}