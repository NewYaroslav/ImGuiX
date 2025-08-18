#pragma once
#ifndef _IMGUIX_CORE_MODEL_IMODEL_HPP_INCLUDED
#define _IMGUIX_CORE_MODEL_IMODEL_HPP_INCLUDED

/// \file Model.hpp
/// \brief Base class for non-visual logic components attached to the application.

namespace ImGuiX {

    /// \brief Base class for non-visual logic models.
    ///
    /// Provides access to application-level services and a safe event interface.
    /// Use this for background logic, state management, and long-lived systems.
    class Model : public Pubsub::EventMediator {
    public:
        /// \brief Constructs the model bound to the application.
        /// \param app Reference to the ApplicationControl interface.
        explicit Model(ApplicationControl& app)
            : EventMediator(app.eventBus()), m_app(app) {}
            
        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        Model(Model&&) = delete;
        Model& operator=(Model&&) = delete;

        virtual ~Model() = default;

                /// \brief Optional initialization callback invoked once.
        virtual void onInit() {}

                /// \brief Called every frame by the application.
        virtual void process() = 0;

        /// \brief Requests the application to close gracefully.
        virtual void close() {
            m_app.close();
        }

        /// \brief Checks whether the application is shutting down.
        /// \return true if the application is in the process of closing.
        virtual bool isClosing() const {
            return m_app.isClosing();
        }

        /// \brief Returns the name of the application.
        virtual const std::string& name() const {
            return m_app.name();
        }

        /// \brief Provides access to the global resource registry.
        ResourceRegistry& registry() {
            return m_app.registry();
        }

        // --- Disable synchronous notify interface (unsafe for multithreading) ---

        void notify(const std::shared_ptr<Pubsub::Event>&) const = delete;
        void notify(const std::unique_ptr<Pubsub::Event>&) const = delete;
        void notify(const Pubsub::Event* const) const = delete;
        void notify(const Pubsub::Event&) const = delete;

    protected:
        ApplicationControl& m_app; ///< Reference to the owning application.
    };

} // namespace ImGuiX

#endif // _IMGUIX_CORE_MODEL_IMODEL_HPP_INCLUDED
