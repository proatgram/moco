#pragma once

#include "ObjectImplementationBase.hpp"
#include "Buffer.hpp"

#include <wayland-server-protocol.hpp>
#include <span>

namespace moco::wayland::implementation {
    class SharedMemoryPool : private ObjectImplementationBase<::wayland::server::shm_pool_t, SharedMemoryPool>, std::enable_shared_from_this<SharedMemoryPool> {
        private:
        public:
            using ObjectImplementationBase<::wayland::server::shm_pool_t, SharedMemoryPool>::GetObjectData;
            using ObjectImplementationBase<::wayland::server::shm_pool_t, SharedMemoryPool>::Create;
            SharedMemoryPool(::wayland::server::shm_pool_t shm_pool, Private);

            ~SharedMemoryPool();

            struct ObjectData : public ObjectDataBase {
                std::span<uint8_t> memorySpace;
            };

            /**
             * @brief Assigns this memory pool a memory space
             * @details Assigns the span of memory to this
             * pools memory space
             *
             * @param data a `std::span<uint8_t>` backing the data
             *
             */ 
            auto Assign(std::span<uint8_t> data) -> void;
        private:

            SharedMemoryPool(::wayland::server::shm_pool_t shm_pool);

            auto HandleCreateBuffer(SharedMemoryBuffer buffer, int offset, int width, int height, int stride, ::wayland::server::shm_format format) -> void;           
            auto HandleDestroy() -> void;
            auto HandleResize(size_t size) -> void;

            ObjectData_t<> m_objectData;
    };
}  // namespace moco::wayland::implementation
