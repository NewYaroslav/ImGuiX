#pragma once
#ifndef _IMGUIX_EVENTS_METRICS_PLOT_UPDATE_EVENT_HPP_INCLUDED
#define _IMGUIX_EVENTS_METRICS_PLOT_UPDATE_EVENT_HPP_INCLUDED

/// \file MetricsPlotUpdateEvent.hpp
/// \brief Emitted when metrics plot data is updated.

#ifdef IMGUI_ENABLE_IMPLOT

#include <memory>
#include <typeindex>

#include "imguix/core/pubsub/Event.hpp"
#include "imguix/widgets/plot/MetricsPlotData.hpp"

namespace ImGuiX::Events {

    /// \brief Carries updated metrics plot data.
    struct MetricsPlotUpdateEvent : Pubsub::Event {
        std::shared_ptr<const Widgets::MetricsPlotData> data; ///< Updated data.

        /// \brief Construct event with data.
        /// \param d Shared metrics plot data.
        explicit MetricsPlotUpdateEvent(
                std::shared_ptr<const Widgets::MetricsPlotData> d)
            : data(std::move(d)) {}

        /// \copydoc Pubsub::Event::type
        std::type_index type() const override {
            return typeid(MetricsPlotUpdateEvent);
        }

        /// \copydoc Pubsub::Event::name
        const char* name() const override {
            return u8"MetricsPlotUpdateEvent";
        }

        /// \copydoc Pubsub::Event::clone
        std::unique_ptr<Event> clone() const override {
            return std::make_unique<MetricsPlotUpdateEvent>(*this);
        }
    };

} // namespace ImGuiX::Events

#endif // IMGUI_ENABLE_IMPLOT

#endif // _IMGUIX_EVENTS_METRICS_PLOT_UPDATE_EVENT_HPP_INCLUDED
