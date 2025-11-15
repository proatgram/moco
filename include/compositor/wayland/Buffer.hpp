#pragma once

#include "ObjectImplementationBase.hpp"

#include "PixelFormat.hpp"

#include <wayland-server-protocol.hpp>
#include <span>
#include <format>

namespace moco::wayland::implementation {
    class SharedMemoryPool;
    class SharedMemoryBufferBase : public ObjectImplementationBase<::wayland::server::buffer_t, SharedMemoryBufferBase> {
        public:
            inline auto GetFormat() const -> ::wayland::server::shm_format {
                return m_format;
            }

            inline SharedMemoryBufferBase(::wayland::server::buffer_t buffer, ::wayland::server::shm_format format, Private) :
                SharedMemoryBufferBase(buffer, format) {}

        protected:
            inline SharedMemoryBufferBase(::wayland::server::buffer_t buffer, ::wayland::server::shm_format format) :
                ObjectImplementationBase<::wayland::server::buffer_t, SharedMemoryBufferBase>(buffer),
                m_format(format) {}

        private:
            ::wayland::server::shm_format m_format;

    };

    template <::wayland::server::shm_format Format>
    class SharedMemoryBuffer : public SharedMemoryBufferBase {
            using SharedMemoryBufferBase::on_destroy;

        public:
            static inline auto Create(::wayland::server::buffer_t buffer) -> std::shared_ptr<SharedMemoryBuffer<Format>> {
                // Basically just return the actual class we are returning from the base class instantiation
                return std::reinterpret_pointer_cast<SharedMemoryBuffer<Format>>(SharedMemoryBufferBase::Create(buffer, Format));
            }

            inline SharedMemoryBuffer(::wayland::server::buffer_t buffer, Private) :
                SharedMemoryBuffer(buffer) {}

            inline auto AssignData(std::span<uint8_t> data, std::shared_ptr<SharedMemoryPool> parentPool) -> std::shared_ptr<SharedMemoryBuffer<Format>> {
                if (data.size_bytes() % sizeof(PixelFormats::PixelFormat<Format>) != 0) {
                    throw std::invalid_argument(std::format("Byte span size must be a multiple of {} bytes.", sizeof(PixelFormats::PixelFormatSize<Format>)));
                }

                // Reinterprets the raw bytes from the data into a span of type Format
                m_bufferData = {reinterpret_cast<PixelFormats::PixelFormat<Format>*>(data.data()), data.size_bytes() / sizeof(PixelFormats::PixelFormat<Format>)}; 
                m_parentPool = parentPool;

                // Allow call chaining
                return std::reinterpret_pointer_cast<SharedMemoryBuffer<Format>>(user_data().template get<std::shared_ptr<SharedMemoryBufferBase>>());
            }

            inline auto SetBufferFormat(size_t bufferHeight, size_t bufferWidth, size_t bufferStride) -> std::shared_ptr<SharedMemoryBuffer<Format>> {
                m_bufferHeight = bufferHeight;
                m_bufferWidth = bufferWidth;
                m_bufferStride = bufferStride;

                // Allow call chaining
                return std::reinterpret_pointer_cast<SharedMemoryBuffer<Format>>(user_data().template get<std::shared_ptr<SharedMemoryBufferBase>>());
            }

            inline auto GetData() const -> std::span<PixelFormats::PixelFormat<Format>> {
                return m_bufferData;
            }

        private:
            inline SharedMemoryBuffer(::wayland::server::buffer_t buffer) :
               ObjectImplementationBase<::wayland::server::buffer_t, SharedMemoryBuffer<Format>>(buffer)
            {
                this->on_destroy() = [this]() -> void {HandleDestroy();};
            }

            inline auto HandleDestroy() -> void {
                /* Nothing to do (yet) */
            }

            std::span<PixelFormats::PixelFormat<Format>> m_bufferData;
            std::shared_ptr<SharedMemoryPool> m_parentPool;

            size_t m_bufferHeight;
            size_t m_bufferWidth;
            size_t m_bufferStride;
    };
}  // namespace moco::wayland::implementation
