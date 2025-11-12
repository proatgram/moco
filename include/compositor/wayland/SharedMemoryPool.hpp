#pragma once

#include "ObjectImplementationBase.hpp"
#include "Buffer.hpp"

#include <wayland-server-protocol.hpp>
#include <span>

namespace moco::wayland::implementation {
    class SharedMemoryPool : private ObjectImplementationBase<::wayland::server::shm_pool_t, SharedMemoryPool> {
        private:
        public:
            
            using ObjectImplementationBase<::wayland::server::shm_pool_t, SharedMemoryPool>::Create;
            SharedMemoryPool(::wayland::server::shm_pool_t shm_pool, Private);

            ~SharedMemoryPool();


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

            auto HandleCreateBuffer(::wayland::server::buffer_t buffer, int offset, int width, int height, int stride, ::wayland::server::shm_format format) -> void;           
            auto HandleDestroy() -> void;
            auto HandleResize(size_t size) -> void;

            std::span<uint8_t> memorySpace;
    };
}  // namespace moco::wayland::implementation
