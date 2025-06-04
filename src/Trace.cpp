#include "MyLogger/Trace.hpp"

#include "../external/json/single_include/nlohmann/json.hpp"
using json = nlohmann::json;

#include <chrono>
#include <random>
#include <iomanip>
#include <sstream>

// *****************************************************************************
//! \brief Implementation class for Trace (PIMPL pattern).
// *****************************************************************************
class TraceImpl
{
public:
    struct Event
    {
        std::string name;
        uint64_t timestamp_nanos;
        json attributes;
    };

    std::string m_operation_name;
    std::string m_trace_id;
    std::string m_span_id;
    std::string m_parent_span_id;
    Trace* m_parent_trace;

    json m_json;  // For backward compatibility
    std::map<std::string, std::string> m_attributes;
    std::vector<Event> m_events;
    std::vector<std::shared_ptr<Trace>> m_children;

    std::chrono::steady_clock::time_point m_start_time;
    std::chrono::steady_clock::time_point m_end_time;
    uint64_t m_start_time_epoch;
    uint64_t m_end_time_epoch;
    bool m_ended = false;

    //-------------------------------------------------------------------------
    //! \brief Constructor for root trace.
    //-------------------------------------------------------------------------
    TraceImpl(const std::string& p_operation_name,
              std::initializer_list<std::pair<const char*, const char*>> p_pairs)
        : m_operation_name(p_operation_name)
        , m_trace_id(generateTraceId())
        , m_span_id(generateSpanId())
        , m_parent_span_id("")
        , m_parent_trace(nullptr)
        , m_json(p_pairs)
    {
        m_start_time = std::chrono::steady_clock::now();
        m_start_time_epoch = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
    }

    //-------------------------------------------------------------------------
    //! \brief Constructor for child trace/span.
    //-------------------------------------------------------------------------
    TraceImpl(Trace& parent,
              const std::string& p_operation_name,
              std::initializer_list<std::pair<const char*, const char*>> p_pairs)
        : m_operation_name(p_operation_name)
        , m_trace_id(parent.m_pImpl->m_trace_id)  // Inherit trace ID from parent
        , m_span_id(generateSpanId())
        , m_parent_span_id(parent.m_pImpl->m_span_id)
        , m_parent_trace(&parent)
        , m_json(p_pairs)
    {
        m_start_time = std::chrono::steady_clock::now();
        m_start_time_epoch = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
    }

    //-------------------------------------------------------------------------
    //! \brief Copy constructor.
    //-------------------------------------------------------------------------
    TraceImpl(const TraceImpl& other)
        : m_operation_name(other.m_operation_name)
        , m_trace_id(other.m_trace_id)
        , m_span_id(other.m_span_id)
        , m_parent_span_id(other.m_parent_span_id)
        , m_parent_trace(other.m_parent_trace)
        , m_json(other.m_json)
        , m_attributes(other.m_attributes)
        , m_events(other.m_events)
        , m_children(other.m_children)
        , m_start_time(other.m_start_time)
        , m_end_time(other.m_end_time)
        , m_start_time_epoch(other.m_start_time_epoch)
        , m_end_time_epoch(other.m_end_time_epoch)
        , m_ended(other.m_ended)
    {
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

    //-------------------------------------------------------------------------
    //! \brief Add an event to this trace.
    //-------------------------------------------------------------------------
    void addEvent(const std::string& p_name,
                  std::initializer_list<std::pair<const char*, const char*>> p_attributes)
    {
        auto event_time = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());

        json event_obj;
        if (p_attributes.size() > 0)
        {
            event_obj = json(p_attributes);
        }
        else
        {
            event_obj = json{{"name", p_name.c_str()}};
        }

        m_events.push_back({p_name, event_time, std::move(event_obj)});
    }

    //-------------------------------------------------------------------------
    //! \brief End this trace/span.
    //-------------------------------------------------------------------------
    void end()
    {
        if (!m_ended)
        {
            m_end_time = std::chrono::steady_clock::now();
            m_end_time_epoch = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());
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
            return m_end_time_epoch - m_start_time_epoch;
        }
        else
        {
            auto current_time = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count());
            return current_time - m_start_time_epoch;
        }
    }
};

// *****************************************************************************
// Trace implementation
// *****************************************************************************

Trace::Trace(const std::string& p_operation_name,
             std::initializer_list<std::pair<const char*, const char*>> p_pairs)
    : m_pImpl(std::make_unique<TraceImpl>(p_operation_name, p_pairs))
{
}

Trace::Trace(Trace& p_parent,
             const std::string& p_operation_name,
             std::initializer_list<std::pair<const char*, const char*>> p_pairs)
    : m_pImpl(std::make_unique<TraceImpl>(p_parent, p_operation_name, p_pairs))
{
    // Add this span to parent's children
    p_parent.m_pImpl->m_children.push_back(std::make_shared<Trace>(*this));
}

Trace::Trace(const Trace& p_other)
    : m_pImpl(std::make_unique<TraceImpl>(*p_other.m_pImpl))
{
}

Trace::Trace(Trace&& p_other) noexcept
    : m_pImpl(std::move(p_other.m_pImpl))
{
}

Trace& Trace::operator=(const Trace& p_other)
{
    if (this != &p_other)
    {
        m_pImpl = std::make_unique<TraceImpl>(*p_other.m_pImpl);
    }
    return *this;
}

Trace& Trace::operator=(Trace&& p_other) noexcept
{
    if (this != &p_other)
    {
        m_pImpl = std::move(p_other.m_pImpl);
    }
    return *this;
}

Trace::~Trace()
{
    if (m_pImpl && !m_pImpl->m_ended)
    {
        m_pImpl->end();
    }
}

std::shared_ptr<Trace> Trace::createChildSpan(const std::string& p_operation_name,
    std::initializer_list<std::pair<const char*, const char*>> p_pairs)
{
    auto child = std::make_shared<Trace>(*this, p_operation_name, p_pairs);
    m_pImpl->m_children.push_back(child);
    return child;
}

void Trace::addAttribute(const std::string& p_key, const std::string& p_value)
{
    m_pImpl->m_attributes[p_key] = p_value;
}

void Trace::addEvent(const std::string& p_name,
                     std::initializer_list<std::pair<const char*, const char*>> p_attributes)
{
    m_pImpl->addEvent(p_name, p_attributes);
}

void Trace::end()
{
    m_pImpl->end();
}

const std::string& Trace::getTraceId() const
{
    return m_pImpl->m_trace_id;
}

const std::string& Trace::getSpanId() const
{
    return m_pImpl->m_span_id;
}

const std::string& Trace::getParentSpanId() const
{
    return m_pImpl->m_parent_span_id;
}

const std::string& Trace::getOperationName() const
{
    return m_pImpl->m_operation_name;
}

uint64_t Trace::getStartTimeNanos() const
{
    return m_pImpl->m_start_time_epoch;
}

uint64_t Trace::getEndTimeNanos() const
{
    return m_pImpl->m_end_time_epoch;
}

uint64_t Trace::getDurationNanos() const
{
    return m_pImpl->getDurationNanos();
}

bool Trace::isEnded() const
{
    return m_pImpl->m_ended;
}

const std::vector<std::shared_ptr<Trace>>& Trace::getChildren() const
{
    return m_pImpl->m_children;
}

const std::map<std::string, std::string>& Trace::getAttributes() const
{
    return m_pImpl->m_attributes;
}

std::string Trace::getJsonString() const
{
    return m_pImpl->m_json.dump();
}