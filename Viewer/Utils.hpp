#pragma once

#include <algorithm>
#include <cctype>
#include <imgui.h>
#include <string>

namespace utils
{
// --------------------------------------------------------------------------
//! \brief Convert string to lowercase
//! \param p_str The string to convert
//! \return Lowercase version of the string
// --------------------------------------------------------------------------
std::string toLower(const std::string& p_str);

// --------------------------------------------------------------------------
//! \brief Check if string contains substring (case-insensitive)
//! \param p_str The string to search in
//! \param p_substr The substring to search for
//! \return true if substring is found, false otherwise
// --------------------------------------------------------------------------
bool containsIgnoreCase(const std::string& p_str, const std::string& p_substr);

// --------------------------------------------------------------------------
//! \brief Format time duration to string
//! \param p_duration_ms Duration in milliseconds
//! \return Formatted string representation
// --------------------------------------------------------------------------
std::string formatDuration(double p_duration_ms);

// --------------------------------------------------------------------------
//! \brief Enhance color brightness for minimap visibility
//! \param p_color Original color
//! \param p_boost_factor Brightness boost factor (default 1.4)
//! \return Enhanced color
// --------------------------------------------------------------------------
ImU32 enhanceColorBrightness(ImU32 p_color, float p_boost_factor = 1.4f);

// --------------------------------------------------------------------------
//! \brief Darken a color by a factor
//! \param p_color Original color
//! \param p_darken_factor Darkening factor (0.0 to 1.0, where 1.0 is darkest)
//! \return Darkened color
// --------------------------------------------------------------------------
ImU32 darkenColor(ImU32 p_color, float p_darken_factor = 0.5f);

// --------------------------------------------------------------------------
//! \brief Clamp value between min and max
//! \param p_value Value to clamp
//! \param p_min_val Minimum value
//! \param p_max_val Maximum value
//! \return Clamped value
// --------------------------------------------------------------------------
template <typename T>
T clamp(T p_value, T p_min_val, T p_max_val)
{
    return std::max(p_min_val, std::min(p_value, p_max_val));
}

// --------------------------------------------------------------------------
//! \brief Convert time to pixel position
//! \param p_time Time value
//! \param p_view_start Start of view window
//! \param p_view_end End of view window
//! \param p_canvas_width Width of canvas
//! \return Pixel position
// --------------------------------------------------------------------------
float timeToPixel(double p_time,
                  double p_view_start,
                  double p_view_end,
                  float p_canvas_width);

// --------------------------------------------------------------------------
//! \brief Convert pixel position to time
//! \param p_pixel Pixel position
//! \param p_canvas_pos Canvas position
//! \param p_canvas_width Width of canvas
//! \param p_view_start Start of view window
//! \param p_view_end End of view window
//! \return Time value
// --------------------------------------------------------------------------
double pixelToTime(float p_pixel,
                   float p_canvas_pos,
                   float p_canvas_width,
                   double p_view_start,
                   double p_view_end);

// --------------------------------------------------------------------------
//! \brief Generate color from string hash
//! \param p_input String to hash for color generation
//! \return ImU32 color value
// --------------------------------------------------------------------------
ImU32 generateColorFromHash(const std::string& p_input);

// --------------------------------------------------------------------------
//! \brief Check if point is inside rectangle
//! \param p_point Point to check
//! \param p_rect_min Rectangle minimum point
//! \param p_rect_max Rectangle maximum point
//! \return true if point is inside rectangle, false otherwise
// --------------------------------------------------------------------------
bool isPointInRect(const ImVec2& p_point,
                   const ImVec2& p_rect_min,
                   const ImVec2& p_rect_max);

// --------------------------------------------------------------------------
//! \brief Check if rectangle is clicked
//! \param p_rect_min Rectangle minimum point
//! \param p_rect_max Rectangle maximum point
//! \return true if rectangle is clicked, false otherwise
// --------------------------------------------------------------------------
bool isRectClicked(const ImVec2& p_rect_min, const ImVec2& p_rect_max);

// Calculate ratio between a value and a range
double
calculateTimeRatio(double p_time, double p_view_start, double p_view_end);

// Linear interpolation between two values
double interpolate(double p_start, double p_end, double p_ratio);
} // namespace utils