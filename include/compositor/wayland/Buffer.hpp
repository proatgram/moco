#pragma once

#include "ObjectImplementationBase.hpp"

#include <wayland-server-protocol.hpp>
#include <span>

namespace moco::wayland::implementation {
    class SharedMemoryPool;
    class SharedMemoryBuffer : private ObjectImplementationBase<::wayland::server::buffer_t, SharedMemoryBuffer> {
        public:
            using ObjectImplementationBase<::wayland::server::buffer_t, SharedMemoryBuffer>::Create;

            SharedMemoryBuffer(::wayland::server::buffer_t buffer, Private);

            auto AssignData(std::span<uint8_t> data, std::shared_ptr<SharedMemoryPool> parentPool) -> void;

            auto SetBufferFormat(size_t bufferHeight, size_t bufferWidth, size_t bufferStride, ::wayland::server::shm_format format) -> void;

            auto GetData() const -> std::span<uint8_t>;

        private:
            SharedMemoryBuffer(::wayland::server::buffer_t buffer);

            auto HandleDestroy() -> void;

            std::span<uint8_t> bufferData;
            std::shared_ptr<SharedMemoryPool> parentPool;

            ::wayland::server::shm_format bufferFormat;

            size_t bufferHeight;
            size_t bufferWidth;
            size_t bufferStride;
    };
}  // namespace moco::wayland::implementation
