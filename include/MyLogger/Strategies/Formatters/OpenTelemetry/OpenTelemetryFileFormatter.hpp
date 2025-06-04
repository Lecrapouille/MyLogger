#pragma once

#include "MyLogger/Strategies/LogFileFormatter.hpp"
#include "MyLogger/Strategies/Formatters/OpenTelemetry/OpenTelemetryLineFormatter.hpp"

#include "project_info.hpp"

#include <chrono>
#include <iomanip>
#include <sstream>

// *****************************************************************************
//! \brief OpenTelemetry file formatter that adds JSON headers/footers with project info.
// *****************************************************************************
class OpenTelemetryFileFormatter : public LogFileFormatter<OpenTelemetryFileFormatter, OpenTelemetryFormatter>
{
public:

    //-------------------------------------------------------------------------
    //! \brief Constructor.
    //-------------------------------------------------------------------------
    OpenTelemetryFileFormatter(OpenTelemetryFormatter& p_line_formatter,
                              const std::string& p_log_name = "application.log")
        : LogFileFormatter<OpenTelemetryFileFormatter, OpenTelemetryFormatter>(p_line_formatter)
        , m_log_name(p_log_name)
    {
    }

    //-------------------------------------------------------------------------
    //! \brief Implementation for header formatting with project information in JSON.
    //-------------------------------------------------------------------------
    std::string headerImpl()
    {
        std::ostringstream os;

        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);

        // Get compilation mode string
        std::string mode_str;
        switch (project::info::compilation::mode)
        {
            case project::info::compilation::debug:
                mode_str = "debug";
                break;
            case project::info::compilation::release:
                mode_str = "release";
                break;
            case project::info::compilation::normal:
                mode_str = "normal";
                break;
            default:
                mode_str = "unknown";
                break;
        }

        // Create JSON header with project information
        os << "{ ";
        os << "\"event\": \"log_session_started\", ";
        os << "\"timestamp\": \"" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "\", ";
        os << "\"project\": { ";
        os << "\"name\": \"" << project::info::name << "\", ";
        os << "\"application\": \"" << project::info::application::name << "\", ";
        os << "\"version\": { ";
        os << "\"major\": " << project::info::version::major << ", ";
        os << "\"minor\": " << project::info::version::minor << ", ";
        os << "\"patch\": " << project::info::version::patch << " ";
        os << "}, ";
        os << "\"compilation\": \"" << mode_str << "\", ";
        os << "\"git\": { ";
        os << "\"branch\": \"" << project::info::git::branch << "\", ";
        os << "\"sha1\": \"" << project::info::git::sha1 << "\" ";
        os << "} ";
        os << "}, ";
        os << "\"summary\": \"" << project::info::application::summary << "\" ";
        os << "}\n";

        return os.str();
    }

    //-------------------------------------------------------------------------
    //! \brief Implementation for footer formatting with project information in JSON.
    //-------------------------------------------------------------------------
    std::string footerImpl()
    {
        std::ostringstream os;

        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);

        // Create JSON footer with session end information
        os << "{ ";
        os << "\"event\": \"log_session_ended\", ";
        os << "\"timestamp\": \"" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "\", ";
        os << "\"application\": \"" << project::info::application::name << "\", ";
        os << "\"version\": \"" << project::info::version::full << "\" ";
        os << "}\n";

        return os.str();
    }

private:
    std::string m_log_name;
};