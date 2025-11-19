#pragma once

#include "ObjectImplementationBase.hpp"

#include "Buffer.hpp"

#include <vector>
#include <wayland-server-protocol.hpp>

namespace moco::wayland::implementation {
    struct SurfaceState {

    };

    class Surface : public ObjectImplementationBase<::wayland::server::surface_t, Surface> {
        using ObjectImplementationBase::on_destroy;
        using ObjectImplementationBase::on_attach;
        using ObjectImplementationBase::on_commit;
        using ObjectImplementationBase::on_damage;
        using ObjectImplementationBase::on_damage_buffer;
        using ObjectImplementationBase::on_set_buffer_scale;
        using ObjectImplementationBase::on_set_buffer_transform;
        using ObjectImplementationBase::on_frame;
        using ObjectImplementationBase::on_offset;
        using ObjectImplementationBase::on_set_input_region;
        using ObjectImplementationBase::on_set_opaque_region;

    public:
        Surface(::wayland::server::surface_t surface, Private);

    private:
        Surface(::wayland::server::surface_t surface);



    };
}  // namesoace moco::wayland::implementation
