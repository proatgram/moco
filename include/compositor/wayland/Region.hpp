#pragma once

#include <wayland-server-protocol.hpp>
#include <pixman.h>

#include "ObjectImplementationBase.hpp"

namespace moco::wayland::implementation {
        class Region : public ObjectImplementationBase<::wayland::server::region_t, Region> {
                using ObjectImplementationBase::on_destroy;
                using ObjectImplementationBase::on_add;
                using ObjectImplementationBase::on_subtract;

            public:
                Region(::wayland::server::region_t region, Private);
                ~Region();

            private:
                Region(::wayland::server::region_t region);

                auto HandleDestory() -> void;
                auto HandleAdd(int x, int y, unsigned int width, unsigned int height) -> void;
                auto HandleSubtract(int x, int y, unsigned int width, unsigned int height) -> void;

                pixman_region32_t m_region;

        };
}  // namespace moco::wayland::implementation
