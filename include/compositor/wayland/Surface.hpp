#pragma once

#include "ObjectImplementationBase.hpp"
#include "Buffer.hpp"
#include "Region.hpp"

#include <memory>
#include <queue>
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

        /**
         * @brief Surface State Tracker
         *
         * @details A class that tracks transactional states
         * for wl_surface interface implementations.
         *
         */
        class SurfaceState {
            public:
                SurfaceState(const std::shared_ptr<Buffer> &buffer);
                SurfaceState() = default;
                ~SurfaceState();

                SurfaceState(const SurfaceState &other);
                SurfaceState(SurfaceState &&other) = delete;

                auto AddBuffer(const std::shared_ptr<Buffer> &buffer) -> void;
                auto GetBuffer() const -> std::shared_ptr<Buffer>;

                auto AddSurfaceDamage(int x, int y, size_t width, size_t height) -> void;
                auto AddSurfaceDamage(const Area &area) -> void;
                auto GetSurfaceDamage() const -> std::vector<Area>;

                auto AddBufferDamage(int x, int y, size_t width, size_t height) -> void;
                auto AddBufferDamage(const Area &area) -> void;

                auto GetDamageTracker() const -> pixman_region32_t;
                auto GetDamagedRegions() const -> std::vector<Area>;

                auto SetBufferTransform(const ::wayland::server::output_transform &transform) -> void;
                auto GetBufferTransform() const -> ::wayland::server::output_transform;

                auto GetBufferScale() const -> int;
                auto SetBufferScale(int scale) -> void;

                auto GetLogicalWidth() const -> size_t;
                auto GetLogicalHeight() const -> size_t;

                auto SetBufferOffset(int x, int y) -> void;
                auto GetBufferOffset() const -> std::pair<int, int>;

                auto AddFrameCallback(::wayland::server::callback_t callback) -> void;
                auto GetFrameCallbacks() const -> std::vector<::wayland::server::callback_t>;
                auto SetOpaqueRegion(std::shared_ptr<Region> region) -> void;
                auto GetOpaqueRegion() const -> std::shared_ptr<Region>;

                auto SetInputRegion(std::shared_ptr<Region> region) -> void;
                auto GetInputRegion() const -> std::shared_ptr<Region>;

                // Will not reset the underlying buffer, only resets modifications
                // since last transaction.
                // Buffer changing happens when AddBuffer is called ONLY.
                auto Reset() -> void;

                auto ConvertSurfaceToBuffer(const Area &area) -> Area;

            private:
                // Will update true logical surface information based off of
                // the inverse operations specified in the buffer transform
                // and buffer scale.
                auto UpdateLogicalSurfaceDimensions() -> void;

                std::shared_ptr<Buffer> m_buffer;
                std::vector<Area> m_surfaceDamage;
                pixman_region32_t m_damageTracker;

                ::wayland::server::output_transform m_bufferTransform = ::wayland::server::output_transform::normal;
                int m_bufferScale;

                size_t m_bufferOffsetX;
                size_t m_bufferOffsetY;

                // Uniform logical surface area
                // Calculated by inverse buffer transformatuins
                size_t m_surfaceWidth;
                size_t m_surfaceHeight;

                std::vector<::wayland::server::callback_t> m_frameCallbacks;

                std::shared_ptr<Region> m_opaqueRegion;
                std::shared_ptr<Region> m_inputRegion;
        };

    private:
        Surface(::wayland::server::surface_t surface);

        auto HandleAttach(::wayland::server::buffer_t buffer, int x, int y) -> void;
        auto HandleDamage(int x, int y, size_t width, size_t height) -> void;
        auto HandleFrame(::wayland::server::callback_t callback) -> void;
        auto HandleSetOpaqueRegion(::wayland::server::region_t region) -> void;
        auto HandleSetInputRegion(::wayland::server::region_t region) -> void;
        auto HandleCommit() -> void;
        auto HandleSetBufferTransform(::wayland::server::output_transform transform) -> void;
        auto HandleSetBufferScale(int scale) -> void;
        auto HandleDamageBuffer(int x, int y, size_t width, size_t height) -> void;
        auto HandleOffset(int x, int y) -> void;
        
        SurfaceState m_pendingState;
        std::queue<SurfaceState> m_activeStates;

    };
}  // namespace moco::wayland::implementation
