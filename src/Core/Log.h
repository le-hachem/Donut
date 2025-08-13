#pragma once

#include "Core/Memory.h"

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <mutex>
#include <string_view>

namespace Donut
{
    enum class LogLevel
    {
        TRACE = 0,
        INFO  = 2,
        WARN  = 3,
        ERR   = 4,
        FATAL = 5
    };

    class Logger 
    {
    public:
        static void Init();
        static void Shutdown();

        static Ref<Logger> GetLogger();

        template<typename... Args>
        static void Trace(const std::string_view& format, const Args&... args) 
        {
            auto logger = GetLogger();
            if (logger) logger->LogMessage(LogLevel::TRACE, format, args...);
        }

        template<typename... Args>
        static void Info(const std::string_view& format, const Args&... args) 
        {
            auto logger = GetLogger();
            if (logger) logger->LogMessage(LogLevel::INFO, format, args...);
        }

        template<typename... Args>
        static void Warn(const std::string_view& format, const Args&... args) 
        {
            auto logger = GetLogger();
            if (logger) logger->LogMessage(LogLevel::WARN, format, args...);
        }

        template<typename... Args>
        static void Error(const std::string_view& format, const Args&... args) 
        {
            auto logger = GetLogger();
            if (logger) logger->LogMessage(LogLevel::ERR, format, args...);
        }

        template<typename... Args>
        static void Fatal(const std::string_view& format, const Args&... args) 
        {
            auto logger = GetLogger();
            if (logger) logger->LogMessage(LogLevel::FATAL, format, args...);
        }

        template<typename... Args>
        void LogMessage(LogLevel level, const std::string_view& format, const Args&... args) 
        {
            if (level < m_LogLevel) 
                return;

            std::lock_guard<std::mutex> lock(m_Mutex);
            std::string message = FormatString(format, args...);
            std::string fullMessage = GetTimeStamp() + " [" + GetLogLevelString(level) + "] " + message;
            
            if (m_ConsoleOutput) 
            {
                SetConsoleColor(level);
                std::cout << fullMessage << std::endl;
                ResetConsoleColor();
            }
            
            if (m_FileOutput && m_LogFile.is_open()) 
            {
                m_LogFile << fullMessage << std::endl;
                m_LogFile.flush();
            }
        }

        template<typename... Args>
        std::string FormatString(const std::string_view& format, const Args&... args)
        {
            std::string result = format.data();
            std::vector<std::string> argStrings = { ToString(args)... };
            
            size_t argIndex = 0;
            size_t pos = 0;
            
            while ((pos = result.find("{}", pos)) != std::string::npos && argIndex < argStrings.size())
            {
                result.replace(pos, 2, argStrings[argIndex]);
                pos += argStrings[argIndex].length();
                argIndex++;
            }
            
            return result;
        }

        template<typename T>
        std::string ToString(const T& value)
        {
            std::stringstream ss;
            ss << value;
            return ss.str();
        }

        void SetLogLevel(LogLevel level)      { m_LogLevel      = level;  }
        void EnableConsoleOutput(bool enable) { m_ConsoleOutput = enable; }
        void EnableFileOutput(bool enable)    { m_FileOutput    = enable; }
        void SetLogFile(const std::string& filename);
    public:
        Logger();
        ~Logger();

        std::string GetTimeStamp();
        std::string GetLogLevelString(LogLevel level);
        void SetConsoleColor(LogLevel level);
        void ResetConsoleColor();
    private:
        LogLevel      m_LogLevel;
        bool          m_ConsoleOutput;
        bool          m_FileOutput;
        std::ofstream m_LogFile;
        std::mutex    m_Mutex;

        static Ref<Logger> s_Logger;
    };
}

#if defined(DONUT_DEBUG)
    #define DONUT_TRACE(format, ...)    ::Donut::Logger::Trace(format, ##__VA_ARGS__)
    #define DONUT_INFO(format, ...)     ::Donut::Logger::Info(format, ##__VA_ARGS__)
    #define DONUT_WARN(format, ...)     ::Donut::Logger::Warn(format, ##__VA_ARGS__)
    #define DONUT_ERROR(format, ...)    ::Donut::Logger::Error(format, ##__VA_ARGS__)
    #define DONUT_FATAL(format, ...)    ::Donut::Logger::Fatal(format, ##__VA_ARGS__)
#else
    #define DONUT_TRACE(format, ...)    {}
    #define DONUT_INFO(format, ...)     {}
    #define DONUT_WARN(format, ...)     {}
    #define DONUT_ERROR(format, ...)    {}
    #define DONUT_FATAL(format, ...)    {}
#endif