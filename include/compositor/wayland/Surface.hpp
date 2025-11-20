#pragma once

#include "ObjectImplementationBase.hpp"
#include "Buffer.hpp"

#include <memory>
#include <wayland-server-protocol.hpp>
#include <pixman.h>

namespace moco::wayland::implementation {
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
        struct Area {
            int x, y;
            size_t width, height;
        };

        class SurfaceState {
            public:
                SurfaceState(const std::shared_ptr<Buffer> &buffer);
                SurfaceState() = default;
                ~SurfaceState();

                SurfaceState(const SurfaceState &other);
                SurfaceState(SurfaceState &&other) = delete;

                auto AddBuffer(const std::shared_ptr<Buffer> &buffer) -> void;
                auto GetBuffer() const -> std::shared_ptr<Buffer>;
                auto AddDamage(int x, int y, int width, int height) -> void;
                auto GetDamageTracker() const -> pixman_region32_t;
                auto GetDamagedRegions() const -> std::vector<Area>;

                // Will not reset the underlying buffer, only resets modifications
                // since last transaction.
                // Buffer changing happens when AddBuffer is called ONLY.
                auto Reset() -> void;

            private:
                std::shared_ptr<Buffer> m_surfaceBuffer;
                pixman_region32_t m_damageTracker;

                ::wayland::server::output_transform m_bufferTransform = ::wayland::server::output_transform::normal;
                int m_bufferScale;
        };
    private:
        Surface(::wayland::server::surface_t surface);

        auto HandleAttach(::wayland::server::buffer_t buffer, int x, int y) -> void;
        auto HandleDamage(int x, int y, int width, int height) -> void;
        auto HandleBufferDamage(int x, int y, int width, int height) -> void;
        
        // Helper function to apply the given transformations and return
        // a vector holding the resulting transformations
        auto ApplyTransformations(const std::vector<Area> &areas, int scale, ::wayland::server::output_transform transform) -> std::vector<Area>;

        SurfaceState m_pendingState;
        SurfaceState m_activeState;

    };
}  // namespace moco::wayland::implementation
