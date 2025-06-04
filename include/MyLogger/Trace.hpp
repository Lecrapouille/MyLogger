#pragma once

#include <chrono>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <initializer_list>

// Forward declaration of implementation
class TraceImpl;

// *****************************************************************************
//! \brief OpenTelemetry-style Trace class with support for nested spans.
//! Uses PIMPL pattern to hide external dependencies.
// *****************************************************************************
class Trace
{
public:

    //-------------------------------------------------------------------------
    //! \brief Constructor for root trace.
    //! \param operation_name The name of the operation.
    //! \param p_pairs Key-value pairs for the trace attributes.
    //-------------------------------------------------------------------------
    explicit Trace(const std::string& operation_name,
                   std::initializer_list<std::pair<const char*, const char*>> p_pairs = {});

    //-------------------------------------------------------------------------
    //! \brief Constructor for child trace/span.
    //! \param parent The parent trace.
    //! \param operation_name The name of this span operation.
    //! \param p_pairs Key-value pairs for the trace attributes.
    //-------------------------------------------------------------------------
    Trace(Trace& parent,
          const std::string& operation_name,
          std::initializer_list<std::pair<const char*, const char*>> p_pairs = {});

    //-------------------------------------------------------------------------
    //! \brief Copy constructor.
    //-------------------------------------------------------------------------
    Trace(const Trace& other);

    //-------------------------------------------------------------------------
    //! \brief Move constructor.
    //-------------------------------------------------------------------------
    Trace(Trace&& other) noexcept;

    //-------------------------------------------------------------------------
    //! \brief Copy assignment operator.
    //-------------------------------------------------------------------------
    Trace& operator=(const Trace& other);

    //-------------------------------------------------------------------------
    //! \brief Move assignment operator.
    //-------------------------------------------------------------------------
    Trace& operator=(Trace&& other) noexcept;

    //-------------------------------------------------------------------------
    //! \brief Destructor - automatically ends the trace.
    //-------------------------------------------------------------------------
    ~Trace();

    //-------------------------------------------------------------------------
    //! \brief Create a child span.
    //! \param operation_name The name of the operation for the child span.
    //! \param p_pairs Key-value pairs for the trace attributes.
    //! \return A new child trace/span.
    //-------------------------------------------------------------------------
    std::shared_ptr<Trace> createChildSpan(
        const std::string& operation_name,
        std::initializer_list<std::pair<const char*, const char*>> p_pairs = {});

    //-------------------------------------------------------------------------
    //! \brief Add an attribute to this trace.
    //! \param key The attribute key.
    //! \param value The attribute value.
    //-------------------------------------------------------------------------
    void addAttribute(const std::string& key, const std::string& value);

    //-------------------------------------------------------------------------
    //! \brief Add an event to this trace.
    //! \param name The event name.
    //! \param attributes Optional attributes for the event.
    //-------------------------------------------------------------------------
    void addEvent(const std::string& name,
                  std::initializer_list<std::pair<const char*, const char*>> attributes = {});

    //-------------------------------------------------------------------------
    //! \brief End this trace/span.
    //-------------------------------------------------------------------------
    void end();

    //-------------------------------------------------------------------------
    //! \brief Get the trace ID.
    //-------------------------------------------------------------------------
    const std::string& getTraceId() const;

    //-------------------------------------------------------------------------
    //! \brief Get the span ID.
    //-------------------------------------------------------------------------
    const std::string& getSpanId() const;

    //-------------------------------------------------------------------------
    //! \brief Get the parent span ID.
    //-------------------------------------------------------------------------
    const std::string& getParentSpanId() const;

    //-------------------------------------------------------------------------
    //! \brief Get the operation name.
    //-------------------------------------------------------------------------
    const std::string& getOperationName() const;

    //-------------------------------------------------------------------------
    //! \brief Get start time in nanoseconds since epoch.
    //-------------------------------------------------------------------------
    uint64_t getStartTimeNanos() const;

    //-------------------------------------------------------------------------
    //! \brief Get end time in nanoseconds since epoch.
    //-------------------------------------------------------------------------
    uint64_t getEndTimeNanos() const;

    //-------------------------------------------------------------------------
    //! \brief Get duration in nanoseconds.
    //-------------------------------------------------------------------------
    uint64_t getDurationNanos() const;

    //-------------------------------------------------------------------------
    //! \brief Check if trace is ended.
    //-------------------------------------------------------------------------
    bool isEnded() const;

    //-------------------------------------------------------------------------
    //! \brief Get all child spans.
    //-------------------------------------------------------------------------
    const std::vector<std::shared_ptr<Trace>>& getChildren() const;

    //-------------------------------------------------------------------------
    //! \brief Get all attributes.
    //-------------------------------------------------------------------------
    const std::map<std::string, std::string>& getAttributes() const;

    //-------------------------------------------------------------------------
    //! \brief Get JSON representation (for backward compatibility).
    //! \return JSON string representation of the trace.
    //-------------------------------------------------------------------------
    std::string getJsonString() const;

private:
    //! \brief Pointer to implementation (PIMPL pattern)
    std::unique_ptr<TraceImpl> m_pImpl;

    //! \brief Friend class for accessing private members during construction
    friend class TraceImpl;
};