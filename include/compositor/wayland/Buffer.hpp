#pragma once

#include "ObjectImplementationBase.hpp"

#include <wayland-server-protocol.hpp>
#include <span>

namespace moco::wayland::implementation {
    class SharedMemoryPool;
    class SharedMemoryBuffer : private ObjectImplementationBase<::wayland::server::buffer_t, SharedMemoryBuffer> {
        public:
            using ObjectImplementationBase<::wayland::server::buffer_t, SharedMemoryBuffer>::GetObjectData;
            
            struct ObjectData : public ObjectDataBase {
                std::span<uint8_t> bufferData;
                std::shared_ptr<SharedMemoryPool> parentPool;

                ::wayland::server::shm_format bufferFormat;

                size_t bufferHeight;
                size_t bufferWidth;
                size_t bufferStride;
            };

            SharedMemoryBuffer(::wayland::server::buffer_t buffer);

            auto AssignData(std::span<uint8_t> data, std::shared_ptr<SharedMemoryPool> parent) -> void;

            auto SetBufferFormat(size_t bufferHeight, size_t bufferWidth, size_t bufferStride, ::wayland::server::shm_format format) -> void;

            auto GetData() const -> std::span<uint8_t>;

        private:
            auto HandleDestroy() -> void;

            ObjectData_t<> m_objectData;
    };
}  // namespace moco::wayland::implementation
