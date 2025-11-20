#pragma once

#include "ObjectImplementationBase.hpp"

#include "PixelFormat.hpp"

#include <wayland-server-protocol.hpp>
#include <span>
#include <format>

namespace moco::wayland::implementation {
    class SharedMemoryPool;
    class Buffer : public ObjectImplementationBase<::wayland::server::buffer_t, Buffer> {
        public:
            inline Buffer(::wayland::server::buffer_t buffer, auto format, Private) :
                Buffer(buffer, PixelFormats::GetFormat(format)) {}

            template <auto Format>
            inline auto GetData() -> std::span<PixelFormats::PixelFormat<Format>> const {
                if (m_bufferData.size_bytes() % sizeof(Format) != 0) {
                    throw std::runtime_error(std::format("Template type size {} must be able to be aligned with the size ({}) of the data.", sizeof(PixelFormats::PixelFormatSize<Format>::Size), m_bufferData.size_bytes()));
                }

                return {reinterpret_cast<PixelFormats::PixelFormatSize<Format>*>(m_bufferData.data()), m_bufferData.size_bytes() / sizeof(Format)}; 
            }

            inline auto GetFormat() const -> PixelFormats::Format {
                return m_format;
            }

            inline auto GetHeight() const -> size_t {
                return m_bufferHeight;
            }

            inline auto GetWidth() const -> size_t {
                return m_bufferWidth;
            }

            inline auto GetStride() const -> size_t {
                return m_bufferStride;
            }

            inline auto AssignData(std::span<uint8_t> data, std::shared_ptr<SharedMemoryPool> parentPool) -> std::shared_ptr<Buffer> {
                m_bufferData = data;
                m_parentPool = parentPool;

                // Allow call chaining
                return shared_from_this();
            }

            inline auto SetBufferFormat(size_t bufferHeight, size_t bufferWidth, size_t bufferStride) -> std::shared_ptr<Buffer> {
                m_bufferHeight = bufferHeight;
                m_bufferWidth = bufferWidth;
                m_bufferStride = bufferStride;

                // Allow call chaining
                return shared_from_this();
            }
        private:
            inline Buffer(::wayland::server::buffer_t buffer, PixelFormats::Format format) :
                ObjectImplementationBase(buffer),
                m_format(format) {}

            std::shared_ptr<SharedMemoryPool> m_parentPool;

            std::span<uint8_t> m_bufferData;
            PixelFormats::Format m_format;

            size_t m_bufferHeight;
            size_t m_bufferWidth;
            size_t m_bufferStride;
    };
}  // namespace moco::wayland::implementation
