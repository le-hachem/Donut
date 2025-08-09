#pragma once

#include <GLFW/glfw3.h>
#include <string>

namespace Donut
{
    class Window
    {
    public:
        Window(const std::string& title, int width, int height);
        ~Window();

        bool ShouldClose() const;
        void OnUpdate() const;
    private:
        GLFWwindow* m_Window;

        std::string m_Title;
        int m_Width;
        int m_Height;
        bool m_IsClosed;
    };
}