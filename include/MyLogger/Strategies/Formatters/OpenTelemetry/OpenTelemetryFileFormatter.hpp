#pragma once

#include "MyLogger/Strategies/Formatters/OpenTelemetry/OpenTelemetryLineFormatter.hpp"
#include "MyLogger/Strategies/LogFileFormatter.hpp"

// Generated file from MyMakefile
#include "project_info.hpp"

#include <chrono>
#include <sstream>

// *****************************************************************************
//! \brief File log formatter that adds JSON headers/footers with project info.
// *****************************************************************************
class OpenTelemetryFileFormatter
    : public LogFileFormatter<OpenTelemetryFileFormatter,
                              OpenTelemetryLineFormatter>
{
public:

    //-------------------------------------------------------------------------
    //! \brief Constructor.
    //! \param p_formatter The line formatter to use.
    //! \param p_filename The name of the log file.
    //! \param p_mode The file mode (Append or Create) - defaults to Append.
    //-------------------------------------------------------------------------
    OpenTelemetryFileFormatter(OpenTelemetryLineFormatter& p_formatter,
                               const std::string& p_filename,
                               const FileMode p_mode)
        : LogFileFormatter<OpenTelemetryFileFormatter,
                           OpenTelemetryLineFormatter>(p_formatter,
                                                       p_filename,
                                                       p_mode)
    {
    }

    //-------------------------------------------------------------------------
    //! \brief Create JSON header with project information.
    //-------------------------------------------------------------------------
    std::string headerImpl() const
    {
        std::ostringstream os;

#if 0
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
#endif

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

        // Start of the OpenTelemetry file
        os << "{ \"traces\": [ ";

#if 0
        // Initial trace: project info
        os << "\"begin_log\": { ";
        os << "\"creation\": \""
           << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
           << "\", ";
        os << "\"project\": \"" << project::info::name << "\", ";
        os << "\"application\": { ";
        os << "\"name\": \"" << project::info::application::name << "\", ";
        os << "\"summary\": \"" << project::info::application::summary
           << "\", ";
        os << "\"version\": { ";
        os << "\"major\": " << project::info::version::major << ", ";
        os << "\"minor\": " << project::info::version::minor << ", ";
        os << "\"patch\": " << project::info::version::patch << " ";
        os << "}, ";
        os << "\"compilation\": \"" << mode_str << "\", ";
        os << "\"git\": { ";
        os << "\"branch\": \"" << project::info::git::branch << "\", ";
        os << "\"sha1\": \"" << project::info::git::sha1 << "\" ";
        os << "} } },";
#endif
        return os.str();
    }

    //-------------------------------------------------------------------------
    //! \brief Create JSON footer with session end information.
    //-------------------------------------------------------------------------
    std::string footerImpl() const
    {
        std::ostringstream os;

#if 0
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);

        // Final trace: project info
        os << "\"end_log\": { ";
        os << "\"event\": \"log_session_ended\", ";
        os << "\"timestamp\": \"";
        os << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        os << "\"}";
#endif

        // End of the OpenTelemetry file
        os << "] }\n";

        return os.str();
    }
};