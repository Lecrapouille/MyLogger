#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

// Forward declaration
class TimelineViewer;

//*****************************************************************************
//! \brief Application class
//! \details This class is responsible for initializing the application with
//! GLFW and ImGui, and running the main loop.
//*****************************************************************************
class Application
{
public:

    //-------------------------------------------------------------------------
    //! \brief Constructor.
    //! \details Initializes the application with a default window size of
    //! 1280x720.
    //-------------------------------------------------------------------------
    explicit Application(TimelineViewer& p_viewer);

    //-------------------------------------------------------------------------
    //! \brief Destructor.
    //! \details Cleans up the application resources.
    //-------------------------------------------------------------------------
    ~Application();

    //-------------------------------------------------------------------------
    //! \brief Initialize the application.
    //! \details Initializes the application with a given window size and title.
    //! \param p_width Window width.
    //! \param p_height Window height.
    //! \param p_title Window title.
    //! \return True if the application is initialized successfully, false
    //! otherwise.
    //-------------------------------------------------------------------------
    bool initialize(int p_width = 1280,
                    int p_height = 720,
                    const char* p_title = "Timeline Viewer");

    //-------------------------------------------------------------------------
    //! \brief Main application loop.
    //! \details Runs the main application loop.
    //-------------------------------------------------------------------------
    void run();

    //-------------------------------------------------------------------------
    //! \brief Cleanup resources.
    //! \details Cleans up the application resources.
    //-------------------------------------------------------------------------
    void cleanup();

private:

    //-------------------------------------------------------------------------
    //! \brief Initialize GLFW.
    //! \details Initializes GLFW.
    //-------------------------------------------------------------------------
    bool initializeGLFW();

    //-------------------------------------------------------------------------
    //! \brief Initialize ImGui.
    //! \details Initializes ImGui.
    //-------------------------------------------------------------------------
    bool initializeImGui();

    // Setup ImGui style
    void setupImGuiStyle();

private:

    //! \brief Reference to the timeline viewer.
    TimelineViewer& m_viewer;
    //! \brief GLFW window and context.
    GLFWwindow* m_window;
    //! \brief Window dimensions.
    int m_width;
    //! \brief Window height.
    int m_height;
};