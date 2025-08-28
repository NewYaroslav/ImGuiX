#include <algorithm>
#include <cmath>
#include <string>

namespace ImGuiX::Widgets {

    namespace detail {
        inline constexpr double kBarHalfWidthFactor = 0.25; ///< Half-width factor for bar bodies.
        inline constexpr double kTooltipTolerance   = 0.1;  ///< Max time diff to show tooltip.
        inline constexpr float  kUserLineThickness  = 1.5f; ///< Thickness of user lines.
        inline constexpr int    kTooltipBufferSize  = 64;   ///< Tooltip buffer size.
        const ImVec4 kNoDataColor{1.0f, 0.3f, 0.3f, 1.0f};  ///< Color for no-data message.
    } // namespace detail

    template<typename T, typename Adapter>
    void PlotOHLCChart(
            const char* id,
            const std::vector<T>& bars,
            OHLCChartConfig& config
    ) {
        static int selected_tf = config.initial_timeframe;
        static std::string selected_symbol = config.initial_symbol;

        if (!config.symbols.empty()) {
            ImGui::TextUnformatted("Symbol:");
            ImGui::SameLine();
            if (ImGui::BeginCombo("##symbol", selected_symbol.c_str())) {
                for (const auto& sym : config.symbols) {
                    const bool selected = (sym == selected_symbol);
                    if (ImGui::Selectable(sym.c_str(), selected)) {
                        selected_symbol = sym;
                        if (config.on_symbol_changed) {
                            config.on_symbol_changed(sym);
                        }
                    }
                }
                ImGui::EndCombo();
            }
        } else {
            ImGui::Text("Symbol: %s", selected_symbol.c_str());
        }

        if (config.enable_timeframes) {
            ImGui::SameLine();
            ImGui::TextUnformatted("Timeframe:");
            ImGui::SameLine();
            std::string tf_label = std::to_string(selected_tf);
            if (selected_tf >= kSecondsPerDay) {
                tf_label = "D" + std::to_string(selected_tf / kSecondsPerDay);
            } else if (selected_tf >= kSecondsPerHour) {
                tf_label = "H" + std::to_string(selected_tf / kSecondsPerHour);
            } else if (selected_tf >= kSecondsPerMinute) {
                tf_label = "M" + std::to_string(selected_tf / kSecondsPerMinute);
            } else {
                tf_label = "S" + std::to_string(selected_tf);
            }

            if (ImGui::BeginCombo("##tf", tf_label.c_str())) {
                for (int tf : config.timeframes_seconds) {
                    std::string label;
                    if (tf >= kSecondsPerDay) {
                        label = "D" + std::to_string(tf / kSecondsPerDay);
                    } else if (tf >= kSecondsPerHour) {
                        label = "H" + std::to_string(tf / kSecondsPerHour);
                    } else if (tf >= kSecondsPerMinute) {
                        label = "M" + std::to_string(tf / kSecondsPerMinute);
                    } else {
                        label = "S" + std::to_string(tf);
                    }

                    const bool selected = (tf == selected_tf);
                    if (ImGui::Selectable(label.c_str(), selected)) {
                        selected_tf = tf;
                        if (config.on_timeframe_changed) {
                            config.on_timeframe_changed(tf);
                        }
                    }
                }
                ImGui::EndCombo();
            }
        }

        if (bars.empty()) {
            ImGui::TextColored(detail::kNoDataColor, "%s", config.no_data_text);
            return;
        }

        if (ImPlot::BeginPlot(id, config.plot_size)) {
            ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
            ImPlot::SetupAxes("Time", "Price");
            ImPlot::SetupAxesLimits(
                Adapter::getTime(bars.front()),
                Adapter::getTime(bars.back()),
                0,
                0,
                ImPlotCond_Once
            );

            ImDrawList* draw = ImPlot::GetPlotDrawList();
            const double width = bars.size() > 1
                ? (Adapter::getTime(bars[1]) - Adapter::getTime(bars[0])) * detail::kBarHalfWidthFactor
                : selected_tf * detail::kBarHalfWidthFactor;

            if (ImPlot::IsPlotHovered() && config.show_tooltip) {
                ImPlotPoint mouse = ImPlot::GetPlotMousePos();
                mouse.x = ImPlot::RoundTime(
                        ImPlotTime::FromDouble(mouse.x),
                        ImPlotTimeUnit_S
                ).ToDouble();

                const auto it = std::find_if(
                        bars.begin(),
                        bars.end(),
                        [&](const T& bar) {
                            return std::abs(
                                    Adapter::getTime(bar) - mouse.x
                                ) < detail::kTooltipTolerance;
                        }
                );

                if (it != bars.end()) {
                    ImGui::BeginTooltip();
                    ImPlotTime t = ImPlotTime::FromDouble(Adapter::getTime(*it));
                    char buf[detail::kTooltipBufferSize];
                    ImPlot::FormatDateTime(
                            t,
                            buf,
                            detail::kTooltipBufferSize,
                            ImPlotDateTimeFmt_DayMoYrHrMin,
                            false
                    );
                    ImGui::Text("Time:  %s", buf);
                    ImGui::Text("Open:  %.2f", Adapter::getOpen(*it));
                    ImGui::Text("High:  %.2f", Adapter::getHigh(*it));
                    ImGui::Text("Low:   %.2f", Adapter::getLow(*it));
                    ImGui::Text("Close: %.2f", Adapter::getClose(*it));
                    ImGui::EndTooltip();
                }
            }

            for (const auto& bar : bars) {
                const double t = Adapter::getTime(bar);
                const ImVec2 open = ImPlot::PlotToPixels(
                        t - width,
                        Adapter::getOpen(bar)
                );
                const ImVec2 close = ImPlot::PlotToPixels(
                        t + width,
                        Adapter::getClose(bar)
                );
                const ImVec2 high = ImPlot::PlotToPixels(
                        t,
                        Adapter::getHigh(bar)
                );
                const ImVec2 low = ImPlot::PlotToPixels(
                        t,
                        Adapter::getLow(bar)
                );
                const ImU32 col = ImGui::GetColorU32(
                        Adapter::getClose(bar) >= Adapter::getOpen(bar)
                            ? config.bull_color
                            : config.bear_color
                );
                draw->AddLine(low, high, col);
                draw->AddRectFilled(open, close, col);
            }

            if (config.enable_lines) {
                for (const auto& line : config.user_lines) {
                    const ImVec2 p1 = ImPlot::PlotToPixels(line.x1, line.y1);
                    const ImVec2 p2 = ImPlot::PlotToPixels(line.x2, line.y2);
                    draw->AddLine(
                            p1,
                            p2,
                            ImGui::GetColorU32(line.color),
                            detail::kUserLineThickness
                    );
                }
            }

            ImPlot::EndPlot();
        }
    }

} // namespace ImGuiX::Widgets

