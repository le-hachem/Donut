#include "Core/Application.h"

int main()
{
    Donut::Application* app = new Donut::Application("Donut Engine - Black Hole Simulation", 1280, 720);
    app->Run();
    delete app;
}