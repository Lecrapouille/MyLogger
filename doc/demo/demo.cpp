#include "MyLogger/MyLogger.hpp"
#include "MyLogger/Strategies/Formatters/OpenTelemetry/OpenTelemetryFileFormatter.hpp"
#include "MyLogger/Strategies/Formatters/OpenTelemetry/OpenTelemetryLevel.hpp"
#include "MyLogger/Strategies/Formatters/OpenTelemetry/OpenTelemetryLineFormatter.hpp"
#include "MyLogger/Strategies/LogFileFormatter.hpp"
#include "MyLogger/Strategies/Writers/ConsoleLogWriter.hpp"
#include "MyLogger/Strategies/Writers/FileLogWriter.hpp"

#include <iostream>
#include <memory>

// *****************************************************************************
//! \brief Example demonstrating the improved template-based logging
//! architecture with simplified API.
// *****************************************************************************
class ImprovedTemplateExample
{
public:

    //-------------------------------------------------------------------------
    //! \brief Demonstrate console logging with line formatter only.
    //-------------------------------------------------------------------------
    static void demonstrateConsoleLogging()
    {
        std::cout << "=== Console Logging with Line Formatter ===" << std::endl;

        // Create a line formatter
        auto line_formatter = std::make_unique<OpenTelemetryLineFormatter>(
            "console-service", "1.0.0");

        // Create a file formatter that wraps the line formatter
        auto file_formatter = std::make_unique<OpenTelemetryFileFormatter>(
            *line_formatter, "console.json", FileMode::Append);

        // Create a console writer
        auto writer =
            std::make_unique<ConsoleLogWriter<OpenTelemetryLineFormatter>>(
                *line_formatter);

        // Create console logger
        using ConsoleWriterType = ConsoleLogWriter<OpenTelemetryLineFormatter>;
        using ConsoleLoggerType = Logger<ConsoleWriterType,
                                         OpenTelemetryFileFormatter,
                                         OpenTelemetryLineFormatter>;

        ConsoleLoggerType logger(std::move(writer),
                                 std::move(line_formatter),
                                 std::move(file_formatter));

        // Create test traces with the new API
        Trace trace1("startup_operation",
                     { { "component", "main" }, { "duration_ms", "150" } });

        // Add some attributes dynamically
        trace1.addAttribute("version", "1.0.0");
        trace1.addAttribute("environment", "development");

        // Create a nested child span
        auto child_span = trace1.createChildSpan(
            "initialization", { { "step", "config_loading" } });
        child_span->addAttribute("config_file", "/etc/myapp.conf");
        child_span->end(); // End the child span

        Trace trace2(
            "user_request",
            { { "user_id", "12345" }, { "endpoint", "/api/v1/users" } });

        // Log with different levels
        logger.log(LogLevel::INFO, trace1);
        logger.log(LogLevel::DEBUG, trace2);
        logger.log(LogLevel::ERROR, trace1);

        logger.flush();
        std::cout << "Console logging completed!\n" << std::endl;
    }

    //-------------------------------------------------------------------------
    //! \brief Demonstrate file logging with simplified API (no filename
    //! duplication).
    //-------------------------------------------------------------------------
    static void demonstrateFileLogging()
    {
        std::cout << "=== File Logging with Simplified API ===" << std::endl;

        // Create file logger
        using FileLoggerType = Logger<FileLogWriter<OpenTelemetryLineFormatter>,
                                      OpenTelemetryFileFormatter,
                                      OpenTelemetryLineFormatter>;

        // Create a line formatter
        auto line_formatter = std::make_unique<OpenTelemetryLineFormatter>(
            "file-service", "2.0.0");

        // Create a file formatter with filename and mode - NO DUPLICATION!
        auto file_formatter = std::make_unique<OpenTelemetryFileFormatter>(
            *line_formatter, "demo.json", FileMode::Create);

        // Create a file writer using the file formatter configuration
        // Filename and mode are automatically extracted from file_formatter
        auto log_writer =
            std::make_unique<FileLogWriter<OpenTelemetryLineFormatter>>(
                *file_formatter);

        FileLoggerType logger(std::move(log_writer),
                              std::move(line_formatter),
                              std::move(file_formatter));

        // Create test traces with the new API
        Trace payment_trace("payment_processing",
                            { { "amount", "99.99" },
                              { "currency", "EUR" },
                              { "transaction_id", "tx_123456" } });

        // Add child spans to payment trace
        auto validation_span = payment_trace.createChildSpan(
            "payment_validation",
            { { "card_type", "visa" }, { "validation_method", "3ds" } });
        validation_span->addAttribute("card_last_four", "1234");
        validation_span->addEvent("card_validated");

        auto processing_span = payment_trace.createChildSpan(
            "payment_processing",
            { { "gateway", "stripe" }, { "processor_id", "proc_789" } });
        processing_span->addAttribute("gateway_response_time", "120ms");
        processing_span->addEvent("payment_sent_to_gateway");
        processing_span->addEvent("payment_confirmed");

        Trace auth_trace("authentication",
                         { { "user_email", "user@example.com" },
                           { "auth_method", "oauth2" },
                           { "success", "true" } });

        // Add child spans to authentication trace
        auto token_span = auth_trace.createChildSpan(
            "token_validation",
            { { "token_type", "jwt" }, { "issuer", "auth0" } });
        token_span->addAttribute("token_expiry", "3600s");
        token_span->addEvent("token_decoded");
        token_span->addEvent("signature_verified");

        auto permission_span = auth_trace.createChildSpan(
            "permission_check",
            { { "resource", "payment_api" }, { "action", "write" } });
        permission_span->addAttribute("user_role", "premium");
        permission_span->addEvent("permissions_loaded");
        permission_span->addEvent("access_granted");

        // End spans explicitly
        validation_span->end();
        processing_span->end();
        token_span->end();
        permission_span->end();

        // Log various operations
        logger.log(LogLevel::INFO, payment_trace);
        logger.log(LogLevel::WARNING, auth_trace);
        logger.log(LogLevel::DEBUG, payment_trace);

        logger.flush();
        std::cout << "File logging completed! Check demo.json for output.\n"
                  << std::endl;
    }

    //-------------------------------------------------------------------------
    //! \brief Demonstrate improved thread safety with simplified API.
    //-------------------------------------------------------------------------
    static void demonstrateThreadSafety()
    {
        std::cout << "=== Thread Safety Demonstration ===" << std::endl;

        // Create a line formatter
        auto line_formatter = std::make_unique<OpenTelemetryLineFormatter>(
            "threaded-service", "1.0.0");

        // Create a file formatter with filename - single source of truth
        auto file_formatter = std::make_unique<OpenTelemetryFileFormatter>(
            *line_formatter, "thread-safety.json", FileMode::Append);

        // Create a file writer using the file formatter configuration
        auto writer =
            std::make_unique<FileLogWriter<OpenTelemetryLineFormatter>>(
                *file_formatter);

        using FileWriterType = FileLogWriter<OpenTelemetryLineFormatter>;
        using LoggerType = Logger<FileWriterType,
                                  OpenTelemetryFileFormatter,
                                  OpenTelemetryLineFormatter>;

        LoggerType logger(std::move(writer),
                          std::move(line_formatter),
                          std::move(file_formatter));

        // The new architecture ensures that:
        // 1. Each write operation (begin, middle, end) is atomic
        // 2. Multiple threads can safely log without message interleaving
        // 3. The lock is held for the entire log operation

        Trace thread_trace("concurrent_logging",
                           { { "thread_safe", "yes" },
                             { "lock_scope", "entire_write_operation" } });

        logger.log(LogLevel::INFO, thread_trace);

        std::cout << "Thread safety features:" << std::endl;
        std::cout << "- Single lock per write operation" << std::endl;
        std::cout << "- No lock contention between write phases" << std::endl;
        std::cout << "- Atomic log message output" << std::endl;

        logger.flush();
        std::cout << "Thread safety demonstration completed!\n" << std::endl;
    }

    //-------------------------------------------------------------------------
    //! \brief Demonstrate nested traces with parent-child relationships.
    //-------------------------------------------------------------------------
    static void demonstrateNestedTraces()
    {
        std::cout << "=== Nested Traces Demonstration ===" << std::endl;

        // Create a line formatter
        auto line_formatter = std::make_unique<OpenTelemetryLineFormatter>(
            "nested-service", "3.0.0");

        // Create a file formatter with filename - simplified API
        auto file_formatter = std::make_unique<OpenTelemetryFileFormatter>(
            *line_formatter, "nested-traces.json", FileMode::Create);

        // Create a file writer using the file formatter configuration
        auto writer =
            std::make_unique<FileLogWriter<OpenTelemetryLineFormatter>>(
                *file_formatter);

        using FileWriterType = FileLogWriter<OpenTelemetryLineFormatter>;
        using LoggerType = Logger<FileWriterType,
                                  OpenTelemetryFileFormatter,
                                  OpenTelemetryLineFormatter>;

        LoggerType logger(std::move(writer),
                          std::move(line_formatter),
                          std::move(file_formatter));

        // Create a root trace (HTTP request)
        Trace root_trace("http_request_handler",
                         { { "method", "POST" },
                           { "url", "/api/v1/orders" },
                           { "user_id", "user123" } });

        // Add some attributes to the root trace
        root_trace.addAttribute("request_id", "req_abc123");
        root_trace.addAttribute("session_id", "sess_xyz789");

        // Log the root trace
        logger.log(LogLevel::INFO, root_trace);

        // Create nested child spans
        auto auth_span = root_trace.createChildSpan(
            "authentication",
            { { "auth_method", "jwt" }, { "token_type", "bearer" } });
        auth_span->addAttribute("user_role", "premium");
        auth_span->addEvent("token_validated");
        logger.log(LogLevel::DEBUG, *auth_span);

        auto validation_span = root_trace.createChildSpan(
            "request_validation",
            { { "schema_version", "v2.1" }, { "validator", "ajv" } });
        validation_span->addAttribute("fields_count", "5");
        validation_span->addEvent("schema_loaded");
        validation_span->addEvent("validation_passed");
        logger.log(LogLevel::DEBUG, *validation_span);

        // Create a nested span within the validation span
        auto business_logic_span = validation_span->createChildSpan(
            "business_logic",
            { { "operation", "create_order" }, { "product_count", "3" } });
        business_logic_span->addAttribute("total_amount", "299.99");
        business_logic_span->addEvent("inventory_checked");
        business_logic_span->addEvent("payment_processed");
        logger.log(LogLevel::INFO, *business_logic_span);

        // Database operation as child of business logic
        auto db_span = business_logic_span->createChildSpan(
            "database_operation",
            { { "query_type", "INSERT" }, { "table", "orders" } });
        db_span->addAttribute("rows_affected", "1");
        db_span->addEvent("connection_acquired");
        db_span->addEvent("query_executed");
        logger.log(LogLevel::DEBUG, *db_span);

        // End spans explicitly to get accurate timing
        db_span->end();
        business_logic_span->end();
        validation_span->end();
        auth_span->end();

        // Log the completed root trace again to show final timing
        logger.log(LogLevel::INFO, root_trace);

        logger.flush();
        std::cout << "Nested traces demonstration completed!" << std::endl;
        std::cout
            << "Check the console output above to see the trace hierarchy:"
            << std::endl;
        std::cout << "- Root trace: " << root_trace.getTraceId() << std::endl;
        std::cout << "- Authentication span: " << auth_span->getSpanId()
                  << " (parent: " << auth_span->getParentSpanId() << ")"
                  << std::endl;
        std::cout << "- Validation span: " << validation_span->getSpanId()
                  << " (parent: " << validation_span->getParentSpanId() << ")"
                  << std::endl;
        std::cout << "- Business logic span: "
                  << business_logic_span->getSpanId()
                  << " (parent: " << business_logic_span->getParentSpanId()
                  << ")" << std::endl;
        std::cout << "- Database span: " << db_span->getSpanId()
                  << " (parent: " << db_span->getParentSpanId() << ")"
                  << std::endl;
        std::cout << std::endl;
    }

    //-------------------------------------------------------------------------
    //! \brief Demonstrate file mode options with simplified API.
    //-------------------------------------------------------------------------
    static void demonstrateFileModes()
    {
        std::cout << "=== File Modes Demonstration ===" << std::endl;

        // First, create a file with Create mode (overwrites if exists)
        {
            auto line_formatter = std::make_unique<OpenTelemetryLineFormatter>(
                "create-service", "1.0.0");

            // Specify filename and mode in ONE place only
            auto file_formatter = std::make_unique<OpenTelemetryFileFormatter>(
                *line_formatter, "filemode-demo.json", FileMode::Create);

            // FileLogWriter automatically uses the configuration from
            // file_formatter
            auto writer =
                std::make_unique<FileLogWriter<OpenTelemetryLineFormatter>>(
                    *file_formatter);

            using FileWriterType = FileLogWriter<OpenTelemetryLineFormatter>;
            using FileLoggerType = Logger<FileWriterType,
                                          OpenTelemetryFileFormatter,
                                          OpenTelemetryLineFormatter>;

            FileLoggerType logger(std::move(writer),
                                  std::move(line_formatter),
                                  std::move(file_formatter));

            Trace trace1("file_create_operation", { { "mode", "create" } });
            logger.log(LogLevel::INFO, trace1);
            logger.flush();
            std::cout << "Created new file with FileMode::Create" << std::endl;
        }

        // Then, append to the same file with Append mode
        {
            auto line_formatter = std::make_unique<OpenTelemetryLineFormatter>(
                "append-service", "1.0.0");

            // Specify filename and mode in ONE place only
            auto file_formatter = std::make_unique<OpenTelemetryFileFormatter>(
                *line_formatter, "filemode-demo.json", FileMode::Append);

            // FileLogWriter automatically uses the configuration from
            // file_formatter
            auto writer =
                std::make_unique<FileLogWriter<OpenTelemetryLineFormatter>>(
                    *file_formatter);

            using FileWriterType = FileLogWriter<OpenTelemetryLineFormatter>;
            using FileLoggerType = Logger<FileWriterType,
                                          OpenTelemetryFileFormatter,
                                          OpenTelemetryLineFormatter>;

            FileLoggerType logger(std::move(writer),
                                  std::move(line_formatter),
                                  std::move(file_formatter));

            Trace trace2("file_append_operation", { { "mode", "append" } });
            logger.log(LogLevel::INFO, trace2);
            logger.flush();
            std::cout << "Appended to existing file with FileMode::Append"
                      << std::endl;
        }

        // Append mode
        {
            auto line_formatter = std::make_unique<OpenTelemetryLineFormatter>(
                "default-service", "1.0.0");

            // No FileMode specified - defaults to Append
            auto file_formatter = std::make_unique<OpenTelemetryFileFormatter>(
                *line_formatter, "filemode-demo.json", FileMode::Append);

            // FileLogWriter automatically uses the configuration from
            // file_formatter
            auto writer =
                std::make_unique<FileLogWriter<OpenTelemetryLineFormatter>>(
                    *file_formatter);

            using FileWriterType = FileLogWriter<OpenTelemetryLineFormatter>;
            using FileLoggerType = Logger<FileWriterType,
                                          OpenTelemetryFileFormatter,
                                          OpenTelemetryLineFormatter>;

            FileLoggerType logger(std::move(writer),
                                  std::move(line_formatter),
                                  std::move(file_formatter));

            Trace trace3("file_default_operation",
                         { { "mode", "default_append" } });
            logger.log(LogLevel::INFO, trace3);
            logger.flush();
            std::cout << "Used default mode (Append for backward compatibility)"
                      << std::endl;
        }

        std::cout << "File modes demo completed! Check 'filemode-demo.json'\n"
                  << std::endl;
    }

    //-------------------------------------------------------------------------
    //! \brief Run all demonstrations.
    //-------------------------------------------------------------------------
    static void runAll()
    {
        demonstrateConsoleLogging();
        demonstrateFileLogging();
        demonstrateThreadSafety();
        demonstrateNestedTraces();
        demonstrateFileModes();
    }
};

// *****************************************************************************
//! \brief Main function to run all examples.
// *****************************************************************************
int main()
{
    try
    {
        ImprovedTemplateExample::runAll();
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}