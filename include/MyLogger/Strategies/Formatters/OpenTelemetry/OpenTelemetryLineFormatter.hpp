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
    std::string formatBeginImpl(LogLevel /*p_level*/,
                                bool p_is_first_line) const
    {
        return p_is_first_line ? "" : ",";
    }

    //-------------------------------------------------------------------------
    //! \brief Implementation for formatting the middle part with trace data.
    //! This creates a complete trace entry with embedded spans for the viewer.
    //-------------------------------------------------------------------------
    std::string formatMiddleImpl(const Trace& p_trace) const
    {
        std::ostringstream os;

        os << "{";
        os << "\"traceID\":\"" << p_trace.getTraceId() << "\","
           << "\"traceName\":\"" << p_trace.getOperationName() << "\","
           << "\"spans\":[" << formatAllSpans(p_trace) << "]"
           << "," << formatTraceMetadata(p_trace);
        os << "}";

        return os.str();
    }

    //-------------------------------------------------------------------------
    //! \brief Implementation for formatting the end of a log line.
    //-------------------------------------------------------------------------
    std::string formatEndImpl() const
    {
        return "\n";
    }

private:

    //-------------------------------------------------------------------------
    //! \brief Format all spans (main span + children) recursively.
    //-------------------------------------------------------------------------
    std::string formatAllSpans(const Trace& p_trace) const
    {
        std::ostringstream os;

        // Format the main span
        os << formatSpan(p_trace);

        // Format all child spans
        const auto& children = p_trace.getChildren();
        for (const auto& child : children)
        {
            os << "," << formatAllSpans(*child);
        }

        return os.str();
    }

    //-------------------------------------------------------------------------
    //! \brief Format a single span with all its properties.
    //-------------------------------------------------------------------------
    std::string formatSpan(const Trace& p_trace) const
    {
        std::ostringstream os;

        os << "{" << formatBasicSpanProperties(p_trace)
           << formatTags(p_trace.getTags())
           << formatAttributes(p_trace.getAttributes())
           << formatEvents(p_trace.getEvents()) << "}";

        return os.str();
    }

    //-------------------------------------------------------------------------
    //! \brief Format basic span properties (ID, parent, operation, service,
    //! timing).
    //-------------------------------------------------------------------------
    std::string formatBasicSpanProperties(const Trace& p_trace) const
    {
        std::ostringstream os;

        // Main span data in viewer format
        os << "\"spanID\":\"" << p_trace.getSpanId() << "\",";

        // Add parent span ID (traceID) if any
        if (!p_trace.getParentSpanId().empty())
        {
            os << "\"traceID\":\"" << p_trace.getParentSpanId() << "\",";
        }

        os << "\"operationName\":\"" << p_trace.getOperationName() << "\","
           << "\"serviceName\":\"" << m_service_name << "\","
           << "\"startTime\":" << p_trace.getStartTimeNanos() << ","
           << "\"duration\":" << p_trace.getDurationNanos() << ",";

        // Add depth based on parent span existence
        int depth = p_trace.getParentSpanId().empty() ? 0 : 1;
        os << "\"depth\":" << depth;

        return os.str();
    }

    //-------------------------------------------------------------------------
    //! \brief Format span attributes as JSON object.
    //-------------------------------------------------------------------------
    std::string formatTags(const Trace::Tags& p_tags) const
    {
        if (p_tags.empty())
            return "";

        std::ostringstream os;
        std::string separator = "";

        os << ",\"tags\":{";
        for (const auto& [key, value_pair] : p_tags)
        {
            os << separator << "\"" << key << "\":{";
            os << "\"value\":\"" << value_pair.first << "\",";
            if (!value_pair.second.empty())
            {
                os << "\"type\":\"" << value_pair.second << "\"";
            }
            os << "}";
            separator = ",";
        }
        os << "}";

        return os.str();
    }

    //-------------------------------------------------------------------------
    //! \brief Format span attributes as JSON object.
    //-------------------------------------------------------------------------
    std::string formatAttributes(const Trace::Attributes& p_attributes) const
    {
        if (p_attributes.empty())
            return "";

        std::ostringstream os;
        std::string separator = "";

        os << ",\"attributes\":{";
        for (const auto& [key, value] : p_attributes)
        {
            os << separator << "\"" << key << "\":\"" << value << "\"";
            separator = ",";
        }
        os << "}";

        return os.str();
    }

    //-------------------------------------------------------------------------
    //! \brief Format span events as JSON array.
    //-------------------------------------------------------------------------
    std::string formatEvents(const std::vector<Event>& p_events) const
    {
        if (p_events.empty())
            return "";

        std::ostringstream os;
        std::string separator = "";

        os << ",\"events\":[";
        for (const auto& event : p_events)
        {
            os << separator << "{\"name\":\"" << event.name
               << "\",\"timestamp\":" << event.timestamp_nanos
               << formatAttributes(event.attributes);
            os << "}";

            separator = ",";
        }
        os << "]";

        return os.str();
    }

    //-------------------------------------------------------------------------
    //! \brief Format trace-level metadata (timing and span count).
    //-------------------------------------------------------------------------
    std::string formatTraceMetadata(const Trace& p_trace) const
    {
        std::ostringstream os;

        os << "\"startTime\":" << p_trace.getStartTimeNanos() << ","
           << "\"total_duration\":" << p_trace.getDurationNanos() << ","
           << "\"total_spans\":" << p_trace.getChildren().size() + 1u;

        return os.str();
    }

private:

    std::string m_service_name;
    std::string m_service_version;
};