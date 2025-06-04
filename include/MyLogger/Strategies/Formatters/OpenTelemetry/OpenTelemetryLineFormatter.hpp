#pragma once

#include "MyLogger/Strategies/LogLineFormatter.hpp"
#include "MyLogger/Strategies/Formatters/OpenTelemetry/OpenTelemetryLevel.hpp"
#include "MyLogger/Trace.hpp"

#include <sstream>

// *****************************************************************************
//! \brief OpenTelemetry JSON line formatter that generates OpenTelemetry-compliant JSON logs.
// *****************************************************************************
class OpenTelemetryFormatter : public LogLineFormatter<OpenTelemetryFormatter>
{
public:

    //-------------------------------------------------------------------------
    //! \brief Constructor with service configuration.
    //-------------------------------------------------------------------------
    OpenTelemetryFormatter(const std::string& p_service_name, const std::string& p_service_version)
        : m_service_name(p_service_name)
        , m_service_version(p_service_version)
    {
    }

    //-------------------------------------------------------------------------
    //! \brief Implementation for formatting the beginning of a log line.
    //-------------------------------------------------------------------------
    std::string formatBeginImpl(LogLevel p_level)
    {
        // OpenTelemetry format starts with timestamp and basic info
        // We'll complete the JSON in formatMiddle where we have access to the trace
        return "";
    }

    //-------------------------------------------------------------------------
    //! \brief Implementation for formatting the middle part with trace data.
    //! This creates the complete OpenTelemetry JSON log entry.
    //-------------------------------------------------------------------------
    std::string formatMiddleImpl(const Trace& p_trace)
    {
        // Create a complete OpenTelemetry JSON log entry manually
        std::ostringstream json_entry;
        json_entry << "{"
            << "\"timestamp\":" << p_trace.getStartTimeNanos() << ","
            << "\"traceId\":\"" << p_trace.getTraceId() << "\","
            << "\"spanId\":\"" << p_trace.getSpanId() << "\",";

        // Only add parentSpanId if it's not empty
        if (!p_trace.getParentSpanId().empty())
        {
            json_entry << "\"parentSpanId\":\"" << p_trace.getParentSpanId() << "\",";
        }

        json_entry << "\"operationName\":\"" << p_trace.getOperationName() << "\","
            << "\"duration\":" << p_trace.getDurationNanos() << ","
            << "\"service\":{\"name\":\"" << m_service_name << "\",\"version\":\"" << m_service_version << "\"}";

        // Add attributes if any
        const auto& attributes = p_trace.getAttributes();
        if (!attributes.empty())
        {
            json_entry << ",\"attributes\":{";
            bool first = true;
            for (const auto& pair : attributes)
            {
                if (!first) json_entry << ",";
                json_entry << "\"" << pair.first << "\":\"" << pair.second << "\"";
                first = false;
            }
            json_entry << "}";
        }

        json_entry << "}";

        return json_entry.str();
    }

    //-------------------------------------------------------------------------
    //! \brief Implementation for formatting the end of a log line.
    //-------------------------------------------------------------------------
    std::string formatEndImpl()
    {
        return "\n";
    }

private:
    std::string m_service_name;
    std::string m_service_version;
};