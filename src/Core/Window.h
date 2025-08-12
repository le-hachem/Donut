#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include "Event.h"

namespace Donut
{
    class Window
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        Window(const std::string& title, int width, int height);
        ~Window();

        bool ShouldClose() const;
        void OnUpdate() const;

        void SetEventCallback(const EventCallbackFn& callback) { m_EventHandler.SetEventCallback(callback); }
        EventHandler& GetEventHandler() { return m_EventHandler; }

        unsigned int GetWidth() const { return m_Width; }
        unsigned int GetHeight() const { return m_Height; }

        void* GetNativeWindow() const { return m_Window; }

    private:
        void Init();
        void Shutdown();

        static void GLFWErrorCallback(int error, const char* description);
        static void GLFWWindowCloseCallback(GLFWwindow* window);
        static void GLFWWindowSizeCallback(GLFWwindow* window, int width, int height);
        static void GLFWWindowFocusCallback(GLFWwindow* window, int focused);
        static void GLFWWindowPosCallback(GLFWwindow* window, int xpos, int ypos);
        static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void GLFWCharCallback(GLFWwindow* window, unsigned int keycode);
        static void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void GLFWMouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
        static void GLFWCursorPosCallback(GLFWwindow* window, double xPos, double yPos);

    private:
        GLFWwindow* m_Window;
        EventHandler m_EventHandler;

        std::string m_Title;
        unsigned int m_Width;
        unsigned int m_Height;
        bool m_IsClosed;
    };
}