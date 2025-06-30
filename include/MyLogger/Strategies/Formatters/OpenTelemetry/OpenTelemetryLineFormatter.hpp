#pragma once

#include "MyLogger/Strategies/Formatters/OpenTelemetry/OpenTelemetryLevel.hpp"
#include "MyLogger/Strategies/LogLineFormatter.hpp"
#include "MyLogger/Strategies/LogTrace.hpp"

#include <sstream>

// *****************************************************************************
//! \brief Log line formatter that generates viewer-compatible OpenTelemetry
//! JSON. This formatter creates trace entries with embedded spans for direct
//! viewer compatibility.
// *****************************************************************************
class OpenTelemetryLineFormatter
    : public LogLineFormatter<OpenTelemetryLineFormatter>
{
public:

    //-------------------------------------------------------------------------
    //! \brief Constructor with service configuration.
    //-------------------------------------------------------------------------
    OpenTelemetryLineFormatter(const std::string& p_service_name,
                               const std::string& p_service_version)
        : m_service_name(p_service_name), m_service_version(p_service_version)
    {
    }

    //-------------------------------------------------------------------------
    //! \brief Implementation for formatting the beginning of a log line.
    //-------------------------------------------------------------------------
    std::string formatBeginImpl(LogLevel /*p_level*/) const
    {
        // No prefix needed for viewer-compatible format
        return "";
    }

    //-------------------------------------------------------------------------
    //! \brief Implementation for formatting the middle part with trace data.
    //! This creates a complete trace entry with embedded spans for the viewer.
    //-------------------------------------------------------------------------
    std::string formatMiddleImpl(const Trace& p_trace) const
    {
        std::ostringstream json_entry;

        // Create a trace entry with embedded spans (viewer-compatible format)
        json_entry << R"({)"
                   << R"("traceId":")" << p_trace.getTraceId() << R"(",)"
                   << R"("traceName":")" << p_trace.getOperationName()
                   << R"(",)"
                   << R"("spans":[{)";

        // Main span data in viewer format
        json_entry << R"("spanId":")" << p_trace.getSpanId() << R"(",)";

        // Add parentSpanId if any
        if (!p_trace.getParentSpanId().empty())
        {
            json_entry << R"("parentSpanId":")" << p_trace.getParentSpanId()
                       << R"(",)";
        }

        json_entry << R"("operationName":")" << p_trace.getOperationName()
                   << R"(",)"
                   << R"("serviceName":")" << m_service_name << R"(",)"
                   << R"("startTime":)" << p_trace.getStartTimeNanos() << R"(,)"
                   << R"("duration":)" << p_trace.getDurationNanos();

        // Add depth based on parent span existence
        int depth = p_trace.getParentSpanId().empty() ? 0 : 1;
        json_entry << R"(,"depth":)" << depth;

        // Add attributes if any
        if (const auto& attributes = p_trace.getAttributes();
            !attributes.empty())
        {
            json_entry << R"(,"attributes":{)";
            bool first = true;
            for (const auto& [key, value] : attributes)
            {
                if (!first)
                    json_entry << R"(,)";
                json_entry << R"(")" << key << R"(":")" << value << R"(")";
                first = false;
            }
            json_entry << R"(})";
        }

        // Add events if any
        if (const auto& events = p_trace.getEvents(); !events.empty())
        {
            json_entry << R"(,"events":[)";
            bool first_event = true;
            for (const auto& event : events)
            {
                if (!first_event)
                    json_entry << R"(,)";

                json_entry << R"({"name":")" << event.name
                           << R"(","timestamp":)" << event.timestamp_nanos;

                // Add event attributes if any
                if (!event.attributes.empty())
                {
                    json_entry << R"(,"attributes":{)";
                    bool first_attr = true;
                    for (const auto& [key, value] : event.attributes)
                    {
                        if (!first_attr)
                            json_entry << R"(,)";
                        json_entry << R"(")" << key << R"(":")" << value
                                   << R"(")";
                        first_attr = false;
                    }
                    json_entry << R"(})";
                }

                json_entry << R"(})";
                first_event = false;
            }
            json_entry << R"(])";
        }

        // Close the span and add trace-level metadata
        json_entry << R"(}],)";

        // Add trace-level properties
        json_entry << R"("startTime":)" << p_trace.getStartTimeNanos() << R"(,)"
                   << R"("total_duration":)" << p_trace.getDurationNanos()
                   << R"(,)"
                   << R"("total_spans":1)";

        json_entry << R"(})";

        return json_entry.str();
    }

    //-------------------------------------------------------------------------
    //! \brief Implementation for formatting the end of a log line.
    //-------------------------------------------------------------------------
    std::string formatEndImpl() const
    {
        return ",\n";
    }

private:

    std::string m_service_name;
    std::string m_service_version;
};