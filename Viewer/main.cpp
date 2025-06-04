#include "Application.hpp"
#include "Viewer.hpp"
#include <iostream>

//*****************************************************************************
//! \brief Application entry point
//! \details This is the main entry point of the timeline viewer application.
//! It handles:
//! - Creation of the TimelineViewer and Application instances
//! - Command line argument parsing for JSON trace files
//! - Application initialization and main loop execution
//!
//! Usage: timeline_viewer [json_file]
//!   json_file: Optional path to a JSON file containing trace data
//*****************************************************************************
int main(int argc, char* argv[])
{
    TimelineViewer viewer;
    Application app(viewer);
    std::string json_file;

    // Parse command line arguments
    if (argc > 1)
    {
        json_file = argv[1];
    }

    // Initialize the application
    if (!app.initialize())
    {
        std::cerr << "Failed to initialize application" << std::endl;
        return EXIT_FAILURE;
    }

    // Load JSON file if provided as command line argument
    if (!json_file.empty())
    {
        std::string error_message = viewer.loadFromFile(json_file);
        if (!error_message.empty())
        {
            std::cerr << error_message << std::endl;
            return EXIT_FAILURE;
        }
    }

    // Start the main application loop
    app.run();

    return EXIT_SUCCESS;
}