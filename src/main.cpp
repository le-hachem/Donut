#include "Core/Application.h"

int main()
{
    Donut::Application* app = new Donut::Application("Donut", 1280, 720);
    app->Run();
    delete app;
}