#pragma once
#ifndef _IMGUIX_CORE_TIME_DELTA_CLOCK_SFML_HPP_INCLUDED
#define _IMGUIX_CORE_TIME_DELTA_CLOCK_SFML_HPP_INCLUDED

/// \file DeltaClockSfml.hpp
/// \brief Lightweight wrapper around sf::Clock for delta timing.

#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

namespace ImGuiX {

    /// \brief SFML-specific delta time clock wrapper.
    class DeltaClockSfml {
    public:
        /// \brief Updates internal delta time by restarting the SFML clock.
        void update() {
            m_delta = m_clock.restart();
        }

        /// \brief Returns last delta time value.
        const sf::Time& delta() const {
            return m_delta;
        }

    private:
        sf::Clock m_clock; ///< Underlying SFML clock.
        sf::Time m_delta;  ///< Time elapsed since last update.
    };

} // namespace ImGuiX

#endif // _IMGUIX_CORE_TIME_DELTA_CLOCK_SFML_HPP_INCLUDED