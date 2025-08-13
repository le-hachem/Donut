#include <glad/glad.h>
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

        SetupImGuiFonts();
        ApplyImGuiTheme();

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

    void Window::SetupImGuiFonts()
    {
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();
        
        m_MainFont = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Inter/static/Inter_18pt-Regular.ttf", 16.0f);
        if (!m_MainFont)
        {
            DONUT_WARN("Failed to load Inter font, falling back to default");
            m_MainFont = io.Fonts->AddFontDefault();
        }
        else
            DONUT_INFO("Successfully loaded Inter font");
        
        m_LargeFont = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Inter/static/Inter_18pt-Bold.ttf", 20.0f);
        if (!m_LargeFont)
            m_LargeFont = m_MainFont;
        
        m_SmallFont = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Inter/static/Inter_18pt-Light.ttf", 12.0f);
        if (!m_SmallFont)
            m_SmallFont = m_MainFont;
        
        io.FontDefault = m_MainFont;
        DONUT_INFO("ImGUI fonts loaded successfully");
    }

    void Window::ApplyImGuiTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        colors[ImGuiCol_Text]                   = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        colors[ImGuiCol_Border]                 = ImVec4(0.25f, 0.25f, 0.26f, 0.54f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.25f, 0.25f, 0.26f, 1.00f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.38f, 0.38f, 0.39f, 1.00f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
        colors[ImGuiCol_Button]                 = ImVec4(0.25f, 0.25f, 0.26f, 1.00f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.38f, 0.38f, 0.39f, 1.00f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        colors[ImGuiCol_Header]                 = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.25f, 0.25f, 0.26f, 1.00f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.42f, 0.42f, 0.44f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.08f, 0.08f, 0.09f, 0.97f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        colors[ImGuiCol_DockingPreview]         = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
        colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

        style.WindowPadding     = ImVec2(8, 8);
        style.FramePadding      = ImVec2(5, 2);
        style.CellPadding       = ImVec2(6, 4);
        style.ItemSpacing       = ImVec2(6, 4);
        style.ItemInnerSpacing  = ImVec2(4, 4);
        style.IndentSpacing     = 21;
        style.ScrollbarSize     = 14;
        style.GrabMinSize       = 10;

        style.WindowBorderSize  = 1;
        style.ChildBorderSize   = 1;
        style.PopupBorderSize   = 1;
        style.FrameBorderSize   = 0;
        style.TabBorderSize     = 0;

        style.WindowRounding    = 7;
        style.ChildRounding     = 4;
        style.FrameRounding     = 3;
        style.PopupRounding     = 4;
        style.ScrollbarRounding = 9;
        style.GrabRounding      = 3;
        style.TabRounding       = 4;

        style.WindowTitleAlign  = ImVec2(0.5f, 0.5f);
        style.ButtonTextAlign   = ImVec2(0.5f, 0.5f);
        style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

        style.DisplaySafeAreaPadding = ImVec2(3, 3);
        
        style.Alpha = 1.0f;
        style.DisabledAlpha = 0.6f;
        
        ImGuiIO& io = ImGui::GetIO();
        io.FontGlobalScale = 1.0f;
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