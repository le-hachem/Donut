#include "Core/Window.h"

int main()
{   
    Donut::Window window("Donut", 1280, 720);

    while (!window.ShouldClose())
    {
        window.OnUpdate();
    }
}