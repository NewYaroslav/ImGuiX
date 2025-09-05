#pragma once
#ifndef _IMGUIX_WIDGETS_PLOT_PLOTOHLCCHART_HPP_INCLUDED
#define _IMGUIX_WIDGETS_PLOT_PLOTOHLCCHART_HPP_INCLUDED

/// \file PlotOHLCChart.hpp
/// \brief OHLC price chart widget built on ImPlot.

#include <imgui.h>
#include <implot.h>
#include <implot_internal.h>
#include <string>
#include <vector>
#include <functional>
#include <cstdio>

#include <imguix/config/colors.hpp>
#include <imguix/widgets/controls/icon_button.hpp>

#include "bars.hpp"
#include "BarAdapter.hpp"

namespace ImGuiX::Widgets {

    /// \brief Configuration for PlotOHLCChart.
    struct OHLCChartConfig {
        ImVec4 bull_color = IMGUIX_COLOR_OHLC_BULL; ///< Color for bullish candles.
        ImVec4 bear_color = IMGUIX_COLOR_OHLC_BEAR; ///< Color for bearish candles.
        ImVec4 no_data_color = ImVec4(0.5f, 0.5f, 0.5f, 0.8f); ///< Color for no-data message.
        ImVec2 plot_size  = ImVec2(-1, 0);          ///< Plot size; (-1,0) uses full width.
        ImVec2 overlay_padding = ImVec2(-1, -1);    ///< (-1,-1) uses style values.

        int price_decimals = 3; ///< Decimal precision for prices.

        bool enable_timeframes = true; ///< Show timeframe combo.
        std::vector<int> timeframes_seconds = {
            60,
            300,
            900,
            3600,
            86400
        }; ///< Available timeframe options in seconds.
        int initial_timeframe = 60; ///< Default timeframe value.
        std::function<void(int)> on_timeframe_changed; ///< Callback on timeframe change.

        std::vector<std::string> symbols{}; ///< Available trading symbols.
        std::string initial_symbol{u8"EURUSD"}; ///< Initially selected symbol.
        std::function<void(const std::string&)> on_symbol_changed; ///< Callback on symbol change.

        bool enable_lines = true; ///< Render user-defined lines.
        struct Line {
            double x1; ///< Line start X (time).
            double y1; ///< Line start Y (price).
            double x2; ///< Line end X (time).
            double y2; ///< Line end Y (price).
            ImVec4 color; ///< Line color.
        };
        std::vector<Line> user_lines; ///< User lines to draw over chart.
        
        bool  follow_latest          = true;   ///< Enable auto-follow on data update.
        int   follow_bars            = 100;    ///< Number of recent bars kept visible.
        float follow_right_pad_bars  = 5.0f;   ///< Extra empty bars on the right.
        bool  follow_unlock_on_input = true;   ///< Disable follow on user input.

        bool show_tooltip = true; ///< Display tooltip on bar hover.
        ImFont* no_data_font = nullptr; ///< Font for no-data message.
        const char* no_data_text   = u8"No data available"; ///< Text shown when no bars.
        const char* icon_last_page = u8"\uE5DD"; ///< Icon for jump-to-last button.
    };

    /// \brief Render an OHLC chart.
    /// \tparam T Bar data type.
    /// \tparam Adapter Adapter providing accessors for bar fields.
    /// \param id Unique plot identifier.
    /// \param bars Bars ordered by ascending time.
    /// \param config Chart rendering configuration.
    template<typename T, typename Adapter = DefaultBarAdapter<T>>
    void PlotOHLCChart(
            const char* id,
            const std::vector<T>& bars,
            const OHLCChartConfig& config
        ) {
        constexpr double kBarHalfWidthFactor = 0.25; // Half-width factor for bar bodies.
        constexpr double kTooltipTolerance   = 0.1;  // Max time diff to show tooltip.
        constexpr float  kUserLineThickness  = 1.5f; // Thickness of user lines.
        constexpr int    kTooltipBufferSize  = 64;   // Tooltip buffer size.
        
        constexpr int kSecondsPerMinute = 60; // Seconds in one minute.
        constexpr int kSecondsPerHour   = 60 * kSecondsPerMinute;
        constexpr int kSecondsPerDay    = 24 * kSecondsPerHour;
            
        static int selected_tf = config.initial_timeframe;
        static std::string selected_symbol = config.initial_symbol;
        
        struct YAutoFitState {
            bool   pending = false;
            double y_min = 0.0, y_max = 0.0; // values applied on next frame
            double last_vx0 = NAN, last_vx1 = NAN; // for debugging/filtering
        };
        static YAutoFitState YF;
        
        struct FollowState {
            size_t last_count = 0;
            double last_back_t = 0.0;
            bool   follow_active = true;
            bool   need_jump_now = true; // set initial window once on first show
        };
        static FollowState F;
        if (!config.follow_latest) F.follow_active = false;
        
        // Select date/time format for tooltip based on timeframe in seconds
        auto PickDateTimeSpec = [](int tf_sec, bool use24h, bool use_iso) -> ImPlotDateTimeSpec {
            using namespace ImPlot;
            
            constexpr int kSecPerMin   = 60;
            constexpr int kSecPerHour  = 3600;
            constexpr int kSecPerDay   = 86400;
            constexpr int kSecPerMonth = 31 * kSecPerDay;
            constexpr int kSecPerYar   = 365 * kSecPerDay;
            
            if (tf_sec < kSecPerMin) {
                // seconds
                return ImPlotDateTimeSpec(ImPlotDateFmt_DayMoYr, ImPlotTimeFmt_HrMinS,  use24h, use_iso);
            } else 
            if (tf_sec < kSecPerHour) {
                // minutes
                return ImPlotDateTimeSpec(ImPlotDateFmt_DayMoYr, ImPlotTimeFmt_HrMin,   use24h, use_iso);
            } else 
            if (tf_sec < kSecPerDay) {
                // hours
                return ImPlotDateTimeSpec(ImPlotDateFmt_DayMoYr, ImPlotTimeFmt_HrMin, use24h, use_iso);
            } else 
            if (tf_sec < kSecPerMonth) {
                // days
                return ImPlotDateTimeSpec(ImPlotDateFmt_DayMoYr, ImPlotTimeFmt_None, use24h, use_iso);
            } else 
            if (tf_sec < kSecPerYar) {
                // months
                return ImPlotDateTimeSpec(ImPlotDateFmt_MoYr,    ImPlotTimeFmt_None, use24h, use_iso);
            } else {
                // years
                return ImPlotDateTimeSpec(ImPlotDateFmt_Yr,      ImPlotTimeFmt_None, use24h, use_iso);
            }
        };
        
        auto MakeTfLabel = [](int tf_sec) -> std::string {
            constexpr int kSecPerMin  = 60;
            constexpr int kSecPerHour = 3600;
            constexpr int kSecPerDay  = 86400;

            if (tf_sec >= kSecPerDay) {
                return "D" + std::to_string(tf_sec / kSecPerDay);
            }
            if (tf_sec >= kSecPerHour) {
                return "H" + std::to_string(tf_sec / kSecPerHour);
            }
            if (tf_sec >= kSecPerMin) {
                return "M" + std::to_string(tf_sec / kSecPerMin);
            }
            if (tf_sec >= 1) {
                return "S" + std::to_string(tf_sec);
            }
            return "S0";
        };

        ImPlotStyle& style = ImPlot::GetStyle();
        bool prev24 = style.Use24HourClock;
        bool prev_iso = style.UseISO8601;
        style.Use24HourClock = true;
        style.UseISO8601 = true;

        if (bars.empty()) {
            if (ImPlot::BeginPlot(id, config.plot_size, ImPlotFlags_NoMenus)) {
                ImPlot::SetupAxis(ImAxis_X1, "Time", ImPlotAxisFlags_NoLabel| ImPlotAxisFlags_NoTickLabels);
                ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
                ImPlot::SetupAxis(ImAxis_Y1, "Price", ImPlotAxisFlags_Opposite | ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoTickLabels);

                ImDrawList* draw = ImPlot::GetPlotDrawList();
                ImPlot::PushPlotClipRect();

                ImFont* font  = (config.no_data_font ? config.no_data_font : ImGui::GetFont());
                ImGui::PushFont(font);
                const ImVec2 tsz  = ImGui::CalcTextSize(config.no_data_text);
                
                ImVec2 pos  = ImPlot::GetPlotPos();
                ImVec2 sz   = ImPlot::GetPlotSize();
                ImVec2 cen  = ImVec2(pos.x + sz.x * 0.5f, pos.y + sz.y * 0.5f);

                draw->AddText(
                    font, 0.0f,//ImGui::GetFontSize(),
                    ImVec2(cen.x - tsz.x * 0.5f, cen.y - tsz.y * 0.5f),
                    ImGui::GetColorU32(config.no_data_color),
                    config.no_data_text
                );
                ImGui::PopFont();
                
                ImPlot::PopPlotClipRect();
                ImPlot::EndPlot();
            }
            style.Use24HourClock = prev24;
            style.UseISO8601 = prev_iso;
            return;
        }
        
        const int dec = (config.price_decimals < 0 ? 0 : (config.price_decimals > 12 ? 12 : config.price_decimals));
        char price_fmt[16];
        std::snprintf(price_fmt, sizeof(price_fmt), "%%.%df", dec);

        // X limits and base candle body width
        const double x0 = Adapter::getTime(bars.front());
        const double x1 = Adapter::getTime(bars.back());

        double tf_sec = selected_tf > 0 ? double(selected_tf) : 1.0;
        if (bars.size() > 1) {
            const double dt = Adapter::getTime(bars[1]) - Adapter::getTime(bars[0]);
            if (dt > 0) tf_sec = dt;
        }
        const double half_w = tf_sec * kBarHalfWidthFactor;

        // Initial Y range over entire history
        double y_min = Adapter::getLow (bars.front());
        double y_max = Adapter::getHigh(bars.front());

        for (size_t i = 1; i < bars.size(); ++i) {
            y_min = std::min(y_min, Adapter::getLow (bars[i]));
            y_max = std::max(y_max, Adapter::getHigh(bars[i]));
        }

        auto pad = [](double a, double b) {
            double p = (b - a) * 0.05;
            if (p <= 0) p = std::max(1.0, std::abs(b) * 0.01);
            return p;
        };

        double ypad0 = pad(y_min, y_max);

        // Event: data updated (new bars or last bar shift)
        const bool data_changed = (F.last_count != bars.size()) || (F.last_back_t != x1);
        if (data_changed) {
            F.last_count = bars.size();
            F.last_back_t = x1;
            if (config.follow_latest)
                F.need_jump_now = true; // jump in next frame
        }
        
        const double right = x1 + tf_sec * config.follow_right_pad_bars;
        const double left  = std::max(
            Adapter::getTime(bars.front()),
            right - tf_sec * std::max(1, config.follow_bars)
        );
        

        ImVec2 pp = ImPlot::GetStyle().PlotPadding;
        const float extra_left = ImGui::CalcTextSize("0000-00-00").x * 0.4f;
        ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(pp.x + extra_left, pp.y));

        if (ImPlot::BeginPlot(id, config.plot_size, ImPlotFlags_NoMenus)) {
            ImPlot::SetupAxisFormat(ImAxis_Y1, price_fmt);
            ImPlot::SetupAxis(ImAxis_X1, "Time", ImPlotAxisFlags_NoLabel);
            ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
            ImPlot::SetupAxis(ImAxis_Y1, "Price", ImPlotAxisFlags_Opposite | ImPlotAxisFlags_NoLabel);
            
            if (YF.pending) {
                ImPlot::SetupAxisLimits(ImAxis_Y1, YF.y_min, YF.y_max, ImPlotCond_Always);
                YF.pending = false;
            }

        // Auto-jump to last N bars when follow active
            if (F.follow_active && (F.need_jump_now || data_changed)) {
                ImPlot::SetupAxisLimits(ImAxis_X1, left, right, ImPlotCond_Always);
                F.need_jump_now = false; // no more jumps until new data arrives
            }

            ImPlot::SetupAxesLimits(x0, x1, y_min - ypad0, y_max + ypad0, ImPlotCond_Once);

        // Unlock follow when user zooms, scrolls or drags
            if (config.follow_unlock_on_input && F.follow_active) {
                bool user_nav =
                    (ImPlot::IsPlotHovered() &&
                     (ImGui::IsMouseDragging(ImGuiMouseButton_Left) ||
                      ImGui::IsMouseDragging(ImGuiMouseButton_Right) ||
                      ImGui::GetIO().MouseWheel != 0.0f)) ||
                    ImPlot::IsAxisHovered(ImAxis_X1); // axis drag
                if (user_nav) F.follow_active = false;
            }

            ImDrawList* draw = ImPlot::GetPlotDrawList();
            ImPlot::PushPlotClipRect();

            // --- tooltip ---
            if (ImPlot::IsPlotHovered() && config.show_tooltip) {
                ImPlotPoint mouse = ImPlot::GetPlotMousePos();
                mouse.x = ImPlot::RoundTime(ImPlotTime::FromDouble(mouse.x), ImPlotTimeUnit_S).ToDouble();

                const double tol = 0.45 * tf_sec; // X snap tolerance to bar
                auto it = std::find_if(bars.begin(), bars.end(), [&](const T& bar){
                    return std::abs(Adapter::getTime(bar) - mouse.x) < tol;
                });

                if (it != bars.end()) {
                    ImGui::BeginTooltip();
                    ImPlotTime t = Adapter::getTimeExact(*it);
                    char buf[kTooltipBufferSize];
                    ImPlot::FormatDateTime(t, buf, kTooltipBufferSize, PickDateTimeSpec(selected_tf, true, true));
                    
                    auto PrintPrice = [&](const char* label, double v) {
                        char fmt[32];
                        std::snprintf(fmt, sizeof(fmt), "%s  %s", label, price_fmt); // e.g. "Open:  %.5f"
                        ImGui::Text(fmt, v);
                    };

                    PrintPrice("Open:",  Adapter::getOpen(*it));
                    PrintPrice("High:",  Adapter::getHigh(*it));
                    PrintPrice("Low:",   Adapter::getLow (*it));
                    PrintPrice("Close:", Adapter::getClose(*it));
                    
                    ImGui::Text("Time:  %s", buf);
                    ImGui::EndTooltip();
                }
            }
            
            // Render bars

            // Helpers for pixel-perfect rendering
            auto SnapHalf = [](float x) -> float { return std::floor(x) + 0.5f; };
            auto PxX = [](double tx) -> float { return ImPlot::PlotToPixels(tx, 0.0).x; };
            auto PxY = [](double ty) -> float { return ImPlot::PlotToPixels(0.0, ty).y; };

            // Parameters (could move to config)
            const float kMinBodyPx       = 1.0f;  // minimum body width in pixels
            const float kThinThresholdPx = 2.0f;  // below this draw in thin mode
            const float kWickThickness   = 1.0f;  // wick thickness in pixels
            const float kBodyLineThick   = 1.0f;  // body thickness in thin mode

            for (const auto& bar : bars) {
                const double t = Adapter::getTime(bar);
                const double o = Adapter::getOpen(bar);
                const double h = Adapter::getHigh(bar);
                const double l = Adapter::getLow (bar);
                const double c = Adapter::getClose(bar);

                const ImU32 col = ImGui::GetColorU32(c >= o ? config.bull_color : config.bear_color);

                // Body center and width in pixels (from time)
                float xL = PxX(t - half_w);
                float xR = PxX(t + half_w);
                if (xL > xR) std::swap(xL, xR);
                float cx = 0.5f * (xL + xR);        // body center before snapping
                float wpx = ImMax(kMinBodyPx, xR - xL); // body width in pixels, at least 1px

                // Snap to half-pixel for crisp 1px lines
                cx = SnapHalf(cx);
                float half = 0.5f * wpx;
                float left  = SnapHalf(cx - half);
                float right = SnapHalf(cx + half);
                if (right - left < kMinBodyPx) { // ensure at least 1px
                    left = cx - 0.5f; right = cx + 0.5f;
                }

                // Y coordinates in pixels
                float yO = PxY(o);
                float yH = PxY(h);
                float yL = PxY(l);
                float yC = PxY(c);

                // Wick centered on body
                ImVec2 wickTop (cx, yH);
                ImVec2 wickBot (cx, yL);

                // Optionally disable AA for lines for extra sharpness
                const ImDrawListFlags old_flags = draw->Flags;
                draw->Flags &= ~ImDrawListFlags_AntiAliasedLines;

                draw->AddLine(wickBot, wickTop, col, kWickThickness);

                // Body
                if ((right - left) <= kThinThresholdPx) {
                    // Thin mode: vertical candle between open and close
                    draw->AddLine(ImVec2(cx, yO), ImVec2(cx, yC), col, kBodyLineThick);
                } else {
                    ImVec2 p0(left,  ImMin(yO, yC));
                    ImVec2 p1(right, ImMax(yO, yC));
                    draw->AddRectFilled(p0, p1, col);
                }

                draw->Flags = old_flags; // restore AA
            }
            
            // User lines
            if (config.enable_lines) {
                for (const auto& line : config.user_lines) {
                    draw->AddLine(ImPlot::PlotToPixels(line.x1, line.y1),
                                  ImPlot::PlotToPixels(line.x2, line.y2),
                                  ImGui::GetColorU32(line.color),
                                  kUserLineThickness);
                }
            }
            
            // Button jump to latest bars inside plot
            {
                const ImVec2 plot_pos  = ImPlot::GetPlotPos();
                const ImVec2 plot_size = ImPlot::GetPlotSize();

                ImVec2 pad = config.overlay_padding;
                if (pad.x < 0 || pad.y < 0) {
                    const ImGuiStyle& s = ImGui::GetStyle();
                    pad = s.ItemInnerSpacing;
                }

                // Square button sized by current frame height
                const float btn_w = ImGui::GetFrameHeight();

                // Position: top-right corner with padding
                const ImVec2 btn_pos(plot_pos.x + plot_size.x - pad.x - btn_w,
                                     plot_pos.y + pad.y);

                // Clip to plot area (use ImGui::PushClipRect for ImGui widgets)
                ImGui::PushClipRect(plot_pos,
                                    ImVec2(plot_pos.x + plot_size.x, plot_pos.y + plot_size.y),
                                    true);

                // Set cursor absolutely and draw button
                ImGui::SetCursorScreenPos(btn_pos);

                ImGuiX::Widgets::IconButtonConfig ibc;
                ibc.appear_on_hover = true;

                // Material Icons: "last_page" ▶| (U+E5DD). Alt: fast_forward U+E01F
                constexpr const char* ICON_LAST_PAGE = u8"\uE5DD";

                if (ImGuiX::Widgets::IconButtonCentered("jump_last_inplot", config.icon_last_page, ibc)) {
                    F.follow_active = true;
                    F.need_jump_now = true;
                }
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Jump to latest");

                ImGui::PopClipRect();
            }
            
            // Context popup via right mouse button
            if (ImPlot::IsPlotHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
                ImGui::OpenPopup("##ohlc_custom_menu");
            }
            if (ImGui::BeginPopup("##ohlc_custom_menu")) {
                ImGui::TextUnformatted("Chart settings");
                ImGui::Separator();

                // Example: manage follow state via runtime data
                ImGui::Checkbox("Follow latest", &F.follow_active);
                if (ImGui::MenuItem("Jump to latest")) { F.follow_active = true; F.need_jump_now = true; }

                // static bool show_tt = config.show_tooltip; use show_tt instead of config.show_tooltip
                // ImGui::Checkbox("Tooltip", &show_tt);

                ImGui::EndPopup();
            }
            
            // Capture visible X and prepare Y for next frame
            {
                // Skip if user drags Y axis
                bool user_drag_y = ImPlot::IsAxisHovered(ImAxis_Y1) &&
                                   (ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsMouseDown(ImGuiMouseButton_Right));
                if (!user_drag_y) {
                    ImPlotRect view = ImPlot::GetPlotLimits(); // current visible limits in axis coordinates
                    double vx0 = std::max(view.X.Min, Adapter::getTime(bars.front()));
                    double vx1 = std::min(view.X.Max, Adapter::getTime(bars.back()));
                    if (vx1 > vx0) {
                        // Fast range search (bars sorted by time)
                        auto it0 = std::lower_bound(bars.begin(), bars.end(), vx0,
                            [](const T& b, double x){ return Adapter::getTime(b) < x; });
                        auto it1 = std::upper_bound(bars.begin(), bars.end(), vx1,
                            [](double x, const T& b){ return x < Adapter::getTime(b); });

                        if (it0 != it1) {
                            double yf_min = Adapter::getLow (*it0);
                            double yf_max = Adapter::getHigh(*it0);
                            for (auto it = it0; it != it1; ++it) {
                                yf_min = std::min(yf_min, Adapter::getLow (*it));
                                yf_max = std::max(yf_max, Adapter::getHigh(*it));
                            }
                            auto pad = [](double a, double b) {
                                double p = (b - a) * 0.05;
                                if (p <= 0) p = std::max(1.0, std::abs(b) * 0.01);
                                return p;
                            };
                            const double p = pad(yf_min, yf_max);

                            // Store and apply at start of next frame
                            YF.y_min   = yf_min - p;
                            YF.y_max   = yf_max + p;
                            YF.pending = true;
                            YF.last_vx0 = vx0; YF.last_vx1 = vx1;
                        }
                    }
                }
            }

            ImPlot::PopPlotClipRect();
            ImPlot::EndPlot();
        }
        ImPlot::PopStyleVar();
        style.Use24HourClock = prev24;
        style.UseISO8601 = prev_iso;
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_PLOT_PLOTOHLCCHART_HPP_INCLUDED
