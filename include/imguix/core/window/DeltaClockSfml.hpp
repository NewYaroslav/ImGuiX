#pragma once
#ifndef _IMGUIX_CORE_TIME_DELTA_CLOCK_SFML_HPP_INCLUDED
#define _IMGUIX_CORE_TIME_DELTA_CLOCK_SFML_HPP_INCLUDED

/// \file DeltaClockSfml.hpp
/// \brief Wrap sf::Clock for delta timing.
/// \ingroup Core

#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

namespace ImGuiX {

    /// \brief Track frame delta time using sf::Clock.
    class DeltaClockSfml {
    public:
        /// \brief Restart SFML clock and store elapsed time.
        void update() {
            m_delta = m_clock.restart();
        }

        /// \brief Return elapsed time since last update.
        /// \return Time since last update.
        const sf::Time& delta() const {
            return m_delta;
        }

    private:
        sf::Clock m_clock; ///< Underlying SFML clock.
        sf::Time m_delta;  ///< Time elapsed since last update.
    };

} // namespace ImGuiX

#endif // _IMGUIX_CORE_TIME_DELTA_CLOCK_SFML_HPP_INCLUDED
