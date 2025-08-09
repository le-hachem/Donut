#include "Window.h"
#include <cstdint>

namespace Donut
{
    static bool s_GLFWInitialized = false;
    static uint32_t s_GLFWWindowCount = 0;

    Window::Window(const std::string& title, int width, int height)
        : m_Title(title), m_Width(width), m_Height(height)
    {
        if (!s_GLFWInitialized)
        {
            glfwInit();
            s_GLFWInitialized = true;
        }

        m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        glfwMakeContextCurrent(m_Window);

        s_GLFWWindowCount++;
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_Window);
        s_GLFWWindowCount--;

        if (s_GLFWWindowCount == 0)
        {
            glfwTerminate();
            s_GLFWInitialized = false;
        }
    }

    void Window::OnUpdate() const
    {
        glfwPollEvents();
        glfwSwapBuffers(m_Window);
    }

    bool Window::ShouldClose() const
    {
        return glfwWindowShouldClose(m_Window);
    }
}