#pragma once
#ifndef _IMGUIX_EVENTS_METRICS_PLOT_SET_UPDATE_EVENT_HPP_INCLUDED
#define _IMGUIX_EVENTS_METRICS_PLOT_SET_UPDATE_EVENT_HPP_INCLUDED

/// \file MetricsPlotSetUpdateEvent.hpp
/// \brief Emitted when a MetricsPlotSet data collection is updated.

#ifdef IMGUIX_ENABLE_IMPLOT

#include <memory>
#include <typeindex>

#include <imguix/core/pubsub/Event.hpp>
#include <imguix/widgets/plot/MetricsPlotSetData.hpp>

namespace ImGuiX::Events {

    /// \brief Carries updated MetricsPlotSet data.
    struct MetricsPlotSetUpdateEvent : Pubsub::Event {
        std::shared_ptr<const Widgets::MetricsPlotSetData> data; ///< Updated data.

        /// \brief Construct event with data.
        /// \param d Shared MetricsPlotSet data.
        explicit MetricsPlotSetUpdateEvent(
                std::shared_ptr<const Widgets::MetricsPlotSetData> d)
            : data(std::move(d)) {}

        /// \copydoc Pubsub::Event::type
        std::type_index type() const override {
            return typeid(MetricsPlotSetUpdateEvent);
        }

        /// \copydoc Pubsub::Event::name
        const char* name() const override {
            return u8"MetricsPlotSetUpdateEvent";
        }

        /// \copydoc Pubsub::Event::clone
        std::unique_ptr<Event> clone() const override {
            return std::make_unique<MetricsPlotSetUpdateEvent>(*this);
        }
    };

} // namespace ImGuiX::Events

#endif // IMGUIX_ENABLE_IMPLOT

#endif // _IMGUIX_EVENTS_METRICS_PLOT_SET_UPDATE_EVENT_HPP_INCLUDED
