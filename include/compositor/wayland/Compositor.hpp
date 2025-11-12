#pragma once

#include <wayland-server-protocol.hpp>

namespace moco::wayland::implementation {
    class Compositor : private ::wayland::server::global_compositor_t {
        public:
            Compositor(::wayland::server::display_t display);

        private:
            auto HandleBind(::wayland::server::client_t client, ::wayland::server::compositor_t compositor) -> void;

            /* Request Handlers */
            auto HandleCreateSurface(::wayland::server::surface_t surface) -> void;
            auto HandleCreateRegion(::wayland::server::region_t) -> void;
    };
}  // namespace moco::wayland::implementation
