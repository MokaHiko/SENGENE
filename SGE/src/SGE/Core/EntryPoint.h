#ifndef ENTRYPOINT_H
#define ENTRYPOINT_H

#pragma once

#include "SGE/Core/Core.h"
#include "SGE/Core/Application.h"

extern SGE::Application* SGE::CreateApplication();
int main(int, char**) {
	SGE::Application* app = SGE::CreateApplication();
	app->Run();
	delete app;	
}

#endif