#include "Utils.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <sstream>

namespace utils
{
// --------------------------------------------------------------------------
std::string toLower(const std::string& p_str)
{
    std::string result = p_str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// --------------------------------------------------------------------------
bool containsIgnoreCase(const std::string& p_str, const std::string& p_substr)
{
    if (p_substr.empty())
        return true;

    std::string str_lower = toLower(p_str);
    std::string substr_lower = toLower(p_substr);
    return str_lower.find(substr_lower) != std::string::npos;
}

// --------------------------------------------------------------------------
std::string formatDuration(double p_duration_ms)
{
    std::ostringstream oss;
    if (p_duration_ms < 1.0)
    {
        oss << std::fixed << std::setprecision(3) << p_duration_ms << "ms";
    }
    else if (p_duration_ms < 1000.0)
    {
        oss << std::fixed << std::setprecision(1) << p_duration_ms << "ms";
    }
    else
    {
        double seconds = p_duration_ms / 1000.0;
        oss << std::fixed << std::setprecision(2) << seconds << "s";
    }
    return oss.str();
}

// --------------------------------------------------------------------------
ImU32 enhanceColorBrightness(ImU32 p_color, float p_boost_factor)
{
    // Extract RGB components
    int r = (p_color >> 16) & 0xFF;
    int g = (p_color >> 8) & 0xFF;
    int b = p_color & 0xFF;
    int a = (p_color >> 24) & 0xFF;

    // Boost brightness with minimum threshold for visibility
    r = std::min(255, std::max(100, int(float(r) * p_boost_factor)));
    g = std::min(255, std::max(100, int(float(g) * p_boost_factor)));
    b = std::min(255, std::max(100, int(float(b) * p_boost_factor)));

    return IM_COL32(r, g, b, a);
}

// --------------------------------------------------------------------------
ImU32 darkenColor(ImU32 p_color, float p_darken_factor)
{
    // Extract RGB components
    int r = (p_color >> 16) & 0xFF;
    int g = (p_color >> 8) & 0xFF;
    int b = p_color & 0xFF;
    int a = (p_color >> 24) & 0xFF;

    // Darken each component by the factor
    a = int(float(a) * (1.0f - p_darken_factor));

    return IM_COL32(r, g, b, a);
}

// --------------------------------------------------------------------------
double calculateTimeRatio(double p_time, double p_view_start, double p_view_end)
{
    if (p_view_end <= p_view_start)
        return 0.0;
    return (p_time - p_view_start) / (p_view_end - p_view_start);
}

// --------------------------------------------------------------------------
double interpolate(double p_start, double p_end, double p_ratio)
{
    return std::lerp(p_start, p_end, p_ratio);
}

// --------------------------------------------------------------------------
float timeToPixel(double p_time,
                  double p_view_start,
                  double p_view_end,
                  float p_canvas_width)
{
    double ratio = calculateTimeRatio(p_time, p_view_start, p_view_end);
    return float(ratio * p_canvas_width);
}

// --------------------------------------------------------------------------
double pixelToTime(float p_pixel,
                   float p_canvas_pos,
                   float p_canvas_width,
                   double p_view_start,
                   double p_view_end)
{
    if (p_canvas_width <= 0.0f)
        return p_view_start;

    double ratio = (p_pixel - p_canvas_pos) / p_canvas_width;
    return interpolate(p_view_start, p_view_end, ratio);
}

// --------------------------------------------------------------------------
ImU32 generateColorFromHash(const std::string& p_input)
{
    // Generate a hash from the input string
    std::hash<std::string> hasher;
    size_t hash = hasher(p_input);

    // Extract RGB components from different parts of the hash
    // This ensures good color distribution across different inputs
    ImU32 color = IM_COL32((hash & 0xFF0000) >> 16, // Red from bits 16-23
                           (hash & 0x00FF00) >> 8,  // Green from bits 8-15
                           (hash & 0x0000FF),       // Blue from bits 0-7
                           255);                    // Full alpha (opaque)
    return color;
}

// --------------------------------------------------------------------------
bool isPointInRect(const ImVec2& p_point,
                   const ImVec2& p_rect_min,
                   const ImVec2& p_rect_max)
{
    return p_point.x >= p_rect_min.x && p_point.x <= p_rect_max.x &&
           p_point.y >= p_rect_min.y && p_point.y <= p_rect_max.y;
}

// --------------------------------------------------------------------------
bool isRectClicked(const ImVec2& p_rect_min, const ImVec2& p_rect_max)
{
    ImVec2 mouse_pos = ImGui::GetMousePos();
    return ImGui::IsMouseClicked(0) &&
           isPointInRect(mouse_pos, p_rect_min, p_rect_max);
}
} // namespace utils