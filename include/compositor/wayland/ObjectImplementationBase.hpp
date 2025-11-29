#pragma once

#include <wayland-server-protocol.hpp>

#include <type_traits>
#include <memory>

namespace moco::wayland::implementation {
    namespace {
        template <class Derived>
        concept ResourceBase = std::is_base_of_v<::wayland::server::resource_t, Derived>;
    }

    template <ResourceBase Resource, typename Derived>
    class ObjectImplementationBase : public Resource, public std::enable_shared_from_this<Derived> {
        using Resource::post_error;
        public:
            /**
             * @brief Creates object implementation for a resource
             *
             * @details Will safely contruct or re-use an instance of
             * an object implementation for a resource. Usage of this
             * function prevents multiple implementation instances
             * to be created for a specific resource.
             *
             * @param `resource`: The resource you are creating
             * or getting implementation for.
             *
             * @param `args`: Other variadic arguments for the construction
             * of the implementation.
             *
             * @return `std::shared_ptr<Derived>`: The new or already
             * created implementation for the specified resource.
             *
             */
            template <typename... Args>
            inline static auto Create(Resource resource, Args&&... args) -> std::shared_ptr<Derived> {
                std::shared_ptr<Derived> implementationInstance;

                // If the resource already has an implementation instance, use that.
                // Otherwise, create a new implementation object and assign it to the resource.
                try {
                    // Can throw if it isn't able to retreieve it as the templated value
                    // (e.g. if it's uninitialized)
                    implementationInstance = resource.user_data().template get<std::shared_ptr<Derived>>();
                } catch (const std::exception &exception) {
                    implementationInstance = std::make_shared<Derived>(resource, std::forward<Args>(args)..., Private());
                    resource.user_data() = implementationInstance;
                }

                return implementationInstance;
            }

            /**
             * @brief Returns the implementation for a resource
             *
             * @details Will safetly return an already constructed instance
             * of an object implementation for a resource. Usage of this
             * function is only valid if the construction already exists and
             * has happened.
             *
             * @param `resource`: The resource you want the implementation for
             *
             * @return `std::shared_ptr<Derived>`: The already created implementationfor the specified resource.
             *
             */
            inline static auto Get(Resource resource) -> std::shared_ptr<Derived> {
                return resource.user_data().template get<std::shared_ptr<Derived>>();
            }

            template <typename ErrorType>
            inline auto PostError(ErrorType error, const std::string &errorMessage) {
                this->post_error(static_cast<int>(error), errorMessage);
            }

            ObjectImplementationBase() = delete;
            ~ObjectImplementationBase() = default;

        protected:
            struct Private{Private() = default;};

            ObjectImplementationBase(const Resource &resource) : Resource(resource) {};
    };
}  // moco::wayland::implementation
