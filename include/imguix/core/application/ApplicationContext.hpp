#pragma once
#ifndef _IMGUIX_CORE_APPLICATION_CONTEXT_HPP_INCLUDED
#define _IMGUIX_CORE_APPLICATION_CONTEXT_HPP_INCLUDED

/// \file ApplicationContext.hpp
/// \brief Interface for application-level context and global services.

#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

namespace ImGuiX {

    class WindowInstance;
    class Model;

    /// \brief Interface for application-level context and global services.
    /// \note Allows components to interact with core application functionality without tight coupling.
    /// \note Provides access to closing application, application name, event bus, and resource registry.
    class ApplicationContext {
    public:
        virtual ~ApplicationContext() = default;

        /// \brief Create and register a window of the specified type.
        /// \tparam WindowType Type derived from WindowInstance.
        /// \tparam Args Arguments forwarded to the constructor.
        /// \param args Constructor arguments.
        /// \return Reference to the created window.
        template<class WindowType, class... Args>
        WindowType& createWindow(Args&&... args) {
            static_assert(std::is_base_of_v<WindowInstance, WindowType>,
                    "WindowType must derive from WindowInstance");
            auto factory = [&, this](int id) {
                return std::make_unique<WindowType>(
                        id,
                        *this,
                        std::forward<Args>(args)...);
            };
            return static_cast<WindowType&>(createWindowImpl(factory));
        }

        /// \brief Create and register a model of the specified type.
        /// \tparam ModelType Type derived from Model.
        /// \tparam Args Arguments forwarded to the constructor.
        /// \param args Constructor arguments.
        /// \return Reference to the created model.
        template<class ModelType, class... Args>
        ModelType& createModel(Args&&... args) {
            static_assert(std::is_base_of_v<Model, ModelType>,
                    "ModelType must derive from Model");
            auto factory = [&, this]() {
                return std::make_unique<ModelType>(
                        *this,
                        std::forward<Args>(args)...);
            };
            return static_cast<ModelType&>(createModelImpl(factory));
        }

        /// \brief Requests the application to close gracefully.
        virtual void close() = 0;

        /// \brief Check whether the application is shutting down.
        /// \return True if application is closing.
        virtual bool isClosing() const = 0;

        /// \brief Returns the name of the application, if defined.
        /// \return Constant reference to the application name.
        virtual const std::string& name() const = 0;

        /// \brief Provides access to the global event bus.
        /// \return Reference to the event bus instance.
        virtual Pubsub::EventBus& eventBus() = 0;

        /// \brief Provides access to the global resource registry.
        /// \return Reference to the resource registry.
        virtual ResourceRegistry& registry() = 0;

    protected:
        using WindowFactory = std::function<std::unique_ptr<WindowInstance>(int)>;
        using ModelFactory  = std::function<std::unique_ptr<Model>()>;

        /// \brief Implementation hook for window creation.
        /// \param factory Factory functor constructing the window.
        /// \return Reference to the created window.
        virtual WindowInstance& createWindowImpl(WindowFactory factory) = 0;

        /// \brief Implementation hook for model creation.
        /// \param factory Factory functor constructing the model.
        /// \return Reference to the created model.
        virtual Model& createModelImpl(ModelFactory factory) = 0;
    };

} // namespace ImGuiX

#endif // _IMGUIX_CORE_APPLICATION_CONTEXT_HPP_INCLUDED

