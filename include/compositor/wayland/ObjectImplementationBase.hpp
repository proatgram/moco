#pragma once

#include <wayland-server-protocol.hpp>

#include <type_traits>
#include <memory>

namespace moco::wayland::implementation {
    namespace {
        template <class Derived>
        concept ResourceBase = std::is_base_of_v<::wayland::server::resource_t, Derived>;
    }

    // Don't even ask me to reiterate how this works
    // It's basically just template hell--just use it
    // and don't pay attention to how it works, you'll
    // be better off that way in the long run :)
    template <ResourceBase Resource, typename Derived>
    class ObjectImplementationBase : public Resource {
        public:
            struct ObjectDataBase {
                virtual ~ObjectDataBase();
                std::shared_ptr<Derived> resource;
            };

            /**
             * @brief Creates object implementation for a resource
             *
             * @details Will safely contruct or re-use an instance of
             * an object implementation for a resource. Usage of this
             * function prevents a mismatch between the underlying
             * object data and the implementation pointer.
             *
             * @param `resource`: The resource you are creating
             * implementation for
             *
             * @return `std::shared_ptr<Derived>`: The new or already
             * created implementation for the specified resource
             *
             */
            static auto Create(Resource resource) -> std::shared_ptr<Derived> {
                std::shared_ptr<Derived> ptr;

                // If the resource already has an implementation instance, use that.
                // If we just create another shared_ptr, it creates a mismatch between
                // that shared_ptr and what is referred to in the object datas resource
                try {
                    // Can throw if it doesn't have the value (e.g. if it's uninitialized)
                    ptr = resource.user_data().template get<ObjectData_t<>>()->resource;
                    if (ptr) {
                        return ptr;
                    }
                } catch (const std::exception &exception) {}

                // If either the pointer isn't valid or the object data isn't valid, we
                // are safe to create a new implementation instance
                ptr = std::make_shared<Derived>(resource, Private());
                ptr->GetObjectData()->resource = ptr;
                return ptr;
            }

            // Allows derived classes to easily get the object data as their own type
            template <class T = Derived>
            inline auto GetObjectData() -> std::shared_ptr<typename T::ObjectData> {
                return static_pointer_cast<typename T::ObjectData>(m_objectData);
            }
        protected:
            template <typename T = Derived>
            using ObjectData_t = std::shared_ptr<typename T::ObjectData>;

            struct Private{ Private() = default; };

            inline ObjectImplementationBase(const Resource &resource) : Resource(resource), m_objectData() {
                // Assigns the user data either from or to the raw object
                try {
                    m_objectData = this->user_data().template get<ObjectDataBase_t>();
                } catch (const std::exception &exception) {
                    this->user_data().template get<ObjectDataBase_t>() = m_objectData;
                }
            }

            inline ~ObjectImplementationBase() {
                // Constrains derived to being a derived class of us
                static_assert(std::is_base_of_v<ObjectImplementationBase<Resource, Derived>, Derived>);
            }

        private:
            using ObjectDataBase_t = std::shared_ptr<ObjectDataBase>;
            ObjectDataBase_t m_objectData;
    };
}  // moco::wayland::implementation
