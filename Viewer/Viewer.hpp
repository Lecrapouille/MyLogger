#pragma once

#include "Utils.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>

// ****************************************************************************
//! \brief Span structure based on OpenTelemetry Span
// ****************************************************************************
struct Span
{
    friend std::ostream& operator<<(std::ostream& os, const Span& span);

    std::string span_id;
    std::string operation_name;
    std::string service_name;
    double start_time;
    double duration;
    int depth = 0;
    std::vector<std::pair<std::string, std::string>> tags;
    std::vector<std::string> logs;
    ImU32 color = 0;
    bool selected = false;
};

// ****************************************************************************
//! \brief Trace structure based on OpenTelemetry Trace
// ****************************************************************************
struct Trace
{
    std::string trace_id;
    std::string trace_name;
    std::vector<Span> spans;
    double total_duration = 0.0;
    double start_time;
    size_t total_spans;
    std::map<std::string, ImU32> service_colors;

    // Cached min/max values for optimization
    double min_duration = 0.0;
    double max_duration = 0.0;
    double min_start_time = 0.0;
    double max_start_time = 0.0;
};

// ****************************************************************************
//! \brief Colors structure for TimelineViewer
// ****************************************************************************
struct ViewerColors
{
    //! \brief Color for selected spans
    ImU32 selected_span = IM_COL32(255, 215, 0, 255); // Gold
    //! \brief Color for span borders
    ImU32 span_border = IM_COL32(0, 0, 0, 255); // Black
    //! \brief Color for span text
    ImU32 span_text = IM_COL32(255, 255, 255, 255); // White
    //! \brief Color for timeline grid lines
    ImU32 grid_lines = IM_COL32(100, 100, 100, 255); // Dark Gray
    //! \brief Color for timeline text
    ImU32 timeline_text = IM_COL32(200, 200, 200, 255); // Light Gray
    //! \brief Color for minimap view window
    ImU32 minimap_window =
        IM_COL32(0, 0, 0, 128); // Semi-transparent Black
    //! \brief Color for minimap window border
    ImU32 minimap_border = IM_COL32(255, 255, 0, 255); // Yellow
    //! \brief Color for span hover effect
    ImU32 span_hover = IM_COL32(255, 255, 255, 100); // Semi-transparent White
    //! \brief Color for minimap background
    ImU32 minimap_bg = IM_COL32(30, 30, 30, 255); // Dark Gray
    //! \brief Color for minimap grid
    ImU32 minimap_grid = IM_COL32(100, 100, 100, 255); // Medium Gray
    //! \brief Color for minimap spans
    ImU32 minimap_span = IM_COL32(255, 255, 255, 80); // Semi-transparent White
    //! \brief Color for panel background
    ImU32 panel_bg = IM_COL32(25, 25, 25, 255); // Very Dark Gray
    //! \brief Color for panel text
    ImU32 panel_text = IM_COL32(180, 180, 180, 255); // Light Gray
    //! \brief Color for panel header
    ImU32 panel_header =
        IM_COL32(60, 60, 60, 128); // Semi-transparent Dark Gray
};

// ****************************************************************************
//! \brief Configuration structure for TimelineViewer
// ****************************************************************************
struct ViewerConfig
{
    // ===== VISUAL LAYOUT PARAMETERS =====
    //! \brief The height of each span bar in pixels
    float span_height = 20.0f;
    //! \brief The vertical spacing between span rows
    float span_spacing = 25.0f;
    //! \brief The width of the control panel
    float left_panel_width = 300.0f;

    // ===== NAVIGATION SPEED SETTINGS =====
    //! \brief The horizontal scroll distance per key press
    float horizontal_scroll_speed = 50.0f;
    //! \brief The vertical scroll distance per key press
    float vertical_scroll_speed = 25.0f;

    // ===== FILTERING SYSTEM =====
    //! \brief The filter for the service name
    std::string service_filter = "";
    //! \brief The filter for the operation name
    std::string operation_filter = "";
    //! \brief The minimum span duration filter (ms)
    float min_duration_filter = 0.0f;
    //! \brief The maximum span duration filter (ms)
    float max_duration_filter = 1000.0f;
    //! \brief The actual minimum duration value for slider bounds (ms)
    float slider_min_bound = 0.0f;
    //! \brief The actual maximum duration value for slider bounds (ms)
    float slider_max_bound = 1000.0f;
    //! \brief The minimum span start time filter (ms)
    float min_time_filter = 0.0f;
    //! \brief The maximum span start time filter (ms)
    float max_time_filter = 1000.0f;
    //! \brief The actual minimum time value for slider bounds (ms)
    float time_slider_min_bound = 0.0f;
    //! \brief The actual maximum time value for slider bounds (ms)
    float time_slider_max_bound = 1000.0f;

    // ===== MINIMAP CONFIGURATION =====
    //! \brief The height of the minimap in pixels
    float minimap_height = 80.0f;

    // ===== RENDERING CONSTANTS =====
    //! \brief Minimum width for span visibility (pixels)
    float min_span_width = 8.0f;
    //! \brief Indentation per depth level (pixels)
    float depth_indentation = 20.0f;
    //! \brief Minimum span width for text rendering (pixels)
    float min_text_width = 30.0f;
    //! \brief Text offset from span left edge (pixels)
    float text_offset_x = 4.0f;
    //! \brief Text offset from span center (pixels)
    float text_offset_y = 7.0f;

    // ===== MINIMAP RENDERING =====
    //! \brief Compact span height for minimap (pixels)
    float minimap_span_height = 3.0f;
    //! \brief Spacing between minimap levels (pixels)
    float minimap_span_spacing = 4.0f;
    //! \brief Minimum span width in minimap (pixels)
    float minimap_min_width = 2.0f;
    //! \brief Maximum depth levels shown in minimap
    int minimap_max_levels = 15;
    //! \brief Top margin for minimap spans (pixels)
    float minimap_top_margin = 5.0f;
    //! \brief Bottom margin for minimap positioning (pixels)
    float minimap_bottom_margin = 10.0f;

    // ===== TIMELINE GRID =====
    //! \brief Number of time ticks on timeline
    int timeline_ticks = 8;
    //! \brief Timeline top margin (pixels)
    float timeline_top_margin = 20.0f;
    //! \brief Timeline bottom margin (pixels)
    float timeline_bottom_margin = 20.0f;
    //! \brief Grid text offset (pixels)
    float grid_text_offset = 2.0f;
    //! \brief Grid text vertical offset (pixels)
    float grid_text_y_offset = 5.0f;

    // ===== TIMELINE LAYOUT =====
    //! \brief Minimum canvas height (pixels)
    float min_canvas_height = 100.0f;
    //! \brief Additional height buffer (pixels)
    float canvas_height_buffer = 40.0f;
    //! \brief Maximum spans for height calculation
    float max_spans_for_height = 20.0f;
    //! \brief Timeline content top offset (pixels)
    float timeline_content_offset = 60.0f;
    //! \brief Timeline content bottom margin (pixels)
    float timeline_content_margin = 30.0f;
    //! \brief Span label left margin (pixels)
    float span_label_margin = 5.0f;
    //! \brief Span label vertical offset (pixels)
    float span_label_y_offset = 8.0f;

    // ===== ZOOM AND SELECTION =====
    //! \brief Minimum selection width for zoom (pixels)
    float min_selection_width = 10.0f;
    //! \brief Zoom factor for mouse wheel
    float zoom_factor_in = 0.8f;
    //! \brief Zoom factor for mouse wheel (out)
    float zoom_factor_out = 1.25f;
    //! \brief Scroll amount as percentage of view range
    float scroll_percentage = 0.1f;

    // ===== MINIMAP DOCK REQUIREMENTS =====
    //! \brief Minimum minimap dock width (pixels)
    float minimap_dock_min_width = 100.0f;
    //! \brief Minimum minimap dock height (pixels)
    float minimap_dock_min_height = 50.0f;

    // ===== COLOR ENHANCEMENT =====
    //! \brief Brightness multiplier for minimap colors
    float minimap_brightness_factor = 1.4f;
    //! \brief Duration range buffer percentage
    float duration_buffer_percentage = 1.05f;
};

// ****************************************************************************
//! \brief Structure to store the layout information of a span
// ****************************************************************************
struct SpanLayout
{
    //! \brief Vertical position of the span
    float y_pos;
    //! \brief Horizontal start position of the span
    float start_x;
    //! \brief Horizontal end position of the span
    float end_x;
    //! \brief Width of the span
    float width;
    //! \brief Indentation based on depth
    float indent;
};

// ****************************************************************************
//! \brief TimelineViewer class
// ****************************************************************************
class TimelineViewer
{
public:

    // --------------------------------------------------------------------------
    //! \brief Load the traces from a JSON file
    //! \param p_json_data The JSON data
    //! \throw std::runtime_error if the JSON data is invalid
    // --------------------------------------------------------------------------
    void loadFromJSON(const std::string& p_json_data);

    // --------------------------------------------------------------------------
    //! \brief Load the traces from a JSON file path
    //! \param p_file_path The path to the JSON file
    //! \return The error message if the file was not loaded successfully,
    //! empty string otherwise
    // --------------------------------------------------------------------------
    std::string loadFromFile(const std::string& p_file_path);

    // --------------------------------------------------------------------------
    //! \brief Render the timeline
    // --------------------------------------------------------------------------
    void render();

    // --------------------------------------------------------------------------
    //! \brief Cache trace information and initialize filters
    // --------------------------------------------------------------------------
    void cacheTraceInformation();

private:

    // --------------------------------------------------------------------------
    //! \brief Clear all loaded traces
    // --------------------------------------------------------------------------
    void clearAll();

    // --------------------------------------------------------------------------
    //! \brief Setup the main workspace and layout
    // --------------------------------------------------------------------------
    void setupWorkspace();

    // --------------------------------------------------------------------------
    //! \brief Show the control panel with filters and trace selection
    // --------------------------------------------------------------------------
    void showControlPanel();

    // --------------------------------------------------------------------------
    //! \brief Show the minimap panel window
    // --------------------------------------------------------------------------
    void showMinimapPanel();

    // --------------------------------------------------------------------------
    //! \brief Show the main timeline panel window
    // --------------------------------------------------------------------------
    void showTimelinePanel();

    // --------------------------------------------------------------------------
    //! \brief Show the span details panel window
    // --------------------------------------------------------------------------
    void showSpanDetailsPanel() const;

    // --------------------------------------------------------------------------
    //! \brief Show the file loader panel window
    // --------------------------------------------------------------------------
    void showFileLoaderPanel();

    // --------------------------------------------------------------------------
    //! \brief Handle keyboard navigation for the timeline
    // --------------------------------------------------------------------------
    void handleKeyboardNavigation(const Trace& p_trace);

    // --------------------------------------------------------------------------
    //! \brief Display timeline grid and time markers
    // --------------------------------------------------------------------------
    void displayTimelineGrid(ImDrawList* p_draw_list,
                             ImVec2 p_canvas_pos,
                             ImVec2 p_canvas_size) const;

    // --------------------------------------------------------------------------
    //! \brief Handle mouse interactions for the timeline
    // --------------------------------------------------------------------------
    void handleMouseInteractions(ImVec2 p_canvas_pos,
                                 ImVec2 p_canvas_size,
                                 const Trace& p_trace);

    // --------------------------------------------------------------------------
    //! \brief Show the menu bar
    // --------------------------------------------------------------------------
    void showMenuBar();

    // --------------------------------------------------------------------------
    //! \brief Generate or retrieve a consistent color for a service name
    //!
    //! This function implements a deterministic color assignment system:
    //! - Uses string hashing to ensure same service always gets same color
    //! - Extracts RGB components from the hash for color generation
    //! - Caches colors in a static map for performance
    //!
    //! \param p_service_name The name of the service to get color for
    //! \return 32-bit RGBA color value (ImU32 format)
    // --------------------------------------------------------------------------
    ImU32 serviceToColor(const std::string& p_service_name);

    // --------------------------------------------------------------------------
    //! \brief Display a single span bar in the timeline
    //!
    //! This function handles the complete display of a span including:
    //! - Visibility culling (skip spans outside view window)
    //! - Position calculation with time-to-pixel conversion
    //! - Depth-based indentation for hierarchical visualization
    //! - Selection highlighting and click detection
    //! - Text display when span is wide enough
    //!
    //! \param p_span The span data to display
    //! \param p_span_index Index of this span (used for selection tracking)
    //! \param p_y_pos Vertical position where to draw the span
    //! \param p_canvas_pos Top-left corner of the drawing canvas
    //! \param p_canvas_width Width of the drawing area
    //! \param p_draw_list ImGui draw list for display commands
    // --------------------------------------------------------------------------
    void displaySpanBar(const Span& p_span,
                        size_t p_span_index,
                        float p_y_pos,
                        ImVec2 p_canvas_pos,
                        float p_canvas_width,
                        ImDrawList* p_draw_list);

    // --------------------------------------------------------------------------
    //! \brief Display the minimap overview panel
    //!
    //! The minimap provides a birds-eye view of the entire trace with:
    //! - All spans displayed in miniature with enhanced visibility
    //! - Current view window highlighted with yellow rectangle
    //! - Optimized colors (brighter) for better contrast at small size
    //! - Positioned at bottom of canvas to avoid obscuring main content
    //!
    //! \param p_canvas_pos Top-left corner of the main canvas
    //! \param p_canvas_width Width of the main canvas
    //! \param p_draw_list ImGui draw list for display
    //! \param p_canvas_height Height of the main canvas
    //! \param p_trace Reference to the trace data to visualize
    // --------------------------------------------------------------------------
    void displayMinimap(ImVec2 p_canvas_pos,
                        float p_canvas_width,
                        ImDrawList* p_draw_list,
                        float p_canvas_height,
                        const Trace& p_trace);

    // --------------------------------------------------------------------------
    //! \brief Update selected span details
    //! \param p_span_index The index of the span to select
    // --------------------------------------------------------------------------
    void updateSelectedSpanDetails(int p_span_index);

    // --------------------------------------------------------------------------
    //! \brief Check if a span passes all current filters
    //!
    //! This function tests if a span should be displayed based on:
    //! - Duration range (min/max duration filters)
    //! - Service name filter (case-insensitive partial match)
    //! - Operation name filter (case-insensitive partial match)
    //!
    //! \param p_span The span to test against current filters
    //! \return true if span passes all filters, false otherwise
    // --------------------------------------------------------------------------
    bool passesFilters(const Span& p_span) const;

    // --------------------------------------------------------------------------
    //! \brief Scroll the timeline
    // --------------------------------------------------------------------------
    void
    scrollZoom(const Trace& p_trace, ImVec2 p_canvas_pos, ImVec2 p_canvas_size);

    // --------------------------------------------------------------------------
    //! \brief Reset the zoom to the full trace duration
    // --------------------------------------------------------------------------
    void resetZoom(const Trace& p_trace);

    // --------------------------------------------------------------------------
    //! \brief Zoom the timeline from a rectangular selection
    // --------------------------------------------------------------------------
    void rectangularZoom(const Trace& p_trace,
                         ImVec2 p_canvas_pos,
                         ImVec2 p_canvas_size);

    // --------------------------------------------------------------------------
    //! \brief Select the previous span
    // --------------------------------------------------------------------------
    void selectPreviousSpan(const Trace& trace);

    // --------------------------------------------------------------------------
    //! \brief Select the next span
    // --------------------------------------------------------------------------
    void selectNextSpan(const Trace& trace);

    // --------------------------------------------------------------------------
    //! \brief Select the upper span
    // --------------------------------------------------------------------------
    void selectUpperSpan(const Trace& trace);

    // --------------------------------------------------------------------------
    //! \brief Select the lower span
    // --------------------------------------------------------------------------
    void selectLowerSpan(const Trace& trace);

    // --------------------------------------------------------------------------
    //! \brief Scroll the timeline to the left
    // --------------------------------------------------------------------------
    void scrollLeft();

    // --------------------------------------------------------------------------
    //! \brief Scroll the timeline to the right
    // --------------------------------------------------------------------------
    void scrollRight();

    // --------------------------------------------------------------------------
    //! \brief Update the view to center on a span
    // --------------------------------------------------------------------------
    void updateViewToCenterOnSpan(const Span& span);

    void displaySpanText(const std::string& p_text,
                         ImVec2 p_text_pos,
                         float p_available_width,
                         ImDrawList* p_draw_list) const;

    // --------------------------------------------------------------------------
    //! \brief Calculate the layout of a span
    // --------------------------------------------------------------------------
    SpanLayout calculateSpanLayout(const Span& span,
                                   float y_offset,
                                   size_t visual_row,
                                   ImVec2 canvas_pos,
                                   float canvas_width) const;

    // --------------------------------------------------------------------------
    //! \brief Initialize the slider bounds
    // --------------------------------------------------------------------------
    void initializeSliderBounds(double min_duration,
                               double max_duration,
                               double min_time,
                               double max_time);

    // --------------------------------------------------------------------------
    //! \brief Show the file loader panel
    // --------------------------------------------------------------------------
    void showFileLoader();

private:

    //! \brief Configuration parameters for the viewer
    ViewerConfig m_config;
    //! \brief Colors for the viewer
    ViewerColors m_colors;

    //! \brief The traces to display
    std::vector<Trace> m_traces;
    //! \brief The color map for the services
    std::map<std::string, ImU32> m_color_map;
    //! \brief The start time of the visible window (milliseconds)
    double m_view_start = 0.0;
    //! \brief The end time of the visible window (milliseconds)
    double m_view_end = 100.0;
    //! \brief The index of the currently active trace for viewing
    int m_selected_trace = 0;
    //! \brief The index of the currently selected span (-1 if none)
    int m_selected_span_index = -1;
    //! \brief The formatted details text for the selected span
    std::string m_selected_span_details = "";
    //! \brief Whether to display the minimap
    bool m_show_minimap = true;
    //! \brief Mouse selection state for rectangular zoom selection
    struct ZoomSelection
    {
        bool is_selecting = false;
        ImVec2 start;
        ImVec2 end;
    } m_zoom_selection;
};