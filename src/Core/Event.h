#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>

namespace Donut
{
    class Event;
    class EventDispatcher;

    enum class EventType
    {
        None = 0,

        WindowClose,
        WindowResize,
        WindowFocus,
        WindowLostFocus,
        WindowMoved,

        KeyPressed,
        KeyReleased,
        KeyTyped,

        MouseButtonPressed,
        MouseButtonReleased,
        MouseMoved,
        MouseScrolled
    };

    enum EventCategory
    {
        None                     = 0,
        EventCategoryApplication = 1 << 0,
        EventCategoryInput       = 1 << 1,
        EventCategoryKeyboard    = 1 << 2,
        EventCategoryMouse       = 1 << 3,
        EventCategoryMouseButton = 1 << 4
    };

    class Event
    {
    public:
        virtual ~Event() = default;

        bool Handled = false;

        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0;
        virtual int GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return GetName(); }

        bool IsInCategory(EventCategory category) { return GetCategoryFlags() & category; }
    };

    #define EVENT_CLASS_TYPE(type)                                                    \
        static  EventType   GetStaticType()               { return EventType::type; } \
        virtual EventType   GetEventType() const override { return GetStaticType(); } \
        virtual const char* GetName()      const override { return #type; }
    
    #define APPLICATION_EVENT_CLASS_TYPE(type) \
        EVENT_CLASS_TYPE(type)                 \
        virtual int GetCategoryFlags() const override { return EventCategoryApplication; }
    
    #define MOUSE_EVENT_CLASS_TYPE(type) \
        EVENT_CLASS_TYPE(type)           \
        virtual int GetCategoryFlags() const override { return EventCategoryMouse | \
                                                               EventCategoryInput; }
    
    #define MOUSE_BUTTON_EVENT_CLASS_TYPE(type) \
        EVENT_CLASS_TYPE(type)                  \
        virtual int GetCategoryFlags() const override { return EventCategoryMouseButton | \
                                                               EventCategoryMouse       | \
                                                               EventCategoryInput; }
    
    #define KEYBOARD_EVENT_CLASS_TYPE(type) \
        EVENT_CLASS_TYPE(type)              \
        virtual int GetCategoryFlags() const override { return EventCategoryKeyboard | \
                                                               EventCategoryInput; }

    class WindowResizeEvent
        : public Event
    {
    public:
        WindowResizeEvent(unsigned int width, unsigned int height)
            : m_Width(width), m_Height(height) { }

        unsigned int GetWidth() const { return m_Width; }
        unsigned int GetHeight() const { return m_Height; }

        std::string ToString() const override
        {
            return "WindowResizeEvent: " + std::to_string(m_Width) + ", " + std::to_string(m_Height);
        }

        APPLICATION_EVENT_CLASS_TYPE(WindowResize)

    private:
        unsigned int m_Width, m_Height;
    };

    class WindowCloseEvent 
        : public Event
    {
    public:
        WindowCloseEvent() = default;
        APPLICATION_EVENT_CLASS_TYPE(WindowClose)
    };

    class WindowFocusEvent 
        : public Event
    {
    public:
        WindowFocusEvent() = default;
        APPLICATION_EVENT_CLASS_TYPE(WindowFocus)
    };

    class WindowLostFocusEvent 
        : public Event
    {
    public:
        WindowLostFocusEvent() = default;
        APPLICATION_EVENT_CLASS_TYPE(WindowLostFocus)
    };

    class WindowMovedEvent 
        : public Event
    {
    public:
        WindowMovedEvent(int x, int y)
            : m_X(x), m_Y(y) { }

        int GetX() const { return m_X; }
        int GetY() const { return m_Y; }

        std::string ToString() const override
        {
            return "WindowMovedEvent: " + std::to_string(m_X) + ", " + std::to_string(m_Y);
        }

        APPLICATION_EVENT_CLASS_TYPE(WindowMoved)

    private:
        int m_X, m_Y;
    };

    class KeyEvent 
        : public Event
    {
    public:
        int GetKeyCode()               const          { return m_KeyCode;          }
        virtual int GetCategoryFlags() const override { return EventCategoryKeyboard | 
                                                               EventCategoryInput; }

    protected:
        KeyEvent(int keycode)
            : m_KeyCode(keycode) { }
        int m_KeyCode;
    };

    class KeyPressedEvent 
        : public KeyEvent
    {
    public:
        KeyPressedEvent(int keycode, bool isRepeat = false)
            : KeyEvent(keycode), m_IsRepeat(isRepeat) { }

        bool IsRepeat() const { return m_IsRepeat; }

        std::string ToString() const override
        {
            return "KeyPressedEvent: " + std::to_string(m_KeyCode) + 
                   " (repeat = " + std::to_string(m_IsRepeat) + ")";
        }

        KEYBOARD_EVENT_CLASS_TYPE(KeyPressed)

    private:
        bool m_IsRepeat;
    };

    class KeyReleasedEvent 
        : public KeyEvent
    {
    public:
        KeyReleasedEvent(int keycode)
            : KeyEvent(keycode) { }

        std::string ToString() const override
        {
            return "KeyReleasedEvent: " + std::to_string(m_KeyCode);
        }

        KEYBOARD_EVENT_CLASS_TYPE(KeyReleased)
    };

    class KeyTypedEvent 
        : public KeyEvent
    {
    public:
        KeyTypedEvent(int keycode)
            : KeyEvent(keycode) { }

        std::string ToString() const override
        {
            return "KeyTypedEvent: " + std::to_string(m_KeyCode);
        }

        KEYBOARD_EVENT_CLASS_TYPE(KeyTyped)
    };

    class MouseMovedEvent 
        : public Event
    {
    public:
        MouseMovedEvent(float x, float y)
            : m_MouseX(x), m_MouseY(y) { }

        float GetX() const { return m_MouseX; }
        float GetY() const { return m_MouseY; }

        std::string ToString() const override
        {
            return "MouseMovedEvent: " + std::to_string(m_MouseX) + ", " + std::to_string(m_MouseY);
        }

        MOUSE_EVENT_CLASS_TYPE(MouseMoved)

    private:
        float m_MouseX, m_MouseY;
    };

    class MouseScrolledEvent 
        : public Event
    {
    public:
        MouseScrolledEvent(float xOffset, float yOffset)
            : m_XOffset(xOffset), m_YOffset(yOffset) { }

        float GetXOffset() const { return m_XOffset; }
        float GetYOffset() const { return m_YOffset; }

        std::string ToString() const override
        {
            return "MouseScrolledEvent: " + std::to_string(m_XOffset) +
                   ", " + std::to_string(m_YOffset);
        }

        MOUSE_EVENT_CLASS_TYPE(MouseScrolled)

    private:
        float m_XOffset, m_YOffset;
    };

    class MouseButtonEvent 
        : public Event
    {
    public:
        int GetMouseButton()           const          { return m_Button;           }
        virtual int GetCategoryFlags() const override { return EventCategoryMouseButton | 
                                                               EventCategoryMouse       | 
                                                               EventCategoryInput; }

    protected:
        MouseButtonEvent(int button)
            : m_Button(button) { }
        int m_Button;
    };

    class MouseButtonPressedEvent 
        : public MouseButtonEvent
    {
    public:
        MouseButtonPressedEvent(int button)
            : MouseButtonEvent(button) { }

        std::string ToString() const override
        {
            return "MouseButtonPressedEvent: " + std::to_string(m_Button);
        }

        MOUSE_BUTTON_EVENT_CLASS_TYPE(MouseButtonPressed)
    };

    class MouseButtonReleasedEvent 
        : public MouseButtonEvent
    {
    public:
        MouseButtonReleasedEvent(int button)
            : MouseButtonEvent(button) { }

        std::string ToString() const override
        {
            return "MouseButtonReleasedEvent: " + std::to_string(m_Button);
        }

        MOUSE_BUTTON_EVENT_CLASS_TYPE(MouseButtonReleased)
    };

    class EventDispatcher
    {
    public:
        EventDispatcher(Event& event) 
            : m_Event(event) { }

        template<typename T, typename F>
        bool Dispatch(const F& func)
        {
            if (m_Event.GetEventType() == T::GetStaticType())
            {
                m_Event.Handled = func(static_cast<T&>(m_Event));
                return true;
            }
            return false;
        }

    private:
        Event& m_Event;
    };

    class EventHandler
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        EventHandler() = default;
        ~EventHandler() = default;

        void SetEventCallback(const EventCallbackFn& callback)
        {
            m_EventCallback = callback;
        }

        void OnEvent(Event& event)
        {
            if (m_EventCallback)
                m_EventCallback(event);
        }

        template<typename T>
        void BindEvent(const std::function<void(T&)>& callback)
        {
            m_EventCallback = [callback](Event& event) 
            {
                if (event.GetEventType() == T::GetStaticType())
                    callback(static_cast<T&>(event));
            };
        }

    private:
        EventCallbackFn m_EventCallback;
    };

    #undef EVENT_CLASS_TYPE
    #undef APPLICATION_EVENT_CLASS_TYPE
    #undef MOUSE_EVENT_CLASS_TYPE
    #undef MOUSE_BUTTON_EVENT_CLASS_TYPE
    #undef KEYBOARD_EVENT_CLASS_TYPE
}