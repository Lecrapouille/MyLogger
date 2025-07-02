#include "Viewer.hpp"
#include "Utils.hpp"

#include "imgui_stdlib.h"

#include <filesystem>
#include <format>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>

// --------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const Span& span)
{
    os << "Span {\n"
       << "  ID: " << span.span_id << "\n"
       << "  Operation: " << span.operation_name << "\n"
       << "  Service: " << span.service_name << "\n"
       << "  Start Time: " << span.start_time << "ns\n"
       << "  Duration: " << span.duration << "ns\n"
       << "  Depth: " << span.depth << "\n";

    if (!span.tags.empty())
    {
        os << "  Tags:\n";
        for (const auto& [key, value] : span.tags)
        {
            os << "    " << key << ": " << value << "\n";
        }
    }

    if (!span.logs.empty())
    {
        os << "  Logs:\n";
        for (const auto& log : span.logs)
        {
            os << "    " << log << "\n";
        }
    }

    os << "}";
    return os;
}

// --------------------------------------------------------------------------
void TimelineViewer::clearAll()
{
    // Clear all loaded traces
    m_traces.clear();
    m_color_map.clear();
    m_selected_trace = 0;
    m_selected_span_index = -1;
    m_selected_span_details.clear();

    // Reset view configuration
    m_view_start = 0.0;
    m_view_end = 100.0;

    // Reset filters
    m_config.service_filter.clear();
    m_config.operation_filter.clear();
    m_config.min_duration_filter = 0.0f;
    m_config.max_duration_filter = 1000.0f;
    m_config.slider_min_bound = 0.0f;
    m_config.slider_max_bound = 1000.0f;
    m_config.min_time_filter = 0.0f;
    m_config.max_time_filter = 1000.0f;
    m_config.time_slider_min_bound = 0.0f;
    m_config.time_slider_max_bound = 1000.0f;
}

// --------------------------------------------------------------------------
ImU32 TimelineViewer::serviceToColor(const std::string& p_service_name)
{
    // Check if we already have a color for this service
    if (!m_color_map.contains(p_service_name))
    {
        // Generate a new color using the utility function
        ImU32 color = utils::generateColorFromHash(p_service_name);
        m_color_map[p_service_name] = color;
    }
    return m_color_map[p_service_name];
}

// --------------------------------------------------------------------------
void TimelineViewer::updateSelectedSpanDetails(int p_span_index)
{
    // Early exit if the span index is out of bounds
    if (m_traces.empty() || (p_span_index < 0) ||
        (p_span_index >= int(m_traces[m_selected_trace].spans.size())))
    {
        return;
    }

    // Get the selected span details
    const Span& span = m_traces[m_selected_trace].spans[p_span_index];

    std::stringstream ss;
    ss << span;
    m_selected_span_details = ss.str();
}

// --------------------------------------------------------------------------
SpanLayout TimelineViewer::calculateSpanLayout(const Span& span,
                                               float y_offset,
                                               size_t visual_row,
                                               ImVec2 canvas_pos,
                                               float canvas_width) const
{
    SpanLayout layout;
    layout.y_pos = y_offset + float(visual_row) * m_config.span_spacing;
    layout.start_x = canvas_pos.x + utils::timeToPixel(span.start_time,
                                                       m_view_start,
                                                       m_view_end,
                                                       canvas_width);
    layout.end_x =
        canvas_pos.x + utils::timeToPixel(span.start_time + span.duration,
                                          m_view_start,
                                          m_view_end,
                                          canvas_width);
    layout.width =
        std::max(m_config.min_span_width, layout.end_x - layout.start_x);
    layout.indent = float(span.depth) * m_config.depth_indentation;
    return layout;
}

// --------------------------------------------------------------------------
void TimelineViewer::displaySpanBar(const Span& p_span,
                                    size_t p_span_index,
                                    float p_y_pos,
                                    ImVec2 p_canvas_pos,
                                    float p_canvas_width,
                                    ImDrawList* p_draw_list)
{
    // Visibility culling: Skip rendering spans that are completely outside the
    // visible time window
    if ((p_span.start_time + p_span.duration < m_view_start) ||
        (p_span.start_time > m_view_end))
    {
        return;
    }

    SpanLayout layout =
        calculateSpanLayout(p_span, p_y_pos, 0, p_canvas_pos, p_canvas_width);
    layout.start_x += layout.indent;

    // Color calculation: Use color based on service, but highlight if selected
    ImU32 span_color = serviceToColor(p_span.service_name);
    if ((m_selected_span_index != -1) &&
        (m_selected_span_index != int(p_span_index)))
    {
        // Darken non-selected spans
        span_color = utils::darkenColor(span_color, 0.9f);
    }

    // Draw the span bar
    ImVec2 rect_min(layout.start_x, layout.y_pos - m_config.span_height / 2.0f);
    ImVec2 rect_max(layout.start_x + layout.width,
                    layout.y_pos + m_config.span_height / 2.0f);
    p_draw_list->AddRectFilled(rect_min, rect_max, span_color);
    p_draw_list->AddRect(
        rect_min, rect_max, m_colors.span_border, 0.0f, 0, 1.0f);

    // Text rendering: Calculate text position and available width
    ImVec2 text_pos(layout.start_x + m_config.text_offset_x,
                    layout.y_pos - m_config.text_offset_y);

    // Calculate available width for text (accounting for padding)
    float available_width = layout.width - (2.0f * m_config.text_offset_x);
    displaySpanText(
        p_span.operation_name, text_pos, available_width, p_draw_list);

    // Click detection: Handle mouse clicks for span selection
    if (utils::isRectClicked(rect_min, rect_max))
    {
        // Update selection and details
        m_selected_span_index = (int)p_span_index;
        updateSelectedSpanDetails((int)p_span_index);
    }
    // Draw hover effect
    else if (ImGui::IsMouseHoveringRect(rect_min, rect_max))
    {
        p_draw_list->AddRect(
            rect_min, rect_max, m_colors.span_border, 0.0f, 0, 2.0f);
    }
}

// --------------------------------------------------------------------------
void TimelineViewer::displaySpanText(const std::string& p_text,
                                     ImVec2 p_text_pos,
                                     float p_available_width,
                                     ImDrawList* p_draw_list) const
{
    // Only render operation name if span is wide enough
    if (p_available_width <= m_config.min_text_width)
    {
        return;
    }

    // Get text size
    ImVec2 text_size = ImGui::CalcTextSize(p_text.c_str());

    // If text is too wide, truncate it
    if (text_size.x > p_available_width)
    {
        // Calculate how many characters we can fit
        std::string truncated_text = p_text;
        while (!truncated_text.empty() &&
               ImGui::CalcTextSize((truncated_text + "...").c_str()).x >
                   p_available_width)
        {
            truncated_text.pop_back();
        }
        truncated_text += "...";
        p_draw_list->AddText(
            p_text_pos, m_colors.span_text, truncated_text.c_str());
    }
    else
    {
        p_draw_list->AddText(p_text_pos, m_colors.span_text, p_text.c_str());
    }
}

// --------------------------------------------------------------------------
void TimelineViewer::displayMinimap(ImVec2 p_canvas_pos,
                                    float p_canvas_width,
                                    ImDrawList* p_draw_list,
                                    float p_canvas_height,
                                    const Trace& p_trace)
{
    // Early exit if minimap is disabled or no data
    if ((!m_show_minimap) || (m_traces.empty()))
    {
        return;
    }

    // Position minimap at bottom of canvas to avoid obscuring main timeline
    float minimap_y = p_canvas_pos.y + p_canvas_height -
                      m_config.minimap_height - m_config.minimap_bottom_margin;

    // Dark background with border for minimap area
    ImVec2 minimap_min(p_canvas_pos.x, minimap_y);
    ImVec2 minimap_max(p_canvas_pos.x + p_canvas_width,
                       minimap_y + m_config.minimap_height);
    p_draw_list->AddRectFilled(minimap_min, minimap_max, m_colors.minimap_bg);
    p_draw_list->AddRect(minimap_min, minimap_max, m_colors.minimap_grid);

    size_t visual_row = 0;
    for (const auto& span : p_trace.spans)
    {
        // Only render spans that pass current filter criteria
        if (!passesFilters(span))
        {
            continue;
        }

        // Convert span timing to minimap coordinates (full trace width)
        auto ratio = utils::calculateTimeRatio(
            span.start_time, 0, p_trace.total_duration);
        float start_x = p_canvas_pos.x + ratio * p_canvas_width;
        auto duration_ratio =
            utils::calculateTimeRatio(span.duration, 0, p_trace.total_duration);
        float width = std::max(m_config.minimap_min_width,
                               float(duration_ratio * p_canvas_width));

        // Use visual row for vertical positioning (same as main timeline)
        // Add indentation based on depth for hierarchical visualization
        float depth_indent =
            float(span.depth) * 2.0f; // Small indentation per depth level
        float span_y =
            minimap_y + m_config.minimap_top_margin +
            float(visual_row) * (m_config.minimap_span_height + 2.0f);

        // Skip if span would be drawn outside minimap bounds
        if (span_y + m_config.minimap_span_height >
            minimap_y + m_config.minimap_height)
        {
            continue;
        }

        ImU32 service_color = serviceToColor(span.service_name);
        ImU32 bright_color = utils::enhanceColorBrightness(
            service_color, m_config.minimap_brightness_factor);

        // Draw enhanced span with subtle border for definition
        ImVec2 span_min(start_x + depth_indent, span_y);
        ImVec2 span_max(start_x + width + depth_indent,
                        span_y + m_config.minimap_span_height);
        p_draw_list->AddRectFilled(span_min, span_max, m_colors.minimap_span);

        visual_row++;
    }

    // ===== CURRENT VIEW INDICATOR =====
    // Yellow rectangle showing what portion of the trace is currently visible
    auto ratio =
        utils::calculateTimeRatio(m_view_start, 0, p_trace.total_duration);
    float view_start_x = p_canvas_pos.x + ratio * p_canvas_width;
    ratio = utils::calculateTimeRatio(
        m_view_end - m_view_start, 0, p_trace.total_duration);
    float view_width = ratio * p_canvas_width;

    ImVec2 view_min(view_start_x, minimap_y);
    ImVec2 view_max(view_start_x + view_width,
                    minimap_y + m_config.minimap_height);
    p_draw_list->AddRectFilled(view_min, view_max, m_colors.minimap_window);
    p_draw_list->AddRect(view_min, view_max, m_colors.minimap_border);
}

// --------------------------------------------------------------------------
bool TimelineViewer::passesFilters(const Span& p_span) const
{
    // Duration filter: Check if span duration falls within the specified range
    if (p_span.duration < m_config.min_duration_filter ||
        p_span.duration > m_config.max_duration_filter)
    {
        return false;
    }

    // Time filter: Check if span start time falls within the specified range
    if (p_span.start_time < m_config.min_time_filter ||
        p_span.start_time > m_config.max_time_filter)
    {
        return false;
    }

    // Service filter: Apply case-insensitive partial match for service name
    if (!m_config.service_filter.empty())
    {
        if (!utils::containsIgnoreCase(p_span.service_name,
                                       m_config.service_filter))
        {
            return false;
        }
    }

    // Operation filter: Apply case-insensitive partial match for operation name
    if (!m_config.operation_filter.empty())
    {
        if (!utils::containsIgnoreCase(p_span.operation_name,
                                       m_config.operation_filter))
            return false;
    }

    // Span passes all filters
    return true;
}

// --------------------------------------------------------------------------
std::string TimelineViewer::loadFromJSON(const std::string& p_json_data)
{
    nlohmann::json root;

    try
    {
        root = nlohmann::json::parse(p_json_data);
        m_traces.clear();

        for (const auto& trace_data : root["traces"])
        {
            Trace trace;
            trace.trace_id = trace_data["traceID"];
            trace.trace_name = trace_data["traceName"];

            // Check if spans array exists and handle missing spans gracefully
            if (trace_data.contains("spans") && trace_data["spans"].is_array())
            {
                trace.total_spans = trace_data["spans"].size();
            }
            else
            {
                trace.total_spans = 0;
            }

            // Initialize min/max values
            double min_time = std::numeric_limits<double>::max();
            double max_time = 0;
            trace.min_duration = std::numeric_limits<double>::max();
            trace.max_duration = 0.0;
            trace.min_start_time = std::numeric_limits<double>::max();
            trace.max_start_time = std::numeric_limits<double>::lowest();

            // Only process spans if they exist
            if (trace_data.contains("spans") && trace_data["spans"].is_array())
            {
                for (const auto& span_data : trace_data["spans"])
                {
                    Span span;
                    span.span_id = span_data["spanID"];
                    span.operation_name = span_data["operationName"];
                    span.service_name = span_data["serviceName"];
                    span.color = serviceToColor(span.service_name);
                    // Keep timestamps in nanoseconds (original OpenTelemetry
                    // format) Time unit conversion will be handled by display
                    // functions
                    span.start_time = span_data["startTime"].get<double>();
                    span.duration = span_data["duration"].get<double>();
                    if (span_data.contains("depth"))
                    {
                        span.depth = span_data["depth"];
                    }
                    else
                    {
                        span.depth = 0;
                    }

                    // Tags (test.json format)
                    if (span_data.contains("tags"))
                    {
                        for (const auto& tag : span_data["tags"])
                        {
                            span.tags.emplace_back(tag["key"], tag["value"]);
                        }
                    }

                    // Attributes (nested-traces.json format)
                    if (span_data.contains("attributes"))
                    {
                        for (const auto& [key, value] :
                             span_data["attributes"].items())
                        {
                            span.tags.emplace_back(key,
                                                   value.get<std::string>());
                        }
                    }

                    // Events (nested-traces.json format)
                    if (span_data.contains("events"))
                    {
                        for (const auto& event : span_data["events"])
                        {
                            std::string event_info =
                                "Event: " + event["name"].get<std::string>();
                            if (event.contains("timestamp"))
                            {
                                event_info +=
                                    " (timestamp: " +
                                    std::to_string(
                                        event["timestamp"].get<long>()) +
                                    ")";
                            }
                            span.logs.push_back(event_info);
                        }
                    }

                    // Calculate trace time bounds
                    min_time = std::min(min_time, span.start_time);
                    max_time =
                        std::max(max_time, span.start_time + span.duration);

                    // Cache min/max values for optimization
                    trace.min_duration =
                        std::min(trace.min_duration, span.duration);
                    trace.max_duration =
                        std::max(trace.max_duration, span.duration);
                    trace.min_start_time =
                        std::min(trace.min_start_time, span.start_time);
                    trace.max_start_time =
                        std::max(trace.max_start_time, span.start_time);

                    trace.spans.push_back(span);
                }
            } // Close the spans existence check

            trace.start_time = min_time;
            trace.total_duration = max_time - min_time;

            // Normalize timestamps: subtract min_time from all span start_times
            // to make them relative to the trace start
            for (auto& span : trace.spans)
            {
                span.start_time -= min_time;
            }

            // Update cached min/max values after normalization
            if (!trace.spans.empty())
            {
                trace.min_start_time =
                    0.0; // After normalization, minimum is always 0
                trace.max_start_time =
                    trace.total_duration - trace.spans.back().duration;

                // Recalculate these with normalized values
                for (const auto& span : trace.spans)
                {
                    trace.min_start_time =
                        std::min(trace.min_start_time, span.start_time);
                    trace.max_start_time =
                        std::max(trace.max_start_time, span.start_time);
                }
            }

            // Handle empty traces
            if (trace.spans.empty())
            {
                trace.min_duration = 0.0;
                trace.max_duration = 0.0;
                trace.min_start_time = 0.0;
                trace.max_start_time = 0.0;
            }

            // Sort by start time (ascending)
            std::sort(trace.spans.begin(),
                      trace.spans.end(),
                      [](const Span& a, const Span& b)
                      { return a.start_time < b.start_time; });

            m_traces.push_back(trace);
        }
    }
    catch (const nlohmann::json::parse_error& e)
    {
        std::ostringstream error_msg;
        error_msg << "Error: " << e.what();
        return error_msg.str();
    }
    catch (const std::exception& e)
    {
        std::ostringstream error_msg;
        error_msg << "Error: " << e.what();
        return error_msg.str();
    }

    return {};
}

// --------------------------------------------------------------------------
void TimelineViewer::initializeSliderBounds(double min_duration,
                                            double max_duration,
                                            double min_time,
                                            double max_time)
{
    // Set duration slider bounds to the actual data range
    m_config.slider_min_bound = float(min_duration);
    m_config.slider_max_bound =
        float(max_duration * m_config.duration_buffer_percentage);

    // Initialize duration filter values to cover the full range (sliders at
    // extremities)
    m_config.min_duration_filter = m_config.slider_min_bound;
    m_config.max_duration_filter = m_config.slider_max_bound;

    // Set time slider bounds to the actual data range
    m_config.time_slider_min_bound = float(min_time);
    m_config.time_slider_max_bound = float(max_time);

    // Initialize time filter values to cover the full range (sliders at
    // extremities)
    m_config.min_time_filter = m_config.time_slider_min_bound;
    m_config.max_time_filter = m_config.time_slider_max_bound;
}

// --------------------------------------------------------------------------
void TimelineViewer::cacheTraceInformation()
{
    // Track min and max durations across all traces for filter
    // initialization
    double global_min_duration = std::numeric_limits<double>::max();
    double global_max_duration = 0.0;

    // Track min and max start times across all traces for time filter
    // initialization
    double global_min_time = std::numeric_limits<double>::max();
    double global_max_time = std::numeric_limits<double>::lowest();

    // Track if we actually found any spans
    bool has_spans = false;

    // Use pre-calculated values from traces instead of recalculating
    for (const auto& trace : m_traces)
    {
        if (!trace.spans.empty())
        {
            // Use cached min/max values from trace
            global_min_duration =
                std::min(global_min_duration, trace.min_duration);
            global_max_duration =
                std::max(global_max_duration, trace.max_duration);
            global_min_time = std::min(global_min_time, trace.min_start_time);
            global_max_time = std::max(global_max_time, trace.max_start_time);
            has_spans = true;
        }
    }

    // Initialize duration and time filters based on the data from loaded
    // traces
    if (has_spans)
    {
        initializeSliderBounds(global_min_duration,
                               global_max_duration,
                               global_min_time,
                               global_max_time);
    }
    else
    {
        // Fallback values if no spans were found
        initializeSliderBounds(0.0, 1000.0, 0.0, 1000.0);
    }

    if (!m_traces.empty())
    {
        m_view_start = 0;
        m_view_end = m_traces[m_selected_trace].total_duration;

        // Auto-detect best time unit if enabled
        if (m_auto_detect_time_unit && has_spans)
        {
            m_current_time_unit =
                detectBestTimeUnit(global_min_time, global_max_time);
        }
    }
}

// --------------------------------------------------------------------------
std::string TimelineViewer::loadFromFile(const std::string& p_file_path)
{
    std::ifstream file(p_file_path);
    if (!file.is_open())
    {
        return "Error: Cannot open file " + p_file_path;
    }

    std::string json_content;
    std::string line;
    while (std::getline(file, line))
    {
        json_content += line + "\n";
    }
    file.close();

    if (json_content.empty())
    {
        return "Error: File " + p_file_path + " is empty";
    }

    std::string error_message = loadFromJSON(json_content);
    if (!error_message.empty())
    {
        return error_message;
    }
    cacheTraceInformation();
    return {};
}

// --------------------------------------------------------------------------
void TimelineViewer::render()
{
    setupWorkspace();
    showTimelinePanel();
    showControlPanel();
    showMinimapPanel();
    showSpanDetailsPanel();
}

// --------------------------------------------------------------------------
void TimelineViewer::setupWorkspace()
{
    // Create main dockspace
    static bool dockspace_open = true;
    static bool opt_fullscreen = true;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar |
                        ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
                        ImGuiWindowFlags_NoNavFocus;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Timeline Viewer DockSpace", &dockspace_open, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
    {
        ImGui::PopStyleVar(2);
    }

    // Submit the DockSpace
    if (ImGuiIO const& io = ImGui::GetIO();
        io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dock_id_timeline = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dock_id_timeline, ImVec2(0.0f, 0.0f), dockspace_flags);

        // Setup default docking layout only if imgui.ini is missing or
        // invalid
        static bool first_time = true;
        if (first_time)
        {
            first_time = false;

            // Check if the dockspace has no configuration (imgui.ini
            // missing or invalid)
            if (ImGui::DockBuilderGetNode(dock_id_timeline) == nullptr)
            {
                std::cout << "Setting up default docking layout (imgui.ini not "
                             "found or invalid)"
                          << std::endl;

                ImGui::DockBuilderRemoveNode(dock_id_timeline);
                ImGui::DockBuilderAddNode(dock_id_timeline,
                                          dockspace_flags |
                                              ImGuiDockNodeFlags_DockSpace);
                ImGui::DockBuilderSetNodeSize(
                    dock_id_timeline, ImGui::GetMainViewport()->WorkSize);

                // Split the dockspace into left panel and main area
                auto dock_id_control =
                    ImGui::DockBuilderSplitNode(dock_id_timeline,
                                                ImGuiDir_Left,
                                                0.25f,
                                                nullptr,
                                                &dock_id_timeline);
                auto dock_id_file_loader =
                    ImGui::DockBuilderSplitNode(dock_id_timeline,
                                                ImGuiDir_Up,
                                                0.15f,
                                                nullptr,
                                                &dock_id_timeline);
                auto dock_id_span_details =
                    ImGui::DockBuilderSplitNode(dock_id_timeline,
                                                ImGuiDir_Down,
                                                0.3f,
                                                nullptr,
                                                &dock_id_timeline);

                // Split bottom area to have minimap and span details side
                // by side
                auto dock_id_minimap =
                    ImGui::DockBuilderSplitNode(dock_id_span_details,
                                                ImGuiDir_Left,
                                                0.6f,
                                                nullptr,
                                                &dock_id_span_details);

                // Dock windows to specific areas
                ImGui::DockBuilderDockWindow("File Loader",
                                             dock_id_file_loader);
                ImGui::DockBuilderDockWindow("Trace Search & Filter",
                                             dock_id_control);
                ImGui::DockBuilderDockWindow("Trace Timeline",
                                             dock_id_timeline);
                ImGui::DockBuilderDockWindow("Trace Minimap", dock_id_minimap);
                ImGui::DockBuilderDockWindow("Span Details",
                                             dock_id_span_details);

                ImGui::DockBuilderFinish(dock_id_timeline);
            }
            else
            {
                std::cout << "Using existing docking layout from imgui.ini"
                          << std::endl;
            }
        }
    }

    showMenuBar();

    ImGui::End();
}

// --------------------------------------------------------------------------
void TimelineViewer::showFileLoader()
{
    static char file_path_buffer[512] = "";
    static size_t buffer_size = sizeof(file_path_buffer);
    static bool show_file_dialog = false;
    static std::string current_directory = ".";
    static std::vector<std::string> directory_files;
    static bool directory_loaded = false;

    ImGui::InputText("File path", file_path_buffer, buffer_size);

    if (ImGui::Button("Browse"))
    {
        show_file_dialog = true;
        directory_loaded = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Load"))
    {
        std::string file_path(file_path_buffer);
        if (!file_path.empty())
        {
            std::string error_message = loadFromFile(file_path);
            if (error_message.empty())
            {
                // Reset view when new file is loaded
                if (!m_traces.empty())
                {
                    m_selected_trace = 0;
                    m_view_start = 0;
                    m_view_end = m_traces[0].total_duration;
                }
            }
            else
            {
                std::ostringstream error_msg;
                error_msg << "Error: " << error_message;
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                                   "%s",
                                   error_msg.str().c_str());
            }
        }
    }

    // Simple file dialog
    if (show_file_dialog)
    {
        ImGui::OpenPopup("Select File");
    }

    if (ImGui::BeginPopupModal("Select File",
                               &show_file_dialog,
                               ImGuiWindowFlags_AlwaysAutoResize))
    {
        // Load directory contents once
        if (!directory_loaded)
        {
            directory_files.clear();
            try
            {
                for (const auto& entry :
                     std::filesystem::directory_iterator(current_directory))
                {
                    if (entry.is_regular_file())
                    {
                        std::string filename = entry.path().filename().string();
                        if (filename.ends_with(".json") ||
                            filename.ends_with(".JSON"))
                        {
                            directory_files.push_back(filename);
                        }
                    }
                }
                std::sort(directory_files.begin(), directory_files.end());
            }
            catch (const std::exception& e)
            {
                std::ostringstream error_msg;
                error_msg << "Error: " << e.what();
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
                                   "%s",
                                   error_msg.str().c_str());
            }
            directory_loaded = true;
        }

        std::ostringstream directory_msg;
        directory_msg << "Directory: " << current_directory;
        ImGui::Text("%s", directory_msg.str().c_str());
        ImGui::Separator();

        // File list
        if (ImGui::BeginListBox("##Files", ImVec2(400, 200)))
        {
            for (const auto& file : directory_files)
            {
                if (ImGui::Selectable(file.c_str()))
                {
                    std::string full_path = current_directory + "/" + file;
                    strncpy(
                        file_path_buffer, full_path.c_str(), buffer_size - 1);
                    file_path_buffer[buffer_size - 1] = '\0';
                    show_file_dialog = false;
                }
            }
            ImGui::EndListBox();
        }

        if (ImGui::Button("Cancel"))
        {
            show_file_dialog = false;
        }

        ImGui::EndPopup();
    }
}

// --------------------------------------------------------------------------
void TimelineViewer::showMenuBar()
{
    if (!ImGui::BeginMenuBar())
        return;

    if (ImGui::BeginMenu("Load Trace File"))
    {
        showFileLoader();
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View"))
    {
        ImGui::MenuItem("Show Minimap", nullptr, &m_show_minimap);
        ImGui::Separator();
        if (ImGui::MenuItem("Clear All"))
        {
            clearAll();
        }
        if (ImGui::MenuItem("Reset Zoom") && !m_traces.empty())
        {
            const Trace& trace = m_traces[m_selected_trace];
            resetZoom(trace);
        }
        ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
}

// --------------------------------------------------------------------------
void TimelineViewer::showControlPanel()
{
    ImGui::Begin("Trace Search & Filter", nullptr, ImGuiWindowFlags_None);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, m_colors.panel_bg);
    ImGui::PushStyleColor(ImGuiCol_Text, m_colors.panel_text);
    ImGui::PushStyleColor(ImGuiCol_Header, m_colors.panel_header);

    // Trace selection (combo box)
    if (ImGui::BeginCombo("Select Trace",
                          m_traces.empty()
                              ? "No traces"
                              : m_traces[m_selected_trace].trace_name.c_str()))
    {
        for (size_t i = 0; i < m_traces.size(); i++)
        {
            std::string text = std::format("{} ({} spans)",
                                           m_traces[i].trace_name,
                                           m_traces[i].total_spans);
            bool is_selected = (m_selected_trace == int(i));
            if (ImGui::Selectable(text.c_str(), is_selected))
            {
                m_selected_trace = int(i);
                m_view_start = 0;
                m_view_end = m_traces[i].total_duration;

                if (!m_traces[i].spans.empty())
                {
                    // Update slider bounds to match the selected trace's
                    // duration range using cached values
                    initializeSliderBounds(m_traces[i].min_duration,
                                           m_traces[i].max_duration,
                                           m_traces[i].min_start_time,
                                           m_traces[i].max_start_time);
                }
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Separator();

    // Time Unit Selection
    ImGui::Text("Time Units");

    // Store previous state to detect changes
    static bool prev_auto_detect = m_auto_detect_time_unit;
    static TimeUnit prev_time_unit = m_current_time_unit;

    ImGui::Checkbox("Auto-detect", &m_auto_detect_time_unit);

    // If auto-detect was just enabled, recalculate best unit
    if (m_auto_detect_time_unit && !prev_auto_detect && !m_traces.empty())
    {
        // Find global time range across all traces
        double global_min_time = std::numeric_limits<double>::max();
        double global_max_time = std::numeric_limits<double>::lowest();

        for (const auto& trace : m_traces)
        {
            if (!trace.spans.empty())
            {
                global_min_time =
                    std::min(global_min_time, trace.min_start_time);
                global_max_time = std::max(global_max_time,
                                           trace.max_start_time +
                                               trace.spans.back().duration);
            }
        }

        if (global_min_time < global_max_time)
        {
            m_current_time_unit =
                detectBestTimeUnit(global_min_time, global_max_time);
        }
    }

    if (!m_auto_detect_time_unit)
    {
        std::vector<TimeUnitInfo> units = getAvailableTimeUnits();
        TimeUnitInfo current_unit_info = getTimeUnitInfo(m_current_time_unit);

        if (ImGui::BeginCombo("Time Unit", current_unit_info.name.c_str()))
        {
            for (const auto& unit : units)
            {
                bool is_selected = (unit.unit == m_current_time_unit);
                if (ImGui::Selectable(unit.name.c_str(), is_selected))
                {
                    m_current_time_unit = unit.unit;
                }
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }
    else
    {
        TimeUnitInfo current_unit_info = getTimeUnitInfo(m_current_time_unit);
        ImGui::Text("Current: %s", current_unit_info.name.c_str());
    }

    // Update previous state for next frame
    prev_auto_detect = m_auto_detect_time_unit;
    prev_time_unit = m_current_time_unit;

    ImGui::Separator();
    ImGui::Text("Filters");
    ImGui::InputText("Service", &m_config.service_filter);
    ImGui::InputText("Operation", &m_config.operation_filter);

    // Show the actual data range for reference
    if (!m_traces.empty())
    {
        std::string duration_range_text =
            "Duration Range: " +
            formatTimeWithUnit(m_config.slider_min_bound, m_current_time_unit) +
            " - " +
            formatTimeWithUnit(m_config.slider_max_bound, m_current_time_unit);
        ImGui::Text("%s", duration_range_text.c_str());
    }

    ImGui::SliderFloat("Min Duration (ms)",
                       &m_config.min_duration_filter,
                       m_config.slider_min_bound,
                       m_config.slider_max_bound);
    ImGui::SliderFloat("Max Duration (ms)",
                       &m_config.max_duration_filter,
                       m_config.slider_min_bound,
                       m_config.slider_max_bound);

    // Ensure min doesn't exceed max and vice versa for logical consistency
    if (m_config.min_duration_filter > m_config.max_duration_filter)
    {
        m_config.max_duration_filter = m_config.min_duration_filter;
    }

    // Show the actual time range for reference
    ImGui::Separator();
    if (!m_traces.empty())
    {
        std::string time_range_text =
            "Time Range: " +
            formatTimeWithUnit(m_config.time_slider_min_bound,
                               m_current_time_unit) +
            " - " +
            formatTimeWithUnit(m_config.time_slider_max_bound,
                               m_current_time_unit);
        ImGui::Text("%s", time_range_text.c_str());
    }

    ImGui::SliderFloat("Min Start Time (ms)",
                       &m_config.min_time_filter,
                       m_config.time_slider_min_bound,
                       m_config.time_slider_max_bound);
    ImGui::SliderFloat("Max Start Time (ms)",
                       &m_config.max_time_filter,
                       m_config.time_slider_min_bound,
                       m_config.time_slider_max_bound);

    // Ensure min doesn't exceed max and vice versa for logical consistency
    if (m_config.min_time_filter > m_config.max_time_filter)
    {
        m_config.max_time_filter = m_config.min_time_filter;
    }

    // Show trace statistics
    ImGui::Separator();
    if (!m_traces.empty())
    {
        const Trace& trace = m_traces[m_selected_trace];
        ImGui::Separator();
        ImGui::Text("Trace Statistics");
        std::string total_duration_text =
            "Total Duration: " +
            formatTimeWithUnit(trace.total_duration, m_current_time_unit);
        ImGui::Text("%s", total_duration_text.c_str());
        std::string total_spans =
            std::format("Total Spans: {}", trace.total_spans);
        ImGui::Text("%s", total_spans.c_str());
        std::string services =
            std::format("Services: {}", (int)trace.service_colors.size());
        ImGui::Text("%s", services.c_str());
    }

    ImGui::PopStyleColor(3);
    ImGui::End();
}

// --------------------------------------------------------------------------
void TimelineViewer::showMinimapPanel()
{
    if (!m_show_minimap)
        return;

    ImGui::Begin("Trace Minimap", &m_show_minimap, ImGuiWindowFlags_None);

    if (m_traces.empty())
    {
        ImGui::Text("No trace data for minimap");
        ImGui::End();
        return;
    }

    const Trace& current_trace = m_traces[m_selected_trace];
    ImVec2 minimap_canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 minimap_canvas_size = ImGui::GetContentRegionAvail();

    if (minimap_canvas_size.x <= m_config.minimap_dock_min_width ||
        minimap_canvas_size.y <= m_config.minimap_dock_min_height)
    {
        ImGui::Text("Minimap requires more space");
        ImGui::End();
        return;
    }

    ImDrawList* minimap_draw_list = ImGui::GetWindowDrawList();

    minimap_draw_list->AddRectFilled(
        minimap_canvas_pos,
        ImVec2(minimap_canvas_pos.x + minimap_canvas_size.x,
               minimap_canvas_pos.y + minimap_canvas_size.y),
        IM_COL32(30, 30, 30, 255));
    minimap_draw_list->AddRect(
        minimap_canvas_pos,
        ImVec2(minimap_canvas_pos.x + minimap_canvas_size.x,
               minimap_canvas_pos.y + minimap_canvas_size.y),
        IM_COL32(100, 100, 100, 255));

    size_t visual_row = 0;
    for (const auto& span : current_trace.spans)
    {
        if (!passesFilters(span))
            continue;

        auto ratio = utils::calculateTimeRatio(
            span.start_time, 0, current_trace.total_duration);
        float start_x = minimap_canvas_pos.x + ratio * minimap_canvas_size.x;
        auto duration_ratio = utils::calculateTimeRatio(
            span.duration, 0, current_trace.total_duration);
        float width = std::max(m_config.minimap_min_width,
                               float(duration_ratio * minimap_canvas_size.x));

        // Use visual row for vertical positioning (same as main timeline)
        // Add indentation based on depth for hierarchical visualization
        float depth_indent =
            float(span.depth) * 2.0f; // Small indentation per depth level
        float span_y =
            minimap_canvas_pos.y + m_config.minimap_top_margin +
            float(visual_row) * (m_config.minimap_span_height + 2.0f);

        // Skip if span would be drawn outside minimap bounds
        if (span_y + m_config.minimap_span_height >
            minimap_canvas_pos.y + minimap_canvas_size.y)
            continue;

        ImVec2 span_min(start_x + depth_indent, span_y);
        ImVec2 span_max(start_x + width + depth_indent,
                        span_y + m_config.minimap_span_height);

        // Use consistent color enhancement
        ImU32 service_color = serviceToColor(span.service_name);
        ImU32 bright_color = utils::enhanceColorBrightness(
            service_color, m_config.minimap_brightness_factor);

        minimap_draw_list->AddRectFilled(span_min, span_max, bright_color);
        minimap_draw_list->AddRect(
            span_min, span_max, IM_COL32(255, 255, 255, 80), 0.0f, 0, 1.0f);

        visual_row++;
    }

    auto ratio = utils::calculateTimeRatio(
        m_view_start, 0, current_trace.total_duration);
    float view_start_x = minimap_canvas_pos.x + ratio * minimap_canvas_size.x;
    ratio = utils::calculateTimeRatio(
        m_view_end - m_view_start, 0, current_trace.total_duration);
    float view_width = ratio * minimap_canvas_size.x;

    ImVec2 view_min(view_start_x, minimap_canvas_pos.y);
    ImVec2 view_max(view_start_x + view_width,
                    minimap_canvas_pos.y + minimap_canvas_size.y);
    minimap_draw_list->AddRectFilled(
        view_min, view_max, m_colors.minimap_window);
    minimap_draw_list->AddRect(view_min, view_max, m_colors.minimap_border);

    ImGui::InvisibleButton("minimap_canvas", minimap_canvas_size);

    ImGui::End();
}

// --------------------------------------------------------------------------
void TimelineViewer::displayTimelineGrid(ImDrawList* p_draw_list,
                                         ImVec2 p_canvas_pos,
                                         ImVec2 p_canvas_size) const
{
    float timeline_y_start = p_canvas_pos.y + m_config.timeline_top_margin;
    float timeline_y_end =
        p_canvas_pos.y + p_canvas_size.y - m_config.timeline_bottom_margin;

    for (int i = 0; i <= m_config.timeline_ticks; ++i)
    {
        double time = m_view_start +
                      i * (m_view_end - m_view_start) / m_config.timeline_ticks;
        float x = p_canvas_pos.x +
                  float(i) * p_canvas_size.x / float(m_config.timeline_ticks);

        p_draw_list->AddLine(ImVec2(x, timeline_y_start),
                             ImVec2(x, timeline_y_end),
                             m_colors.grid_lines);

        std::string time_str = formatTimeWithUnit(time, m_current_time_unit);
        p_draw_list->AddText(
            ImVec2(x + m_config.grid_text_offset,
                   timeline_y_start + m_config.grid_text_y_offset),
            m_colors.timeline_text,
            time_str.c_str());
    }
}

// --------------------------------------------------------------------------
void TimelineViewer::showTimelinePanel()
{
    ImGui::Begin("Trace Timeline", nullptr, ImGuiWindowFlags_NoCollapse);

    if (m_traces.empty())
    {
        // Show file loader content when no traces are loaded
        ImGui::Text("Load Trace File");
        showFileLoader();
        ImGui::End();
        return;
    }

    const Trace& trace = m_traces[m_selected_trace];

    std::string trace_text = std::format("Trace: {}", trace.trace_name);
    ImGui::Text("%s", trace_text.c_str());
    ImGui::SameLine();
    std::string duration_text =
        "| Duration: " +
        formatTimeWithUnit(trace.total_duration, m_current_time_unit);
    ImGui::Text("%s", duration_text.c_str());
    ImGui::SameLine();
    std::string spans_text = std::format("| {} spans", trace.total_spans);
    ImGui::Text("%s", spans_text.c_str());

    ImGui::Separator();

    std::string view_text =
        "View: " + formatTimeWithUnit(m_view_start, m_current_time_unit) +
        " - " + formatTimeWithUnit(m_view_end, m_current_time_unit);
    ImGui::Text("%s", view_text.c_str());

    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    float min_height =
        m_config.min_canvas_height +
        m_config.span_spacing *
            std::min(m_config.max_spans_for_height, float(trace.spans.size())) +
        m_config.canvas_height_buffer;
    canvas_size.y = std::max(canvas_size.y, min_height);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    draw_list->AddRectFilled(
        canvas_pos,
        ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y),
        IM_COL32(25, 25, 25, 255));

    ImGui::InvisibleButton("timeline_canvas", canvas_size);

    handleMouseInteractions(canvas_pos, canvas_size, trace);
    handleKeyboardNavigation(trace);
    displayTimelineGrid(draw_list, canvas_pos, canvas_size);

    float y_offset = canvas_pos.y + m_config.timeline_content_offset;
    float max_y =
        canvas_pos.y + canvas_size.y - m_config.timeline_content_margin;
    size_t visual_row = 0;

    for (size_t i = 0; i < trace.spans.size(); ++i)
    {
        const Span& span = trace.spans[i];

        if (!passesFilters(span))
            continue;

        float y_pos = y_offset + float(visual_row) * m_config.span_spacing;

        if (y_pos + m_config.span_height / 2 > max_y)
            break;

        displaySpanBar(span, i, y_pos, canvas_pos, canvas_size.x, draw_list);

        std::string span_label =
            std::format("{}::{}", span.service_name, span.operation_name);
        draw_list->AddText(ImVec2(canvas_pos.x + m_config.span_label_margin,
                                  y_pos - m_config.span_label_y_offset),
                           IM_COL32(200, 200, 200, 255),
                           span_label.c_str());

        visual_row++;
    }

    ImGui::End();
}

// --------------------------------------------------------------------------
void TimelineViewer::showSpanDetailsPanel() const
{
    ImGui::Begin("Span Details", nullptr, ImGuiWindowFlags_None);
    if (!m_selected_span_details.empty())
    {
        ImGui::TextWrapped("%s", m_selected_span_details.c_str());
    }
    else
    {
        ImGui::Text("Click on a span to see its details");
    }
    ImGui::End();
}

// --------------------------------------------------------------------------
void TimelineViewer::updateViewToCenterOnSpan(const Span& span)
{
    const Trace& trace = m_traces[m_selected_trace];
    double view_range = m_view_end - m_view_start;
    double span_center = span.start_time + span.duration / 2.0;
    m_view_start = span_center - view_range / 2.0;
    m_view_end = span_center + view_range / 2.0;
    m_view_start = std::max(0.0, m_view_start);
    m_view_end = std::min(trace.total_duration, m_view_end);
}

// --------------------------------------------------------------------------
void TimelineViewer::selectPreviousSpan(const Trace& trace)
{
    if ((m_selected_span_index > 0) && (!trace.spans.empty()))
    {
        m_selected_span_index--;
        const Span& span = trace.spans[m_selected_span_index];

        updateSelectedSpanDetails(m_selected_span_index);
        updateViewToCenterOnSpan(span);
    }
}

// --------------------------------------------------------------------------
void TimelineViewer::selectNextSpan(const Trace& trace)
{
    if ((!trace.spans.empty()) &&
        (m_selected_span_index < int(trace.spans.size()) - 1))
    {
        m_selected_span_index++;
        const Span& span = trace.spans[m_selected_span_index];

        updateSelectedSpanDetails(m_selected_span_index);
        updateViewToCenterOnSpan(span);
    }
}

// --------------------------------------------------------------------------
void TimelineViewer::selectUpperSpan(const Trace& trace)
{
    if (!trace.spans.empty())
    {
        if (m_selected_span_index > 0)
        {
            m_selected_span_index--;
        }
        else if ((m_selected_span_index == -1) && (!trace.spans.empty()))
        {
            m_selected_span_index = int(trace.spans.size()) - 1;
        }

        if (m_selected_span_index >= 0)
        {
            updateSelectedSpanDetails(m_selected_span_index);
        }
    }
}

// --------------------------------------------------------------------------
void TimelineViewer::selectLowerSpan(const Trace& trace)
{
    if (!trace.spans.empty())
    {
        if (m_selected_span_index < int(trace.spans.size()) - 1)
        {
            m_selected_span_index++;
        }
        else if ((m_selected_span_index == -1) && (!trace.spans.empty()))
        {
            m_selected_span_index = 0;
        }

        if (m_selected_span_index >= 0)
        {
            updateSelectedSpanDetails(m_selected_span_index);
        }
    }
}

// --------------------------------------------------------------------------
void TimelineViewer::scrollLeft()
{
    double view_range = m_view_end - m_view_start;
    double scroll_amount = view_range * m_config.scroll_percentage;
    m_view_start = std::max(0.0, m_view_start - scroll_amount);
    m_view_end = std::max(view_range, m_view_end - scroll_amount);
}

// --------------------------------------------------------------------------
void TimelineViewer::scrollRight()
{
    const Trace& trace = m_traces[m_selected_trace];
    double view_range = m_view_end - m_view_start;
    double scroll_amount = view_range * m_config.scroll_percentage;
    m_view_start = std::min(trace.total_duration - view_range,
                            m_view_start + scroll_amount);
    m_view_end = std::min(trace.total_duration, m_view_end + scroll_amount);
}

// --------------------------------------------------------------------------
void TimelineViewer::handleKeyboardNavigation(const Trace& trace)
{
    if (!ImGui::IsWindowFocused())
        return;

    ImGuiIO const& io = ImGui::GetIO();

    // If Escape is pressed, deselect the current span
    if (ImGui::IsKeyPressed(ImGuiKey_Escape))
    {
        m_selected_span_index = -1;
        m_selected_span_details.clear();
        return;
    }

    // If ctrl + left arrow are pressed: select the previous span.
    // Else scroll left.
    if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
    {
        io.KeyCtrl ? selectPreviousSpan(trace) : scrollLeft();
    }

    // If ctrl + right arrow are pressed: select the next span.
    // Else scroll right.
    if (ImGui::IsKeyPressed(ImGuiKey_RightArrow))
    {
        io.KeyCtrl ? selectNextSpan(trace) : scrollRight();
    }

    // If up arrow is pressed: select the upper span.
    if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
    {
        selectUpperSpan(trace);
    }

    // If down arrow is pressed: select the lower span.
    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
    {
        selectLowerSpan(trace);
    }
}

// --------------------------------------------------------------------------
void TimelineViewer::rectangularZoom(const Trace& p_trace,
                                     ImVec2 p_canvas_pos,
                                     ImVec2 p_canvas_size)
{
    // If the mouse is clicked, start the selection rectangle
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
    {
        m_zoom_selection.is_selecting = true;
        m_zoom_selection.start = ImGui::GetMousePos();
    }

    if (m_zoom_selection.is_selecting)
    {
        // Draw the selection rectangle
        m_zoom_selection.end = ImGui::GetMousePos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddRect(m_zoom_selection.start,
                           m_zoom_selection.end,
                           IM_COL32(255, 255, 0, 128),
                           0.0f,
                           0,
                           2.0f);

        // If the mouse is released, zoom to the selection rectangle
        if (ImGui::IsMouseReleased(0))
        {
            if (std::abs(m_zoom_selection.end.x - m_zoom_selection.start.x) >
                m_config.min_selection_width)
            {
                double min_x =
                    std::min(m_zoom_selection.start.x, m_zoom_selection.end.x);
                double max_x =
                    std::max(m_zoom_selection.start.x, m_zoom_selection.end.x);
                double start_time = utils::pixelToTime(float(min_x),
                                                       p_canvas_pos.x,
                                                       p_canvas_size.x,
                                                       m_view_start,
                                                       m_view_end);
                double end_time = utils::pixelToTime(float(max_x),
                                                     p_canvas_pos.x,
                                                     p_canvas_size.x,
                                                     m_view_start,
                                                     m_view_end);

                m_view_start = std::max(0.0, start_time);
                m_view_end = std::min(p_trace.total_duration, end_time);
            }
            m_zoom_selection.is_selecting = false;
        }
    }
}

// --------------------------------------------------------------------------
void TimelineViewer::scrollZoom(const Trace& p_trace,
                                ImVec2 p_canvas_pos,
                                ImVec2 p_canvas_size)
{
    ImVec2 mouse_pos = ImGui::GetMousePos();

    double view_range = m_view_end - m_view_start;
    double ratio = utils::calculateTimeRatio(
        mouse_pos.x - p_canvas_pos.x, 0, p_canvas_size.x);
    double mouse_time = utils::interpolate(m_view_start, m_view_end, ratio);
    double zoom_factor = ImGui::GetIO().MouseWheel > 0
                             ? m_config.zoom_factor_in
                             : m_config.zoom_factor_out;

    m_view_start = mouse_time - (mouse_time - m_view_start) * zoom_factor;
    m_view_start = std::max(0.0, m_view_start);

    m_view_end = m_view_start + view_range * zoom_factor;
    m_view_end = std::min(p_trace.total_duration, m_view_end);
}

// --------------------------------------------------------------------------
void TimelineViewer::resetZoom(const Trace& p_trace)
{
    m_view_start = 0;
    m_view_end = p_trace.total_duration;
}

// --------------------------------------------------------------------------
void TimelineViewer::handleMouseInteractions(ImVec2 p_canvas_pos,
                                             ImVec2 p_canvas_size,
                                             const Trace& p_trace)
{
    // Zoom in/out with the mouse wheel
    if (ImGui::IsItemHovered() && ImGui::GetIO().MouseWheel != 0.0f)
    {
        scrollZoom(p_trace, p_canvas_pos, p_canvas_size);
    }

    // Zoom from a rectangular selection with mouse
    rectangularZoom(p_trace, p_canvas_pos, p_canvas_size);

    // Right-click context menu
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
    {
        ImGui::OpenPopup("TimelineContextMenu");
    }

    if (ImGui::BeginPopup("TimelineContextMenu"))
    {
        if (ImGui::MenuItem("Reset Zoom"))
        {
            resetZoom(p_trace);
        }
        if (ImGui::MenuItem("Clear All"))
        {
            clearAll();
        }
        if (m_selected_span_index >= 0 &&
            m_selected_span_index < int(p_trace.spans.size()))
        {
            ImGui::Separator();
            if (ImGui::MenuItem("Center on Selected Span"))
            {
                const Span& span = p_trace.spans[m_selected_span_index];
                updateViewToCenterOnSpan(span);
            }
            if (ImGui::MenuItem("Deselect Span"))
            {
                m_selected_span_index = -1;
                m_selected_span_details.clear();
            }
        }
        ImGui::EndPopup();
    }

    // If clicked on the canvas but not on any span, deselect current span
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
    {
        bool clicked_on_span = false;
        float y_offset = p_canvas_pos.y + m_config.timeline_content_offset;
        size_t visual_row = 0;

        // Check if click was on any span
        for (size_t i = 0; i < p_trace.spans.size(); ++i)
        {
            const Span& span = p_trace.spans[i];
            SpanLayout layout = calculateSpanLayout(
                span, y_offset, visual_row, p_canvas_pos, p_canvas_size.x);
            layout.start_x += layout.indent;

            ImVec2 rect_min(layout.start_x,
                            layout.y_pos - m_config.span_height / 2.0f);
            ImVec2 rect_max(layout.start_x + layout.width,
                            layout.y_pos + m_config.span_height / 2.0f);

            if (utils::isPointInRect(ImGui::GetMousePos(), rect_min, rect_max))
            {
                clicked_on_span = true;
                break;
            }

            visual_row++;
        }

        // If clicked outside any span, deselect current span
        if (!clicked_on_span)
        {
            m_selected_span_index = -1;
            m_selected_span_details.clear();
        }
    }
}

// --------------------------------------------------------------------------
std::vector<TimeUnitInfo> TimelineViewer::getAvailableTimeUnits()
{
    return { { TimeUnit::Nanoseconds, "ns", "Nanoseconds", 1.0, 0.0, 1000.0 },
             { TimeUnit::Microseconds,
               "us",
               "Microseconds",
               1000.0,
               1000.0,
               1000000.0 },
             { TimeUnit::Milliseconds,
               "ms",
               "Milliseconds",
               1000000.0,
               1000000.0,
               1000000000.0 },
             { TimeUnit::Seconds,
               "s",
               "Seconds",
               1000000000.0,
               1000000000.0,
               60000000000.0 },
             { TimeUnit::Minutes,
               "min",
               "Minutes",
               60000000000.0,
               60000000000.0,
               3600000000000.0 },
             { TimeUnit::Hours,
               "h",
               "Hours",
               3600000000000.0,
               3600000000000.0,
               std::numeric_limits<double>::max() } };
}

// --------------------------------------------------------------------------
double TimelineViewer::convertTime(double nanoseconds, TimeUnit target_unit)
{
    TimeUnitInfo unit_info = getTimeUnitInfo(target_unit);
    return nanoseconds / unit_info.factor_from_nanoseconds;
}

// --------------------------------------------------------------------------
TimeUnit TimelineViewer::detectBestTimeUnit(double min_value_ns,
                                            double max_value_ns)
{
    std::vector<TimeUnitInfo> units = getAvailableTimeUnits();

    // Use the maximum value to determine the best unit
    double range = std::max(std::abs(min_value_ns), std::abs(max_value_ns));

    for (const auto& unit : units)
    {
        if (range >= unit.min_threshold && range < unit.max_threshold)
        {
            return unit.unit;
        }
    }

    // Default to the largest unit if range is very large
    return TimeUnit::Hours;
}

// --------------------------------------------------------------------------
TimeUnitInfo TimelineViewer::getTimeUnitInfo(TimeUnit unit)
{
    std::vector<TimeUnitInfo> units = getAvailableTimeUnits();
    for (const auto& unit_info : units)
    {
        if (unit_info.unit == unit)
        {
            return unit_info;
        }
    }
    // Default to nanoseconds if not found
    return units[0];
}

// --------------------------------------------------------------------------
std::string TimelineViewer::formatTimeWithUnit(double nanoseconds,
                                               TimeUnit unit)
{
    double converted_time = convertTime(nanoseconds, unit);
    TimeUnitInfo unit_info = getTimeUnitInfo(unit);

    // Choose precision based on the magnitude of the value
    int precision = 1;
    if (converted_time < 1.0)
        precision = 3;
    else if (converted_time < 10.0)
        precision = 2;

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << converted_time << " "
        << unit_info.symbol;
    return oss.str();
}