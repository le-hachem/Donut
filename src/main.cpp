#include "Core/Application.h"

int main()
{
    Donut::Application* app = new Donut::Application()  ;
    app->Run();
    delete app;
    return 0;
}