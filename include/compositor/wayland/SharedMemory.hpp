#pragma once

#include <wayland-server-protocol.hpp>

#include "ObjectImplementationBase.hpp"
#include <SharedMemoryPool.hpp>

namespace moco::wayland::implementation {
    class SharedMemory : private ObjectImplementationBase<::wayland::server::shm_t, SharedMemory> {
        public:
            using ObjectImplementationBase<::wayland::server::shm_t, SharedMemory>::Create;

            SharedMemory(::wayland::server::shm_t shm, Private);

            struct ObjectData : ObjectDataBase {/* Unused (for now) */};

        private:
            SharedMemory(::wayland::server::shm_t shm);

            auto HandleCreatePool(::wayland::server::shm_pool_t pool, int32_t fd, size_t size) -> void;
            auto HandleDestroy() -> void;

            ObjectData_t<> m_objectData;
    };
}  // namespace moco::wayland::implementation
