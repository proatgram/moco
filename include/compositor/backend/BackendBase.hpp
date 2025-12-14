#pragma once

#include <wayland-server-protocol.hpp>
#include <memory>

namespace moco::backend {
    template <class Derived>
    class BackendBase {
        protected:
            struct Private {explicit Private();};

        public:
            BackendBase(Private);

            template <typename ...Args>
            inline static auto Initialize(::wayland::server::display_t display, Args ...args) -> void {
                if (!s_backendSingleton) {
                    s_backendSingleton = std::make_shared(Derived(Private(), display, args...));
                }
            }

            inline static auto GetBackend() -> std::shared_ptr<Derived> {
                return s_backendSingleton;
            }

        protected:
            virtual auto BackendLoop() -> void = 0;

        private:
            inline static std::shared_ptr<Derived> s_backendSingleton;
    };
}
