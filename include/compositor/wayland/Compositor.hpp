#pragma once

#include <wayland-server-protocol.hpp>

#include "ObjectImplementationBase.hpp"

namespace moco::wayland::implementation {
    class Compositor : public ObjectImplementationBase<::wayland::server::compositor_t, Compositor> {
            using ObjectImplementationBase::on_destroy;
            using ObjectImplementationBase::on_create_region;
            using ObjectImplementationBase::on_create_surface;
        public:
            Compositor(::wayland::server::compositor_t compositor, Private);


        private:
            Compositor(::wayland::server::compositor_t compositor);

            /* Request Handlers */
            auto HandleCreateSurface(::wayland::server::surface_t surface) -> void;
            auto HandleCreateRegion(::wayland::server::region_t) -> void;
            auto HandleDestroy() -> void;
    };

    class GlobalCompositor : public ::wayland::server::global_compositor_t {
        public:
            GlobalCompositor(::wayland::server::display_t display);

        private:
            static auto HandleBind(::wayland::server::client_t client, ::wayland::server::compositor_t compositor) -> void;
    };
}  // namespace moco::wayland::implementation
