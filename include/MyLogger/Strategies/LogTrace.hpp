#pragma once

#include <chrono>
#include <initializer_list>
#include <iomanip>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <vector>

// *****************************************************************************
//! \brief Simple event structure for trace events.
// *****************************************************************************
struct Event
{
    std::string name;
    uint64_t timestamp_nanos;
    std::map<std::string, std::string> attributes;

    Event(const std::string& p_name,
          uint64_t p_timestamp_nanos,
          std::map<std::string, std::string> p_attributes = {})
        : name(p_name),
          timestamp_nanos(p_timestamp_nanos),
          attributes(std::move(p_attributes))
    {
    }
};

// *****************************************************************************
//! \brief Simple trace structure containing basic trace data.
//! This is a lightweight structure without external dependencies.
// *****************************************************************************
class Trace
{
public:

    using Attributes = std::map<std::string, std::string>;
    using Tags = std::map<std::string, std::pair<std::string, std::string>>;

    //-------------------------------------------------------------------------
    //! \brief Constructor for root trace.
    //! \param p_operation_name The name of the operation.
    //! \param p_attributes Key-value pairs for the trace attributes.
    //-------------------------------------------------------------------------
    explicit Trace(const std::string& p_operation_name,
                   std::initializer_list<std::pair<const char*, const char*>>
                       p_attributes = {})
        : m_operation_name(p_operation_name)
    {
        m_start_time_nanos = getCurrentTimeNanos();
        m_trace_id = generateTraceId();
        m_span_id = generateSpanId();
        for (const auto& [key, value] : p_attributes)
        {
            m_attributes[key] = value;
        }
    }

    //-------------------------------------------------------------------------
    //! \brief Constructor for child trace/span.
    //! \param p_parent The parent trace.
    //! \param p_operation_name The name of this span operation.
    //! \param p_attributes Key-value pairs for the trace attributes.
    //-------------------------------------------------------------------------
    Trace(Trace& p_parent,
          const std::string& p_operation_name,
          std::initializer_list<std::pair<const char*, const char*>>
              p_attributes = {})
        : m_operation_name(p_operation_name),
          m_trace_id(generateTraceId()),
          m_span_id(generateSpanId()),
          m_parent_trace_id(p_parent.m_trace_id)
    {
        m_start_time_nanos = getCurrentTimeNanos();
        for (const auto& [key, value] : p_attributes)
        {
            m_attributes[key] = value;
        }

        // Add this span to parent's children
        p_parent.m_children.push_back(std::make_shared<Trace>(*this));
    }

    //-------------------------------------------------------------------------
    //! \brief Destructor.
    //-------------------------------------------------------------------------
    ~Trace()
    {
        end();
    }

    //-------------------------------------------------------------------------
    //! \brief Add an event to this trace.
    //! \param p_name The event name.
    //! \param p_attributes Key-value pairs for the event attributes.
    //-------------------------------------------------------------------------
    void addEvent(const std::string& p_name,
                  std::initializer_list<std::pair<const char*, const char*>>
                      p_attributes = {})
    {
        auto event_time = getCurrentTimeNanos();
        Attributes attributes;
        for (const auto& [key, value] : p_attributes)
        {
            attributes[key] = value;
        }

        m_events.emplace_back(p_name, event_time, std::move(attributes));
    }

    //-------------------------------------------------------------------------
    //! \brief Add an attribute to this trace.
    //! \param p_key The attribute key.
    //! \param p_value The attribute value.
    //-------------------------------------------------------------------------
    inline void addAttribute(const std::string& p_key,
                             const std::string& p_value)
    {
        m_attributes[p_key] = p_value;
    }

    //-------------------------------------------------------------------------
    //! \brief Add a tag to this trace.
    //! \param p_key The tag key.
    //! \param p_value The tag value.
    //! \param p_value_type The tag value type (optional).
    //-------------------------------------------------------------------------
    inline void addTag(const std::string& p_key,
                       const std::string& p_value,
                       const std::string& p_value_type = {})
    {
        m_tags[p_key] = std::make_pair(p_value, p_value_type);
    }

    //-------------------------------------------------------------------------
    //! \brief End this trace/span.
    //-------------------------------------------------------------------------
    void end()
    {
        if (!m_ended)
        {
            m_end_time_nanos = getCurrentTimeNanos();
            m_ended = true;
        }
    }

    //-------------------------------------------------------------------------
    //! \brief Get duration in nanoseconds.
    //-------------------------------------------------------------------------
    uint64_t getDurationNanos() const
    {
        if (m_ended)
        {
            return m_end_time_nanos - m_start_time_nanos;
        }
        else
        {
            auto current_time = getCurrentTimeNanos();
            return current_time - m_start_time_nanos;
        }
    }

    //-------------------------------------------------------------------------
    //! \brief Create a child span.
    //! \param operation_name The name of the operation for the child span.
    //! \param p_attributes Key-value pairs for the trace attributes.
    //! \return A new child trace/span.
    //-------------------------------------------------------------------------
    std::shared_ptr<Trace>
    createChildSpan(const std::string& p_operation_name,
                    std::initializer_list<std::pair<const char*, const char*>>
                        p_attributes = {})
    {
        auto child =
            std::make_shared<Trace>(*this, p_operation_name, p_attributes);
        m_children.push_back(child);
        return child;
    }

    //-------------------------------------------------------------------------
    //! \brief Check if trace is ended.
    //-------------------------------------------------------------------------
    inline bool isEnded() const
    {
        return m_ended;
    }

    //-------------------------------------------------------------------------
    //! \brief Get the trace ID.
    //-------------------------------------------------------------------------
    const std::string& getTraceId() const
    {
        return m_trace_id;
    }

    //-------------------------------------------------------------------------
    //! \brief Get the span ID.
    //-------------------------------------------------------------------------
    const std::string& getSpanId() const
    {
        return m_span_id;
    }

    //-------------------------------------------------------------------------
    //! \brief Get the parent span ID.
    //-------------------------------------------------------------------------
    const std::string& getParentSpanId() const
    {
        return m_parent_trace_id;
    }

    //-------------------------------------------------------------------------
    //! \brief Get the operation name.
    //-------------------------------------------------------------------------
    const std::string& getOperationName() const
    {
        return m_operation_name;
    }

    //-------------------------------------------------------------------------
    //! \brief Get start time in nanoseconds since epoch.
    //-------------------------------------------------------------------------
    uint64_t getStartTimeNanos() const
    {
        return m_start_time_nanos;
    }

    //-------------------------------------------------------------------------
    //! \brief Get end time in nanoseconds since epoch.
    //-------------------------------------------------------------------------
    uint64_t getEndTimeNanos() const
    {
        return m_end_time_nanos;
    }

    //-------------------------------------------------------------------------
    //! \brief Get all tags.
    //-------------------------------------------------------------------------
    const Tags& getTags() const
    {
        return m_tags;
    }

    //-------------------------------------------------------------------------
    //! \brief Get all attributes.
    //-------------------------------------------------------------------------
    const Attributes& getAttributes() const
    {
        return m_attributes;
    }

    //-------------------------------------------------------------------------
    //! \brief Get all child spans.
    //-------------------------------------------------------------------------
    const std::vector<std::shared_ptr<Trace>>& getChildren() const
    {
        return m_children;
    }

    //-------------------------------------------------------------------------
    //! \brief Get all events.
    //-------------------------------------------------------------------------
    const std::vector<Event>& getEvents() const
    {
        return m_events;
    }

private:

    //-------------------------------------------------------------------------
    //! \brief Get current time in nanoseconds since epoch.
    //-------------------------------------------------------------------------
    static uint64_t getCurrentTimeNanos()
    {
        return static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count());
    }

    //-------------------------------------------------------------------------
    //! \brief Generate a random trace ID (16 bytes = 32 hex chars).
    //-------------------------------------------------------------------------
    static std::string generateTraceId()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint32_t> dis(0, 0xFFFFFFFF);

        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (int i = 0; i < 4; ++i)
        {
            oss << std::setw(8) << dis(gen);
        }
        return oss.str();
    }

    //-------------------------------------------------------------------------
    //! \brief Generate a random span ID (8 bytes = 16 hex chars).
    //-------------------------------------------------------------------------
    static std::string generateSpanId()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint32_t> dis(0, 0xFFFFFFFF);

        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (int i = 0; i < 2; ++i)
        {
            oss << std::setw(8) << dis(gen);
        }
        return oss.str();
    }

private:

    //! \brief The operation name
    std::string m_operation_name;
    //! \brief The trace ID (16 bytes = 32 hex chars)
    std::string m_trace_id;
    //! \brief The span ID (8 bytes = 16 hex chars)
    std::string m_span_id;
    //! \brief The parent span ID (empty for root spans)
    std::string m_parent_trace_id;
    //! \brief Attributes key-value pairs
    Attributes m_attributes;
    //! \brief Tags key-value pairs
    Tags m_tags;
    //! \brief Child spans
    std::vector<std::shared_ptr<Trace>> m_children;
    //! \brief Events
    std::vector<Event> m_events;
    //! \brief Start time in nanoseconds since epoch
    uint64_t m_start_time_nanos;
    //! \brief End time in nanoseconds since epoch
    uint64_t m_end_time_nanos;
    //! \brief Whether the trace has ended
    bool m_ended = false;
};