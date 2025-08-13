#include "Log.h"
#include <filesystem>

#if defined(DONUT_WINDOWS)
    #include <windows.h>
#endif

namespace Donut 
{
    std::shared_ptr<Logger> Logger::s_Logger;

    void Logger::Init()
    {
        s_Logger = std::make_shared<Logger>();
        s_Logger->SetLogLevel(LogLevel::INFO);
        s_Logger->EnableConsoleOutput(true);
        s_Logger->EnableFileOutput(true);
        s_Logger->SetLogFile("logs/donut.log");
        
        s_Logger->LogMessage(LogLevel::INFO, "Logging system initialized");
    }

    void Logger::Shutdown()
    {
        if (s_Logger)
            s_Logger->LogMessage(LogLevel::INFO, "Shutting down logging system");
        s_Logger.reset();
    }

    std::shared_ptr<Logger> Logger::GetLogger()
    {
        return s_Logger;
    }

    Logger::Logger()
        : m_LogLevel(LogLevel::INFO), 
          m_ConsoleOutput(true), 
          m_FileOutput(false) { }

    Logger::~Logger() 
    {
        if (m_LogFile.is_open())
            m_LogFile.close();
    }

    void Logger::SetLogFile(const std::string& filename)
    {
        std::filesystem::path logPath(filename);
        std::filesystem::create_directories(logPath.parent_path());
        
        m_LogFile.open(filename, std::ios::app);
        if (!m_LogFile.is_open())
            std::cerr << "Failed to open log file: " << filename << std::endl;
    }

    std::string Logger::GetTimeStamp() 
    {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        
        return ss.str();
    }

    std::string Logger::GetLogLevelString(LogLevel level) 
    {
        switch (level) 
        {
            case LogLevel::TRACE: return "TRACE";
            case LogLevel::INFO:  return "INFO";
            case LogLevel::WARN:  return "WARN";
            case LogLevel::ERR:   return "ERROR";
            case LogLevel::FATAL: return "FATAL";
            default:              return "UNKNOWN";
        }
    }

    void Logger::SetConsoleColor(LogLevel level) 
    {
#if defined(DONUT_WINDOWS)
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        WORD color;
        
        switch (level) {
            case LogLevel::TRACE:
                color = FOREGROUND_RED   |
                        FOREGROUND_GREEN | 
                        FOREGROUND_BLUE;
                break;
            case LogLevel::INFO:
                color = FOREGROUND_GREEN | 
                        FOREGROUND_INTENSITY;
                break;
            case LogLevel::WARN:
                color = FOREGROUND_RED   | 
                        FOREGROUND_GREEN | 
                        FOREGROUND_INTENSITY;
                break;
            case LogLevel::ERR:
                color = FOREGROUND_RED | 
                        FOREGROUND_INTENSITY;
                break;
            case LogLevel::FATAL:
                color = FOREGROUND_RED  | 
                        FOREGROUND_BLUE | 
                        FOREGROUND_INTENSITY;
                break;
            default:
                color = FOREGROUND_RED   | 
                        FOREGROUND_GREEN | 
                        FOREGROUND_BLUE;
                break;
        }
        
        SetConsoleTextAttribute(hConsole, color);
#else
        const char* colorCode;
        switch (level) {
            case LogLevel::TRACE:
                colorCode = "\033[37m";
                break;
            case LogLevel::INFO:
                colorCode = "\033[32;1m";
                break;
            case LogLevel::WARN:
                colorCode = "\033[33;1m";
                break;
            case LogLevel::ERR:
                colorCode = "\033[31;1m";
                break;
            case LogLevel::FATAL:
                colorCode = "\033[35;1m";
                break;
            default:
                colorCode = "\033[37m";
                break;
        }
        std::cout << colorCode;
#endif
    }

    void Logger::ResetConsoleColor() 
    {
#if defined(DONUT_WINDOWS)
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED   | 
                                          FOREGROUND_GREEN | 
                                          FOREGROUND_BLUE);
#else
        std::cout << "\033[0m";
#endif
    }
}
