#include "Application.hpp"
#include "Viewer.hpp"
#include <iostream>

// --------------------------------------------------------------------------
// GLFW error callback
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// --------------------------------------------------------------------------
Application::Application(TimelineViewer& p_viewer)
    : m_viewer(p_viewer), m_window(nullptr), m_width(1280), m_height(720)
{
}

// --------------------------------------------------------------------------
Application::~Application()
{
    cleanup();
}

// --------------------------------------------------------------------------
bool Application::initialize(int width, int height, const char* title)
{
    m_width = width;
    m_height = height;

    if (!initializeGLFW())
        return false;

    if (!initializeImGui())
        return false;

    return true;
}

// --------------------------------------------------------------------------
bool Application::initializeGLFW()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        std::cerr << "Error during initialization of GLFW" << std::endl;
        return false;
    }

    // Configure OpenGL context version (3.3 Core Profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create main application window
    m_window =
        glfwCreateWindow(m_width, m_height, "Timeline Viewer", NULL, NULL);
    if (!m_window)
    {
        std::cerr << "Error during creation of GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // Enable V-Sync

    return true;
}

// --------------------------------------------------------------------------
bool Application::initializeImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Enable modern ImGui features
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable window docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable multi-viewport

    ImGui::StyleColorsDark(); // Use dark theme
    setupImGuiStyle();

    // Initialize platform/renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    return true;
}

// --------------------------------------------------------------------------
void Application::setupImGuiStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiIO const& io = ImGui::GetIO();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f; // Square corners for platform windows
        style.Colors[ImGuiCol_WindowBg].w = 1.0f; // Opaque window backgrounds
    }
}

// --------------------------------------------------------------------------
void Application::run()
{
    while (!glfwWindowShouldClose(m_window))
    {
        // Poll events (keyboard, mouse, window events)
        glfwPollEvents();

        // Start new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render the timeline viewer if it exists
        m_viewer.render();

        // Setup OpenGL viewport and clear screen
        int display_w;
        int display_h;
        glfwGetFramebufferSize(m_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f); // Light blue background
        glClear(GL_COLOR_BUFFER_BIT);

        // Finalize ImGui rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Handle multi-viewport rendering (for floating windows)
        ImGuiIO const& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        // Present rendered frame
        glfwSwapBuffers(m_window);
    }
}

// --------------------------------------------------------------------------
void Application::cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (m_window)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }

    glfwTerminate();
}