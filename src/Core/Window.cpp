#include "Window.h"

#include <cstdint>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace Donut
{
    static bool     s_GLFWInitialized = false;
    static uint32_t s_GLFWWindowCount = 0;

    Window::Window(const std::string& title, int width, int height)
        : m_Title(title), m_Width(width), 
          m_Height(height), m_IsClosed(false)
    {
        Init();
    }

    Window::~Window()
    {
        Shutdown();
    }

    void Window::Init()
    {
        DONUT_INFO("Initializing window: ", m_Title, " (", m_Width, "x", m_Height, ")");
        
        if (!s_GLFWInitialized)
        {
            int success = glfwInit();
            if (!success)
            {
                DONUT_ERROR("Could not initialize GLFW!");
                return;
            }

            s_GLFWInitialized = true;
            DONUT_INFO("GLFW initialized successfully");
        }

        m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
        if (!m_Window)
        {
            DONUT_ERROR("Could not create GLFW window!");
            glfwTerminate();
            return;
        }
        
        DONUT_INFO("GLFW window created successfully");

        glfwMakeContextCurrent(m_Window);
        glfwSetWindowUserPointer(m_Window, this);

        glfwSetErrorCallback(GLFWErrorCallback);
        glfwSetWindowCloseCallback(m_Window, GLFWWindowCloseCallback);
        glfwSetWindowSizeCallback(m_Window,  GLFWWindowSizeCallback);
        glfwSetWindowFocusCallback(m_Window, GLFWWindowFocusCallback);
        glfwSetWindowPosCallback(m_Window,   GLFWWindowPosCallback);
        glfwSetKeyCallback(m_Window,         GLFWKeyCallback);
        glfwSetCharCallback(m_Window,        GLFWCharCallback);
        glfwSetMouseButtonCallback(m_Window, GLFWMouseButtonCallback);
        glfwSetScrollCallback(m_Window,      GLFWMouseScrollCallback);
        glfwSetCursorPosCallback(m_Window,   GLFWCursorPosCallback);

        s_GLFWWindowCount++;
    }

    void Window::Shutdown()
    {
        DONUT_INFO("Shutting down window: ", m_Title);
        
        ShutdownImGui();
        
        glfwDestroyWindow(m_Window);
        s_GLFWWindowCount--;

        if (s_GLFWWindowCount == 0)
        {
            glfwTerminate();
            s_GLFWInitialized = false;
            DONUT_INFO("GLFW terminated (no more windows)");
        }
    }

    void Window::OnUpdate() const
    {
        glfwPollEvents();
        glfwSwapBuffers(m_Window);
    }

    bool Window::ShouldClose() const
    {
        return glfwWindowShouldClose(m_Window) || m_IsClosed;
    }

    void Window::SetCursorLocked(bool locked)
    {
        m_CursorLocked = locked;
        glfwSetInputMode(m_Window, GLFW_CURSOR, locked ? 
                         GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }

    void Window::SetCursorVisible(bool visible)
    {
        m_CursorVisible = visible;
        glfwSetInputMode(m_Window, GLFW_CURSOR, visible ? 
                         GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
    }

    void Window::InitImGui()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
        ImGui_ImplOpenGL3_Init("#version 130");
        
        DONUT_INFO("ImGUI initialized successfully");
    }

    void Window::ShutdownImGui()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        
        DONUT_INFO("ImGUI shutdown");
    }

    void Window::BeginImGuiFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Window::EndImGuiFrame()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void Window::GLFWErrorCallback(int error, const char* description)
    {
        DONUT_ERROR("GLFW Error (", error, "): ", description);
    }

    void Window::GLFWWindowCloseCallback(GLFWwindow* window)
    {
        Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        WindowCloseEvent event;
        win->m_EventHandler.OnEvent(event);
    }

    void Window::GLFWWindowSizeCallback(GLFWwindow* window, int width, int height)
    {
        Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        win->m_Width = width;
        win->m_Height = height;
        
        WindowResizeEvent event(width, height);
        win->m_EventHandler.OnEvent(event);
    }

    void Window::GLFWWindowFocusCallback(GLFWwindow* window, int focused)
    {
        Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        
        if (focused)
        {
            WindowFocusEvent event;
            win->m_EventHandler.OnEvent(event);
        }
        else
        {
            WindowLostFocusEvent event;
            win->m_EventHandler.OnEvent(event);
        }
    }

    void Window::GLFWWindowPosCallback(GLFWwindow* window, int xpos, int ypos)
    {
        Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        WindowMovedEvent event(xpos, ypos);
        win->m_EventHandler.OnEvent(event);
    }

    void Window::GLFWKeyCallback(GLFWwindow* window, int key, int scancode, 
                                 int action, int mods)
    {
        Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        
        switch (action)
        {
            case GLFW_PRESS:
            {
                KeyPressedEvent event(key, false);
                win->m_EventHandler.OnEvent(event);
            } break;
            case GLFW_RELEASE:
            {
                KeyReleasedEvent event(key);
                win->m_EventHandler.OnEvent(event);
            } break;
            case GLFW_REPEAT:
            {
                KeyPressedEvent event(key, true);
                win->m_EventHandler.OnEvent(event);
            } break;
        }
    }

    void Window::GLFWCharCallback(GLFWwindow* window, unsigned int keycode)
    {
        Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        KeyTypedEvent event(keycode);
        win->m_EventHandler.OnEvent(event);
    }

    void Window::GLFWMouseButtonCallback(GLFWwindow* window, int button, 
                                         int action, int mods)
    {
        Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        
        switch (action)
        {
            case GLFW_PRESS:
            {
                MouseButtonPressedEvent event(button);
                win->m_EventHandler.OnEvent(event);
            } break;
            case GLFW_RELEASE:
            {
                MouseButtonReleasedEvent event(button);
                win->m_EventHandler.OnEvent(event);
            } break;
        }
    }

    void Window::GLFWMouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
    {
        Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        MouseScrolledEvent event((float)xOffset, (float)yOffset);
        win->m_EventHandler.OnEvent(event);
    }

    void Window::GLFWCursorPosCallback(GLFWwindow* window, double xPos, double yPos)
    {
        Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
        MouseMovedEvent event((float)xPos, (float)yPos);
        win->m_EventHandler.OnEvent(event);
    }
}