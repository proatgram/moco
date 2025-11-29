#pragma once

#include <wayland-server-protocol.hpp>

#include "ObjectImplementationBase.hpp"

namespace moco::wayland::implementation {
    class SharedMemory : public ObjectImplementationBase<::wayland::server::shm_t, SharedMemory> {
            using ObjectImplementationBase::on_create_pool;
            using ObjectImplementationBase::on_destroy;

            static constexpr std::array<::wayland::server::shm_format, 2> s_supportedFormats = {::wayland::server::shm_format::argb8888, ::wayland::server::shm_format::xrgb8888};

        public:
            SharedMemory(::wayland::server::shm_t shm, Private);

            enum class Error : uint32_t {
                InvalidFormat = 0,
                InvalidStride = 1,
                InvalidFd = 2
            };

            static auto GetSupportedFormats() -> decltype(s_supportedFormats);

        private:

            SharedMemory(::wayland::server::shm_t shm);

            auto HandleCreatePool(::wayland::server::shm_pool_t pool, int32_t fd, size_t size) -> void;
            auto HandleDestroy() -> void;
    };

    class GlobalSharedMemory : private ::wayland::server::global_shm_t {
            using ::wayland::server::global_shm_t::on_bind;
        public:
            GlobalSharedMemory(::wayland::server::display_t display);
            
        private:
            static auto OnBind(::wayland::server::client_t client, ::wayland::server::shm_t shm) -> void;

    };
}  // namespace moco::wayland::implementation
